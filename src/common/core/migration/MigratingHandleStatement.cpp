//
// Created by qiuwenchen on 2020/3/6.
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

#include "MigratingHandleStatement.hpp"
#include "Assertion.hpp"
#include "Core.hpp"
#include "MigratingHandle.hpp"
#include "SQLite.h"
#include "StringView.hpp"
#include "WINQ.h"

namespace WCDB {
MigratingHandleStatement::MigratingHandleStatement(MigratingHandleStatement&& other)
: HandleStatement(std::move(other))
, m_processing(other.m_processing)
, m_currentStatementType(other.m_currentStatementType)
, m_additionalStatements(std::move(other.m_additionalStatements))
, m_migratingInfo(other.m_migratingInfo)
, m_assignedPrimaryKey(std::move(other.m_assignedPrimaryKey))
, m_primaryKeyIndex(other.m_primaryKeyIndex)
{
    other.m_processing = false;
    other.m_currentStatementType = StatementType::Invalid;
    other.m_additionalStatements.clear();
    other.m_migratingInfo = nullptr;
    other.m_assignedPrimaryKey = NullOpt;
    other.m_primaryKeyIndex = 0;
}

MigratingHandleStatement::MigratingHandleStatement(MigratingHandle* handle)
: HandleStatement(handle)
, m_processing(false)
, m_currentStatementType(StatementType::Invalid)
, m_migratingInfo(nullptr)
, m_primaryKeyIndex(0)
{
}

MigratingHandleStatement::~MigratingHandleStatement()
{
    finalize();
}

#pragma mark - Migration
Optional<std::list<Statement>> MigratingHandleStatement::process(const Statement& originStatement)
{
    m_processing = true;
    bool succeed = true;
    MigratingHandle* migratingHandle = dynamic_cast<MigratingHandle*>(getHandle());
    WCTAssert(migratingHandle != nullptr);
    std::list<Statement> statements;
    do {
        migratingHandle->startBinding();

        // It's dangerous to use origin statement after tampering since all the tokens are not fit.
        Statement falledBackStatement = originStatement;
        // Clear sql cache in statement
        falledBackStatement.syntax();
        // fallback
        falledBackStatement.iterate([&succeed, this, migratingHandle, &originStatement](
                                    Syntax::Identifier& identifier, bool& stop) {
            switch (identifier.getType()) {
            case Syntax::Identifier::Type::TableOrSubquery: {
                // main.table -> temp.unionedView
                Syntax::TableOrSubquery& syntax = (Syntax::TableOrSubquery&) identifier;
                if (syntax.switcher == Syntax::TableOrSubquery::Switch::Table) {
                    succeed = tryFallbackToUnionedView(syntax.schema, syntax.tableOrFunction);
                }
            } break;
            case Syntax::Identifier::Type::QualifiedTableName: {
                // main.table -> schemaForSourceDatabase.sourceTable
                Syntax::QualifiedTableName& syntax = (Syntax::QualifiedTableName&) identifier;
                succeed = tryFallbackToSourceTable(syntax.schema, syntax.table);
            } break;
            case Syntax::Identifier::Type::InsertSTMT: {
                // main.table -> schemaForSourceDatabase.sourceTable
                Syntax::InsertSTMT& syntax = (Syntax::InsertSTMT&) identifier;
                succeed = tryFallbackToSourceTable(syntax.schema, syntax.table);
            } break;
            case Syntax::Identifier::Type::DropTableSTMT: {
                // main.table -> schemaForSourceDatabase.sourceTable
                Syntax::DropTableSTMT& syntax = (Syntax::DropTableSTMT&) identifier;
                succeed = tryFallbackToSourceTable(syntax.schema, syntax.table);
            } break;
            case Syntax::Identifier::Type::AlterTableSTMT: {
                // main.table -> schemaForSourceDatabase.sourceTable
                Syntax::AlterTableSTMT& syntax = (Syntax::AlterTableSTMT&) identifier;
                succeed = tryFallbackToSourceTable(syntax.schema, syntax.table);
            } break;
            case Syntax::Identifier::Type::Expression: {
                // main.table -> temp.unionedView
                Syntax::Expression& syntax = (Syntax::Expression&) identifier;
                switch (syntax.switcher) {
                case Syntax::Expression::Switch::Column:
                    succeed = tryFallbackToUnionedView(syntax.column().schema,
                                                       syntax.column().table);
                    break;
                case Syntax::Expression::Switch::In:
                    if (syntax.inSwitcher == Syntax::Expression::SwitchIn::Table) {
                        succeed
                        = tryFallbackToUnionedView(syntax.schema(), syntax.table());
                    }
                    break;
                default:
                    break;
                }
            } break;
            case Syntax::Identifier::Type::CreateTableSTMT: {
                Syntax::CreateTableSTMT& syntax = (Syntax::CreateTableSTMT&) identifier;
                if (!migratingHandle->hintThatTableWillBeCreated(syntax.table)) {
                    succeed = false;
                }
            } break;
            case Syntax::Identifier::Type::BindParameter: {
                Syntax::BindParameter& bindParameter = (Syntax::BindParameter&) identifier;
                if (bindParameter.switcher != Syntax::BindParameter::Switch::QuestionSign) {
                    getHandle()->notifyError(
                    (int) Error::Code::Error,
                    originStatement.getDescription().data(),
                    "Only numeric bind parameters are allowed to used in the migrating database");
                    succeed = false;
                } else if (bindParameter.n <= 0) {
                    getHandle()->notifyError(
                    (int) Error::Code::Error,
                    originStatement.getDescription().data(),
                    "The indexes of bind parameters must be assigned in the migrating database");
                    succeed = false;
                }
            } break;
            default:
                break;
            }
            if (!succeed) {
                stop = true;
            }
        });
        succeed = migratingHandle->stopBinding(succeed);
        if (!succeed) {
            break;
        }

        switch (originStatement.getType()) {
        case Syntax::Identifier::Type::InsertSTMT: {
            const Syntax::InsertSTMT& migratedInsertSTMT
            = static_cast<const Syntax::InsertSTMT&>(originStatement.syntax());
            const Syntax::InsertSTMT& falledBackSTMT
            = static_cast<const Syntax::InsertSTMT&>(falledBackStatement.syntax());
            WCTRemedialAssert(!migratedInsertSTMT.columns.empty(),
                              "Insert statement that does not explicitly indicate columns is not supported while using migration feature.",
                              succeed = false;
                              break;);
            WCTRemedialAssert(!migratedInsertSTMT.isMultiWrite(),
                              "Insert statement that contains multiple values is not supported while using migration feature.",
                              succeed = false;
                              break;);
            if (migratedInsertSTMT.isTargetingSameTable(falledBackSTMT)) {
                statements.push_back(falledBackStatement);
            } else {
                clearMigrateStatus();
                const MigrationInfo* info
                = migratingHandle->getBoundInfo(migratedInsertSTMT.table);
                WCTAssert(info != nullptr);
                m_migratingInfo = info;
                info->generateStatementsForInsertMigrating(
                falledBackStatement, statements, m_primaryKeyIndex, m_assignedPrimaryKey);
                sqlite3_revertCommitOrder(getRawHandle());
            }
        } break;
        case Syntax::Identifier::Type::UpdateSTMT: {
            const Syntax::QualifiedTableName& migratedTable
            = static_cast<const Syntax::UpdateSTMT&>(originStatement.syntax()).table;
            const Syntax::QualifiedTableName& falledBackTable
            = static_cast<const Syntax::UpdateSTMT&>(falledBackStatement.syntax())
              .table;
            if (migratedTable.isTargetingToSameTable(falledBackTable)) {
                statements.push_back(falledBackStatement);
            } else {
                const UnsafeStringView& migratedTableName = migratedTable.table;
                const MigrationInfo* info = migratingHandle->getBoundInfo(migratedTableName);
                WCTAssert(info != nullptr);
                info->generateStatementsForUpdateMigrating(falledBackStatement, statements);
            }
        } break;
        case Syntax::Identifier::Type::DeleteSTMT: {
            const Syntax::QualifiedTableName& migratedTable
            = static_cast<const Syntax::DeleteSTMT&>(originStatement.syntax()).table;
            const Syntax::QualifiedTableName& falledBackTable
            = static_cast<const Syntax::DeleteSTMT&>(falledBackStatement.syntax())
              .table;
            if (migratedTable.isTargetingToSameTable(falledBackTable)) {
                statements.push_back(falledBackStatement);
            } else {
                const UnsafeStringView& migratedTableName = migratedTable.table;
                const MigrationInfo* info = migratingHandle->getBoundInfo(migratedTableName);
                WCTAssert(info != nullptr);
                info->generateStatementsForDeleteMigrating(falledBackStatement, statements);
            }
        } break;
        case Syntax::Identifier::Type::DropTableSTMT: {
            const Syntax::DropTableSTMT& migratedSTMT
            = static_cast<const Syntax::DropTableSTMT&>(originStatement.syntax());
            const Syntax::DropTableSTMT& falledBackSTMT
            = static_cast<const Syntax::DropTableSTMT&>(falledBackStatement.syntax());
            statements.push_back(originStatement);
            if (!migratedSTMT.isTargetingSameTable(falledBackSTMT)) {
                // Don't drop source table. Instead, delete all contents from source table and wait the stepper do the dropping work.
                Syntax::DropTableSTMT& stmt
                = static_cast<Syntax::DropTableSTMT&>(statements.back().syntax());
                stmt.schema = Schema::main();
                const MigrationInfo* info
                = migratingHandle->getBoundInfo(migratedSTMT.table);
                WCTAssert(info != nullptr);
                statements.push_back(info->getStatementForDeletingFromTable(falledBackStatement));
            }
        } break;
        case Syntax::Identifier::Type::AlterTableSTMT: {
            statements.push_back(falledBackStatement);
            const Syntax::AlterTableSTMT& migratedSTMT
            = static_cast<const Syntax::AlterTableSTMT&>(originStatement.syntax());
            const Syntax::AlterTableSTMT& falledBackSTMT
            = static_cast<const Syntax::AlterTableSTMT&>(falledBackStatement.syntax());
            if (!migratedSTMT.isTargetingSameTable(falledBackSTMT)) {
                statements.push_back(originStatement);
                Syntax::AlterTableSTMT& stmt
                = static_cast<Syntax::AlterTableSTMT&>(statements.back().syntax());
                stmt.schema = Schema::main();
            }
        } break;
        default:
            statements.push_back(falledBackStatement);
            break;
        }
    } while (false);
    m_processing = false;
    if (succeed) {
        m_currentStatementType = originStatement.getType();
        return std::move(statements);
    } else {
        return NullOpt;
    }
}

bool MigratingHandleStatement::tryFallbackToUnionedView(Syntax::Schema& schema, StringView& table)
{
    if (schema.isMain()) {
        MigratingHandle* migratingHandle = dynamic_cast<MigratingHandle*>(getHandle());
        WCTAssert(migratingHandle != nullptr);
        auto optionalInfo = migratingHandle->bindTable(table);
        if (!optionalInfo.succeed()) {
            return false;
        }
        const MigrationInfo* info = optionalInfo.value();
        if (info != nullptr) {
            schema = Schema::temp();
            table = info->getUnionedView();
        }
    }
    return true;
}

bool MigratingHandleStatement::tryFallbackToSourceTable(Syntax::Schema& schema, StringView& table)
{
    if (schema.isMain()) {
        MigratingHandle* migratingHandle = dynamic_cast<MigratingHandle*>(getHandle());
        WCTAssert(migratingHandle != nullptr);
        auto optionalInfo = migratingHandle->bindTable(table);
        if (!optionalInfo.succeed()) {
            return false;
        }
        const MigrationInfo* info = optionalInfo.value();
        if (info != nullptr) {
            schema = info->getSchemaForSourceDatabase();
            table = info->getSourceTable();
        }
    }
    return true;
}

#pragma mark - Override
bool MigratingHandleStatement::prepare(const Statement& statement)
{
    WCTAssert(!m_processing);
    WCTRemedialAssert(!isPrepared(), "Last statement is not finalized.", finalize(););
    auto optionalStatements = process(statement);
    if (!optionalStatements.succeed()) {
        return false;
    }
    auto& statements = optionalStatements.value();
    WCTAssert(!statements.empty());
    WCTAssert(m_additionalStatements.empty());
    auto iter = statements.begin();
    if (!Super::prepare(*iter)) {
        return false;
    }
    for (iter++; iter != statements.end(); iter++) {
        m_additionalStatements.emplace_back(getHandle());
        if (!m_additionalStatements.back().prepare(*iter)) {
            finalize();
            return false;
        }
    }
    return true;
}

void MigratingHandleStatement::finalize()
{
    Super::finalize();
    for (auto& handleStatement : m_additionalStatements) {
        handleStatement.finalize();
    }
    m_additionalStatements.clear();
    m_currentStatementType = StatementType::Invalid;
    clearMigrateStatus();
}

bool MigratingHandleStatement::step()
{
    if (m_additionalStatements.size() > 0) {
        MigratingHandle* migratingHandle = dynamic_cast<MigratingHandle*>(getHandle());
        WCTAssert(migratingHandle != nullptr);
        return migratingHandle->runTransaction(
        [this](InnerHandle*) -> bool { return realStep(); });
    }
    return realStep();
}

bool MigratingHandleStatement::realStep()
{
    if (!Super::step()) {
        return false;
    }
    if (m_additionalStatements.empty()) {
        return true;
    }
    if (m_currentStatementType == StatementType::InsertSTMT) {
        return stepInsert(getHandle()->getLastInsertedRowID());
    } else if (m_currentStatementType == StatementType::DeleteSTMT
               || m_currentStatementType == StatementType::UpdateSTMT) {
        return stepUpdateOrDelete();
    } else {
        for (auto& handleStatement : m_additionalStatements) {
            if (!handleStatement.step()) {
                return false;
            }
        }
    }
    return true;
}

void MigratingHandleStatement::reset()
{
    Super::reset();
    for (auto& handleStatement : m_additionalStatements) {
        handleStatement.reset();
    }
    if (m_primaryKeyIndex > 0) {
        m_assignedPrimaryKey = NullOpt;
    }
}

void MigratingHandleStatement::bindInteger(const Integer& value, int index)
{
    WCTRemedialAssert(index != MigrationInfo::indexOfRowIdOrPrimaryKey,
                      "Binding index is out of range",
                      return;);
    if (getBindParameterCount() >= index) {
        Super::bindInteger(value, index);
    }
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindInteger(value, index);
        }
    }
    if (m_currentStatementType == StatementType::InsertSTMT
        && m_primaryKeyIndex > 0 && index == m_primaryKeyIndex) {
        m_assignedPrimaryKey = value;
    }
}

void MigratingHandleStatement::bindDouble(const Float& value, int index)
{
    WCTRemedialAssert(index != MigrationInfo::indexOfRowIdOrPrimaryKey,
                      "Binding index is out of range",
                      return;);
    if (getBindParameterCount() >= index) {
        Super::bindDouble(value, index);
    }
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindDouble(value, index);
        }
    }
}

void MigratingHandleStatement::bindText(const Text& value, int index)
{
    WCTRemedialAssert(index != MigrationInfo::indexOfRowIdOrPrimaryKey,
                      "Binding index is out of range",
                      return;);
    if (getBindParameterCount() >= index) {
        Super::bindText(value, index);
    }
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindText(value, index);
        }
    }
}

void MigratingHandleStatement::bindText16(const char16_t* value, size_t valueLength, int index)
{
    WCTRemedialAssert(index != MigrationInfo::indexOfRowIdOrPrimaryKey,
                      "Binding index is out of range",
                      return;);
    if (getBindParameterCount() >= index) {
        Super::bindText16(value, valueLength, index);
    }
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindText16(value, valueLength, index);
        }
    }
}

void MigratingHandleStatement::bindBLOB(const BLOB& value, int index)
{
    WCTRemedialAssert(index != MigrationInfo::indexOfRowIdOrPrimaryKey,
                      "Binding index is out of range",
                      return;);
    if (getBindParameterCount() >= index) {
        Super::bindBLOB(value, index);
    }
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindBLOB(value, index);
        }
    }
}

void MigratingHandleStatement::bindNull(int index)
{
    WCTRemedialAssert(index != MigrationInfo::indexOfRowIdOrPrimaryKey,
                      "Binding index is out of range",
                      return;);
    if (getBindParameterCount() >= index) {
        Super::bindNull(index);
    }
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindNull(index);
        }
    }
    if (m_currentStatementType == StatementType::InsertSTMT
        && m_primaryKeyIndex > 0 && index == m_primaryKeyIndex) {
        m_assignedPrimaryKey = NullOpt;
    }
}

void MigratingHandleStatement::bindPointer(void* ptr,
                                           int index,
                                           const Text& type,
                                           void (*destructor)(void*))
{
    WCTRemedialAssert(index != MigrationInfo::indexOfRowIdOrPrimaryKey,
                      "Binding index is out of range",
                      return;);
    if (getBindParameterCount() >= index) {
        Super::bindPointer(ptr, index, type, destructor);
    }
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindPointer(ptr, index, type, destructor);
        }
    }
}

#pragma mark - Insert
bool MigratingHandleStatement::stepInsert(const int64_t& rowid)
{
    // The content inserting to source table is ignored.
    if (getHandle()->getChanges() == 0) {
        return true;
    }
    WCTAssert(m_additionalStatements.size() >= 2);

    // Remove from source table
    auto iter = m_additionalStatements.begin();
    iter->bindInteger(rowid, 1);
    if (!iter->step()) {
        return false;
    }
    iter++;

    // Insert to target table
    HandleStatement& migrateStatement = *iter;

    WCTAssert(m_migratingInfo != nullptr);
    if (m_migratingInfo->isAutoIncrement()) {
        return migrateStatement.step();
    }

    iter++;
    int64_t maxId = 1;

    if (!m_migratingInfo->getIntegerPrimaryKey().empty()) {
        if (m_assignedPrimaryKey.hasValue()) {
            maxId = m_assignedPrimaryKey.value();
        } else {
            WCTAssert(iter != m_additionalStatements.end());
            if (!iter->step()) {
                return false;
            }
            if (!iter->done()) {
                maxId = std::max(maxId, iter->getInteger());
            }
        }
    } else {
        WCTAssert(iter != m_additionalStatements.end());
        maxId = std::max(maxId, rowid);
        if (!iter->step()) {
            return false;
        }
        if (!iter->done()) {
            maxId = std::max(maxId, iter->getInteger());
        }
    }
    migrateStatement.bindInteger(maxId, MigrationInfo::indexOfRowIdOrPrimaryKey);
    if (!migrateStatement.step()) {
        if (getHandle()->getError().code() == Error::Code::Constraint) {
            Error error = Error(
            Error::Code::Warning,
            Error::Level::Warning,
            StringView::formatted("UNIQUE constraint failed with id %d", maxId));
            Notifier::shared().notify(error);
        }
        return false;
    }
    return true;
}

void MigratingHandleStatement::clearMigrateStatus()
{
    m_migratingInfo = nullptr;
    m_assignedPrimaryKey = NullOpt;
    m_primaryKeyIndex = 0;
}

#pragma mark - Update/Delete
bool MigratingHandleStatement::stepUpdateOrDelete()
{
    WCTAssert(m_additionalStatements.size() == 2);
    auto iter = m_additionalStatements.begin();
    HandleStatement& sourceStatement = *iter;
    iter++;
    HandleStatement& targetStatement = *iter;
    while (!Super::done()) {
        int64_t rowid = getInteger();
        sourceStatement.bindInteger(rowid, MigrationInfo::indexOfRowIdOrPrimaryKey);
        if (!sourceStatement.step()) {
            return false;
        }
        targetStatement.bindInteger(rowid, MigrationInfo::indexOfRowIdOrPrimaryKey);
        if (!targetStatement.step()) {
            return false;
        }
        if (!Super::step()) {
            return false;
        }
    }
    return true;
}

} //namespace WCDB
