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

#include "Handle.hpp"
#include "InnerDatabase.hpp"
#include "InnerHandle.hpp"
#include "RecyclableHandle.hpp"

#define GetInnerHandleOrReturnValue(value)                                     \
    InnerHandle* handle = getOrGenerateHandle();                               \
    if (handle == nullptr) {                                                   \
        return value;                                                          \
    }

#define GetInnerHandleOrReturn                                                 \
    InnerHandle* handle = getOrGenerateHandle();                               \
    if (handle == nullptr) {                                                   \
        return;                                                                \
    }

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
        if (m_handleHolder != nullptr) {
            m_innerHandle = m_handleHolder.get();
        }
    }
    return m_innerHandle;
}

HandleStatement* Handle::getInnerHandleStatement()
{
    GetInnerHandleOrReturnValue(nullptr);
    return handle->m_mainStatement;
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
    m_innerHandle = nullptr;
    m_handleHolder = nullptr;
}

long long Handle::getLastInsertedRowID()
{
    GetInnerHandleOrReturnValue(0);
    return handle->getLastInsertedRowID();
}

int Handle::getChanges()
{
    GetInnerHandleOrReturnValue(0);
    return handle->getChanges();
}

int Handle::getTotalChange()
{
    GetInnerHandleOrReturnValue(0);
    return handle->getTotalChange();
}

const Error& Handle::getError()
{
    if (m_innerHandle != nullptr) {
        return m_innerHandle->getError();
    }
    if (m_databaseHolder != nullptr) {
        return m_databaseHolder->getThreadedError();
    }
    static Error* error = new Error();
    return *error;
}

OptionalPreparedStatement Handle::getOrCreatePreparedStatement(const Statement& statement)
{
    OptionalPreparedStatement result;
    GetInnerHandleOrReturnValue(result);
    HandleStatement* preparedStatement = handle->getOrCreatePreparedStatement(statement);
    if (preparedStatement == nullptr) {
        return result;
    }
    return PreparedStatement(preparedStatement);
}

void Handle::finalizeAllStatement()
{
    GetInnerHandleOrReturn;
    handle->finalizeStatements();
}

void Handle::attachCancellationSignal(const CancellationSignal& signal)
{
    GetInnerHandleOrReturn;
    handle->attachCancellationSignal(signal.m_signal);
}

void Handle::detachCancellationSignal()
{
    GetInnerHandleOrReturn;
    handle->detachCancellationSignal();
}

Handle::CancellationSignal::CancellationSignal()
: m_signal(std::make_shared<bool>(false)){};

Handle::CancellationSignal::~CancellationSignal() = default;

void Handle::CancellationSignal::cancel()
{
    *m_signal = true;
}

} //namespace WCDB
