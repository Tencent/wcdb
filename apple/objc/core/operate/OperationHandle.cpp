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
#include <WCDB/CoreConst.h>
#include <WCDB/Handle.hpp>
#include <WCDB/OperationHandle.hpp>
#include <WCDB/RepairKit.h>

namespace WCDB {

OperationHandle::OperationHandle()
: m_statementForAcquireReadLock(
StatementSelect().select(1).from(Syntax::masterTable).limit(0))
, m_statementForReadTransaction(StatementBegin().beginDeferred())
, m_statementForIntegrityCheck(
  StatementPragma().pragma(Pragma::integrityCheck()).schema(Schema::main()))
{
}

void OperationHandle::setType(HandleType type)
{
    HandleCategory category = categoryOfHandleType(type);
    switch (category) {
    case HandleCategoryIntegrity:
        m_error.infos.insert_or_assign(ErrorStringKeyAction, ErrorActionIntegrity);
        break;
    case HandleCategoryBackupRead:
    case HandleCategoryBackupWrite:
        m_error.infos.insert_or_assign(ErrorStringKeyAction, ErrorActionBackup);
        break;
    default:
        WCTAssert(category == HandleCategoryCheckpoint);
        m_error.infos.insert_or_assign(ErrorStringKeyAction, ErrorActionCheckpoint);
        break;
    }
}

#pragma mark - Checkpoint
bool OperationHandle::checkpoint()
{
    WCTAssert(m_error.infos.find(UnsafeStringView(ErrorStringKeyAction))
              != m_error.infos.end());
    WCTAssert(m_error.infos.at(ErrorStringKeyAction).valueType()
              == Error::InfoValue::Type::StringView);
    WCTAssert(m_error.infos.at(ErrorStringKeyAction).stringValue() == ErrorActionCheckpoint);
    return Handle::checkpoint(CheckpointMode::Passive);
}

#pragma mark - Integrity
void OperationHandle::checkIntegrity()
{
    WCTAssert(m_error.infos.find(UnsafeStringView(ErrorStringKeyAction))
              != m_error.infos.end());
    WCTAssert(m_error.infos.at(ErrorStringKeyAction).valueType()
              == Error::InfoValue::Type::StringView);
    WCTAssert(m_error.infos.at(ErrorStringKeyAction).stringValue() == ErrorActionIntegrity);
    execute(m_statementForIntegrityCheck);
}

#pragma mark - Backup
void OperationHandle::setPath(const UnsafeStringView &path)
{
    Handle::setPath(path);
}

const StringView &OperationHandle::getPath() const
{
    return Handle::getPath();
}

const Error &OperationHandle::getError() const
{
    return Handle::getError();
}

bool OperationHandle::acquireReadLock()
{
    WCTAssert(m_error.infos.find(UnsafeStringView(ErrorStringKeyAction))
              != m_error.infos.end());
    WCTAssert(m_error.infos.at(ErrorStringKeyAction).valueType()
              == Error::InfoValue::Type::StringView);
    WCTAssert(m_error.infos.at(ErrorStringKeyAction).stringValue() == ErrorActionBackup);

    return execute(m_statementForReadTransaction) && execute(m_statementForAcquireReadLock);
}

bool OperationHandle::releaseReadLock()
{
    rollbackTransaction();
    return true;
}

bool OperationHandle::acquireWriteLock()
{
    WCTAssert(m_error.infos.find(UnsafeStringView(ErrorStringKeyAction))
              != m_error.infos.end());
    WCTAssert(m_error.infos.at(ErrorStringKeyAction).valueType()
              == Error::InfoValue::Type::StringView);
    WCTAssert(m_error.infos.at(ErrorStringKeyAction).stringValue() == ErrorActionBackup);

    return beginTransaction();
}

bool OperationHandle::releaseWriteLock()
{
    rollbackTransaction();
    return true;
}

} // namespace WCDB
