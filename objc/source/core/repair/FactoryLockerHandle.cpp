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

#include <WCDB/FactoryLockerHandle.hpp>

namespace WCDB {

void FactoryReadLockerHandle::setPath(const std::string &path)
{
    Handle::setPath(path);
}

const std::string &FactoryReadLockerHandle::getPath() const
{
    return Handle::getPath();
}

bool FactoryReadLockerHandle::acquireLock()
{
    if (!open()) {
        return false;
    }
    if (!execute(StatementBegin::deferred())) {
        return false;
    }
    return execute(StatementSelect().select(1).from("sqlite_master").limit(0));
}

bool FactoryReadLockerHandle::releaseLock()
{
    if (isInTransaction()) {
        rollbackTransaction();
    }
    close();
    return true;
}

const Error &FactoryReadLockerHandle::getError() const
{
    return Handle::getError();
}

void FactoryWriteLockerHandle::setPath(const std::string &path)
{
    Handle::setPath(path);
}

const std::string &FactoryWriteLockerHandle::getPath() const
{
    return Handle::getPath();
}

bool FactoryWriteLockerHandle::acquireLock()
{
    return open() && beginTransaction();
}

bool FactoryWriteLockerHandle::releaseLock()
{
    if (isInTransaction()) {
        rollbackTransaction();
    }
    close();
    return true;
}

const Error &FactoryWriteLockerHandle::getError() const
{
    return Handle::getError();
}

} // namespace WCDB
