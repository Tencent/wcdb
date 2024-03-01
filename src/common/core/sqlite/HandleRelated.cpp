//
// Created by sanhuazhang on 2019/05/19
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

#include "HandleRelated.hpp"
#include "Assertion.hpp"
#include "InnerHandle.hpp"

namespace WCDB {

HandleRelated::HandleRelated(AbstractHandle *handle) : m_handle(handle)
{
    WCTAssert(m_handle != nullptr);
}

HandleRelated::~HandleRelated() = default;

bool HandleRelated::APIExit(int rc)
{
    return m_handle->APIExit(rc);
}

bool HandleRelated::APIExit(int rc, const UnsafeStringView &sql)
{
    return m_handle->APIExit(rc, sql);
}

bool HandleRelated::APIExit(int rc, const char *sql)
{
    return m_handle->APIExit(rc, sql);
}

AbstractHandle *HandleRelated::getHandle() const
{
    return m_handle;
}

sqlite3 *HandleRelated::getRawHandle()
{
    return m_handle->getRawHandle();
}

void HandleRelated::cacheCurrentTransactionError()
{
    m_handle->cacheCurrentTransactionError();
}
void HandleRelated::resumeCacheTransactionError()
{
    m_handle->resumeCacheTransactionError();
}

bool HandleRelated::isBusyTraceEnable() const
{
    return m_handle->isBusyTraceEnable();
}

void HandleRelated::setCurrentSQL(const UnsafeStringView &sql)
{
    m_handle->setCurrentSQL(sql);
}

void HandleRelated::resetCurrentSQL(const UnsafeStringView &sql)
{
    m_handle->resetCurrentSQL(sql);
}

} //namespace WCDB
