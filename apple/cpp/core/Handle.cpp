//
// Created by qiuwenchen on 2022/8/3.
//

/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <WCDB/Handle.hpp>
#include <WCDB/InnerDatabase.hpp>
#include <WCDB/InnerHandle.hpp>
#include <WCDB/RecyclableHandle.hpp>

namespace WCDB {

Handle::Handle(Recyclable<InnerDatabase*> database)
: m_databaseHolder(database), m_innerHandle(nullptr)
{
}

Handle::Handle(RecyclableHandle handle)
: m_handleHolder(handle), m_innerHandle(handle.get())
{
}

Handle::Handle(Recyclable<InnerDatabase*> database, InnerHandle* handle)
: m_databaseHolder(database), m_innerHandle(handle)
{
}

Handle::Handle(Handle&& other)
: m_databaseHolder(std::move(other.m_databaseHolder))
, m_handleHolder(std::move(other.m_handleHolder))
, m_innerHandle(other.m_innerHandle)
, m_handleStatementDic(std::move(other.m_handleStatementDic))
{
}

Handle::~Handle()
{
    invalidate();
};

InnerHandle* Handle::getOrGenerateHandle()
{
    if (m_innerHandle == nullptr) {
        m_handleHolder = m_databaseHolder->getHandle();
        m_innerHandle = m_handleHolder.get();
    }
    WCTAssert(m_innerHandle != nullptr);
    return m_innerHandle;
}

InnerHandleStatement* Handle::getInnerHandleStatement()
{
    return getOrGenerateHandle()->m_mainStatement;
}

RecyclableHandle Handle::getHandleHolder()
{
    getOrGenerateHandle();
    if (m_handleHolder != nullptr) {
        return m_handleHolder;
    } else {
        return RecyclableHandle(
        std::shared_ptr<InnerHandle>(m_innerHandle, [](InnerHandle*) {}), nullptr);
    }
}

Recyclable<InnerDatabase*> Handle::getDatabaseHolder()
{
    WCTAssert(m_databaseHolder != nullptr);
    return m_databaseHolder;
}

void Handle::invalidate()
{
    finalizeAllStatement();
    m_innerHandle = nullptr;
    m_handleHolder = nullptr;
}

long long Handle::getLastInsertedRowID()
{
    return getOrGenerateHandle()->getLastInsertedRowID();
}

int Handle::getChanges()
{
    return getOrGenerateHandle()->getChanges();
}

int Handle::getTotalChange()
{
    return getOrGenerateHandle()->getTotalChange();
}

const Error& Handle::getError()
{
    return getOrGenerateHandle()->getError();
}

HandleStatement& Handle::getOrCreateHandleStatementWithTag(const UnsafeStringView& tag)
{
    auto iter = m_handleStatementDic.find(tag);
    if (iter == m_handleStatementDic.end()) {
        iter = m_handleStatementDic
               .try_emplace(StringView(tag),
                            HandleStatement(getOrGenerateHandle()->getStatement()))
               .first;
        iter->second.m_tag = tag;
    }
    return iter->second;
}

void Handle::finalizeAllStatement()
{
    InnerHandle* handle = getOrGenerateHandle();
    for (auto iter = m_handleStatementDic.begin(); iter != m_handleStatementDic.end(); iter++) {
        iter->second.finalize();
        handle->returnStatement(iter->second.m_innerHandleStatement);
    }
    m_handleStatementDic.clear();
    if (m_innerHandle != nullptr) {
        m_innerHandle->finalize();
    }
}

} //namespace WCDB
