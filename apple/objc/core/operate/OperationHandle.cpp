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
    switch (type) {
    case HandleType::OperationIntegrity:
        m_error.infos.insert_or_assign(ErrorStringKeyAction, ErrorActionIntegrity);
        break;
    case HandleType::OperationBackup:
        m_error.infos.insert_or_assign(ErrorStringKeyAction, ErrorActionBackup);
        break;
    case HandleType::OperationCheckpoint:
        m_error.infos.insert_or_assign(ErrorStringKeyAction, ErrorActionCheckpoint);
        break;
    default:
        WCTInnerAssert(false);
        break;
    }
}

#pragma mark - Checkpoint
bool OperationHandle::checkpoint()
{
    WCTInnerAssert(m_error.infos.find(UnsafeStringView(ErrorStringKeyAction))
                   != m_error.infos.end());
    WCTInnerAssert(m_error.infos.at(ErrorStringKeyAction).valueType()
                   == Error::InfoValue::Type::StringView);
    WCTInnerAssert(m_error.infos.at(ErrorStringKeyAction).stringValue() == ErrorActionCheckpoint);
    return Handle::checkpoint(CheckpointMode::Passive);
}

#pragma mark - Integrity
void OperationHandle::checkIntegrity()
{
    WCTInnerAssert(m_error.infos.find(UnsafeStringView(ErrorStringKeyAction))
                   != m_error.infos.end());
    WCTInnerAssert(m_error.infos.at(ErrorStringKeyAction).valueType()
                   == Error::InfoValue::Type::StringView);
    WCTInnerAssert(m_error.infos.at(ErrorStringKeyAction).stringValue() == ErrorActionIntegrity);
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
    WCTInnerAssert(m_error.infos.find(UnsafeStringView(ErrorStringKeyAction))
                   != m_error.infos.end());
    WCTInnerAssert(m_error.infos.at(ErrorStringKeyAction).valueType()
                   == Error::InfoValue::Type::StringView);
    WCTInnerAssert(m_error.infos.at(ErrorStringKeyAction).stringValue() == ErrorActionBackup);

    return execute(m_statementForReadTransaction) && execute(m_statementForAcquireReadLock);
}

bool OperationHandle::releaseReadLock()
{
    rollbackTransaction();
    return true;
}

bool OperationHandle::acquireWriteLock()
{
    WCTInnerAssert(m_error.infos.find(UnsafeStringView(ErrorStringKeyAction))
                   != m_error.infos.end());
    WCTInnerAssert(m_error.infos.at(ErrorStringKeyAction).valueType()
                   == Error::InfoValue::Type::StringView);
    WCTInnerAssert(m_error.infos.at(ErrorStringKeyAction).stringValue() == ErrorActionBackup);

    return beginTransaction();
}

bool OperationHandle::releaseWriteLock()
{
    rollbackTransaction();
    return true;
}

} // namespace WCDB
