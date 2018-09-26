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

#include <WCDB/Assertion.hpp>
#include <WCDB/MigrationHandle.hpp>

namespace WCDB {

#pragma mark - Initialize
MigrationHandle::MigrationHandle(const Migration *const infos)
: Handle(), m_infos(infos)
{
    WCTInnerAssert(m_infos != nullptr);
}

#pragma mark - Override
bool MigrationHandle::execute(const Statement &statement)
{
    return Handle::execute(statement);
}

bool MigrationHandle::prepare(const Statement &statement)
{
    return Handle::prepare(statement);
}

bool MigrationHandle::step(bool &done)
{
    return Handle::step(done);
}

void MigrationHandle::reset()
{
    Handle::reset();
}

void MigrationHandle::bindInteger32(const Integer32 &value, int index)
{
    Handle::bindInteger32(value, index);
}

void MigrationHandle::bindInteger64(const Integer64 &value, int index)
{
    Handle::bindInteger64(value, index);
}

void MigrationHandle::bindDouble(const Float &value, int index)
{
    Handle::bindDouble(value, index);
}

void MigrationHandle::bindText(const Text &value, int length, int index)
{
    Handle::bindText(value, length, index);
}

void MigrationHandle::bindBLOB(const BLOB &value, int index)
{
    Handle::bindBLOB(value, index);
}

void MigrationHandle::bindNull(int index)
{
    Handle::bindNull(index);
}

void MigrationHandle::finalize()
{
    Handle::finalize();
}

} //namespace WCDB
