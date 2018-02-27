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

#include <WCDB/BuiltinStatement.hpp>

namespace WCDB {

const StatementPragma BuiltinStatement::getJournalMode =
    StatementPragma().pragma(Pragma::JournalMode);

const StatementPragma BuiltinStatement::getLockingMode =
    StatementPragma().pragma(Pragma::LockingMode);

const StatementPragma BuiltinStatement::setLockingModeNormal =
    StatementPragma().pragma(Pragma::LockingMode, "NORMAL");

const StatementPragma BuiltinStatement::setSynchronousNormal =
    StatementPragma().pragma(Pragma::Synchronous, "NORMAL");

const StatementPragma BuiltinStatement::setJournalModeWAL =
    StatementPragma().pragma(Pragma::JournalMode, "WAL");

const StatementPragma BuiltinStatement::setFullFSync =
    StatementPragma().pragma(Pragma::Fullfsync, true);

const StatementSelect BuiltinStatement::fts3Tokenizer =
    StatementSelect().select(Expression::Function(
        "fts3_tokenizer", std::list<Expression>(2, BindParameter())));

const StatementPragma BuiltinStatement::checkpoint =
    StatementPragma().pragma(Pragma::WalCheckpoint);

} //namespace WCDB
