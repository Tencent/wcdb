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

#include "OperationHandle.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "InnerHandle.hpp"
#include "Notifier.hpp"
#include "RepairKit.h"

namespace WCDB {

OperationHandle::OperationHandle()
: BackupRelatedHandle()
, m_statementForIntegrityCheck(
  StatementPragma().pragma(Pragma::integrityCheck()).schema(Schema::main()).with(1))
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
    if (optionalIntegrityMessages.succeed()) {
        auto &integrityMessages = optionalIntegrityMessages.value();
        WCTAssert(integrityMessages.size() == 1);
        if (integrityMessages.size() > 0) {
            auto integrityMessage = *integrityMessages.begin();
            if (!integrityMessage.caseInsensitiveEqual("ok")) {
                Error error(Error::Code::Corrupt, Error::Level::Warning, integrityMessage);
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
    Optional<std::set<StringView>> ftsTableSet = getValues(m_statementForGetFTSTable, 0);
    if (!ftsTableSet.succeed()) {
        return;
    }
    for (const StringView &ftsTable : ftsTableSet.value()) {
        if (executeStatement(
            StatementInsert().insertIntoTable(ftsTable).column(Column(ftsTable)).value("integrity-check"))) {
            continue;
        }
        if (Error::rc2ec((int) m_error.getExtCode()) == Error::ExtCode::CorruptVirtualTable) {
            Error error(Error::Code::Corrupt, Error::Level::Warning, m_error.getMessage());
            error.infos.insert_or_assign(ErrorStringKeyPath, getPath());
            error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeIntegrity);
            Notifier::shared().notify(error);
            break;
        }
    }
}

} // namespace WCDB
