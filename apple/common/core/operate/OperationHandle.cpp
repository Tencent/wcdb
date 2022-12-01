//
// Created by sanhuazhang on 2019/06/06
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

#include <WCDB/Assertion.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/InnerHandle.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/OperationHandle.hpp>
#include <WCDB/RepairKit.h>

namespace WCDB {

OperationHandle::OperationHandle()
: m_statementForIntegrityCheck(
StatementPragma().pragma(Pragma::integrityCheck()).with(1).schema(Schema::main()))
, m_statementForReadTransaction(StatementBegin().beginDeferred())
, m_statementForAcquireReadLock(
  StatementSelect().select(1).from(Syntax::masterTable).limit(0))
, m_statementForGetFTSTable(
  StatementSelect()
  .select(Column("name"))
  .from(Syntax::masterTable)
  .where(Column("type") == "table" && Column("sql").like("CREATE VIRTUAL TABLE % USING fts%")))
{
}

OperationHandle::~OperationHandle() = default;

void OperationHandle::doSuspend(bool suspend)
{
    AbstractHandle::doSuspend(suspend);
    BackupExclusiveDelegate::suspendBackup(suspend);
}

#pragma mark - Integrity
void OperationHandle::checkIntegrity()
{
    auto optionalIntegrityMessages = getValues(m_statementForIntegrityCheck, 0);
    bool needCheckFTS = true;
    if (optionalIntegrityMessages.has_value()) {
        auto &integrityMessages = optionalIntegrityMessages.value();
        WCTAssert(integrityMessages.size() == 1);
        if (integrityMessages.size() > 0) {
            auto integrityMessage = *integrityMessages.begin();
            if (!integrityMessage.caseInsensiveEqual("ok")) {
                Error error(Error::Code::Corrupt, Error::Level::Error, integrityMessage);
                error.infos.insert_or_assign(ErrorStringKeyPath, getPath());
                error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeIntegrity);
                Notifier::shared().notify(error);
                needCheckFTS = false;
            }
        }
    }
    if (!needCheckFTS) {
        return;
    }
    std::optional<std::set<StringView>> ftsTableSet
    = getValues(m_statementForGetFTSTable, 0);
    if (!ftsTableSet.has_value()) {
        return;
    }
    for (const StringView &ftsTable : ftsTableSet.value()) {
        if (executeStatement(
            StatementInsert().insertIntoTable(ftsTable).column(Column(ftsTable)).value("integrity-check"))) {
            continue;
        }
        if (Error::rc2ec((int) m_error.infos.at(ErrorIntKeyExtCode).integerValue())
            == Error::ExtCode::CorruptVirtualTable) {
            Error error(Error::Code::Corrupt, Error::Level::Error, m_error.getMessage());
            error.infos.insert_or_assign(ErrorStringKeyPath, getPath());
            error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeIntegrity);
            Notifier::shared().notify(error);
            break;
        }
    }
}

#pragma mark - Backup
void OperationHandle::setBackupPath(const UnsafeStringView &path)
{
    InnerHandle::setPath(path);
}

const StringView &OperationHandle::getBackupPath() const
{
    return InnerHandle::getPath();
}

const Error &OperationHandle::getBackupError() const
{
    return InnerHandle::getError();
}

bool OperationHandle::acquireBackupSharedLock()
{
    return execute(m_statementForReadTransaction) && execute(m_statementForAcquireReadLock);
}

bool OperationHandle::releaseBackupSharedLock()
{
    rollbackTransaction();
    return true;
}

bool OperationHandle::acquireBackupExclusiveLock()
{
    return beginTransaction();
}

bool OperationHandle::releaseBackupExclusiveLock()
{
    rollbackTransaction();
    return true;
}

void OperationHandle::finishBackup()
{
    InnerHandle::close();
}

#pragma mark - Cipher

const Error &OperationHandle::getCipherError() const
{
    return InnerHandle::getError();
}

bool OperationHandle::openCipherInMemory(bool onlyUsedCipherKey)
{
    InnerHandle::setPath(":memory:");
    if (onlyUsedCipherKey) {
        return InnerHandle::openPureCipherDB();
    } else {
        return InnerHandle::open();
    }
}

bool OperationHandle::isCipherDB() const
{
    return InnerHandle::isCipherDB();
}

void OperationHandle::closeCipher()
{
    InnerHandle::close();
}

void *OperationHandle::getCipherContext()
{
    if (!isOpened()) {
        return nullptr;
    }
    return AbstractHandle::getCipherContext();
}

size_t OperationHandle::getCipherPageSize()
{
    return AbstractHandle::getCipherPageSize();
}

StringView OperationHandle::getCipherSalt()
{
    return AbstractHandle::getCipherSalt();
}

bool OperationHandle::setCipherSalt(const UnsafeStringView &salt)
{
    return AbstractHandle::setCipherSalt(salt);
}

} // namespace WCDB
