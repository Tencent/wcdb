//
// Created by qiuwenchen on 2023/11/15.
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

#include "MigratingStatementDecorator.hpp"
#include "Assertion.hpp"
#include "Core.hpp"
#include "SQLite.h"
#include "StringView.hpp"
#include "WINQ.h"

namespace WCDB {

#pragma mark - Basic
MigratingStatementDecorator::MigratingStatementDecorator(Migration::Binder* binder)
: HandleStatementDecorator()
, m_migrationBinder(binder)
, m_processing(false)
, m_currentStatementType(StatementType::Invalid)
, m_migratingInfo(nullptr)
, m_primaryKeyIndex(0)
, m_rowidBindIndex(0)
{
}

MigratingStatementDecorator::MigratingStatementDecorator(MigratingStatementDecorator&& other)
: Super(std::move(other))
, m_migrationBinder(other.m_migrationBinder)
, m_processing(other.m_processing)
, m_currentStatementType(other.m_currentStatementType)
, m_additionalStatements(std::move(other.m_additionalStatements))
, m_migratingInfo(other.m_migratingInfo)
, m_assignedPrimaryKey(std::move(other.m_assignedPrimaryKey))
, m_primaryKeyIndex(other.m_primaryKeyIndex)
, m_rowidBindIndex(other.m_rowidBindIndex)
{
    other.m_migrationBinder = nullptr;
    other.m_processing = false;
    other.m_currentStatementType = StatementType::Invalid;
    other.m_additionalStatements.clear();
    other.m_migratingInfo = nullptr;
    other.m_assignedPrimaryKey = NullOpt;
    other.m_primaryKeyIndex = 0;
    other.m_rowidBindIndex = 0;
}

MigratingStatementDecorator::~MigratingStatementDecorator() = default;

void MigratingStatementDecorator::decorate(Decorative* handleStatement)
{
    Super::decorate(handleStatement);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, prepare);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, finalize);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, step);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, reset);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, clearBindings);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, bindInteger);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, bindDouble);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, bindText);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, bindText16);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, bindBLOB);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, bindNull);
    WCDBSwizzleDecorativeFunction(handleStatement, MigratingStatementDecorator, bindPointer);
}

#pragma mark - Migration
Optional<std::list<Statement>>
MigratingStatementDecorator::process(const Statement& originStatement)
{
    m_processing = true;
    bool succeed = true;
    std::list<Statement> statements;
    do {
        m_migrationBinder->startBinding();

        // It's dangerous to use origin statement after tampering since all the tokens are not fit.
        Statement falledBackStatement = originStatement;
        // fallback
        falledBackStatement.iterate([&succeed, this, &originStatement](
                                    Syntax::Identifier& identifier, bool isBegin, bool& stop) {
            if (!isBegin) {
                return;
            }
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
                if (!m_migrationBinder->hintThatTableWillBeCreated(syntax.table)) {
                    succeed = false;
                }
            } break;
            case Syntax::Identifier::Type::BindParameter: {
                Syntax::BindParameter& bindParameter = (Syntax::BindParameter&) identifier;
                if (bindParameter.switcher != Syntax::BindParameter::Switch::QuestionSign) {
                    getHandle()->notifyError(
                    Error::Code::Misuse,
                    originStatement.getDescription(),
                    "Only numeric bind parameters are allowed to used in the migrating database");
                    succeed = false;
                } else if (bindParameter.n <= 0) {
                    getHandle()->notifyError(
                    (int) Error::Code::Misuse,
                    originStatement.getDescription().data(),
                    "The indexes of bind parameters must be assigned in the migrating database");
                    succeed = false;
                } else {
                    m_rowidBindIndex = std::max(m_rowidBindIndex, bindParameter.n);
                }
            } break;
            default:
                break;
            }
            if (!succeed) {
                stop = true;
            }
        });
        succeed = m_migrationBinder->stopBinding(succeed);
        if (!succeed) {
            break;
        }
        m_rowidBindIndex++;

        switch (originStatement.getType()) {
        case Syntax::Identifier::Type::InsertSTMT: {
            const Syntax::InsertSTMT& migratedInsertSTMT
            = static_cast<const Syntax::InsertSTMT&>(originStatement.syntax());
            const Syntax::InsertSTMT& falledBackSTMT
            = static_cast<const Syntax::InsertSTMT&>(falledBackStatement.syntax());
            if (migratedInsertSTMT.isTargetingSameTable(falledBackSTMT)) {
                statements.push_back(falledBackStatement);
            } else {
                WCTRemedialAssert(!migratedInsertSTMT.columns.empty(),
                                  "Insert statement that does not explicitly indicate columns is not supported while using migration feature.",
                                  succeed = false;
                                  break;);
                WCTRemedialAssert(!migratedInsertSTMT.isMultiWrite(),
                                  "Insert statement that contains multiple values is not supported while using migration feature.",
                                  succeed = false;
                                  break;);
                clearMigrateStatus();
                const MigrationInfo* info
                = m_migrationBinder->getBoundInfo(migratedInsertSTMT.table);
                WCTAssert(info != nullptr);
                m_migratingInfo = info;
                info->generateStatementsForInsertMigrating(
                falledBackStatement, statements, m_primaryKeyIndex, m_rowidBindIndex, m_assignedPrimaryKey);
                sqlite3_revertCommitOrder(getHandleStatement()->getRawHandle());
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
                const MigrationInfo* info
                = m_migrationBinder->getBoundInfo(migratedTableName);
                WCTAssert(info != nullptr);
                info->generateStatementsForUpdateMigrating(
                falledBackStatement, statements, m_rowidBindIndex);
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
                const MigrationInfo* info
                = m_migrationBinder->getBoundInfo(migratedTableName);
                WCTAssert(info != nullptr);
                info->generateStatementsForDeleteMigrating(
                falledBackStatement, statements, m_rowidBindIndex);
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
                = m_migrationBinder->getBoundInfo(migratedSTMT.table);
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

bool MigratingStatementDecorator::tryFallbackToUnionedView(Syntax::Schema& schema,
                                                           StringView& table)
{
    if (schema.isMain()) {
        auto optionalInfo = m_migrationBinder->bindTable(table);
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

bool MigratingStatementDecorator::tryFallbackToSourceTable(Syntax::Schema& schema,
                                                           StringView& table)
{
    if (schema.isMain()) {
        auto optionalInfo = m_migrationBinder->bindTable(table);
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
bool MigratingStatementDecorator::prepare(const Statement& statement)
{
    WCTRemedialAssert(!getHandleStatement()->isPrepared(),
                      "Last statement is not finalized.",
                      finalize(););
    WCTAssert(!m_processing);
    auto optionalStatements = process(statement);
    if (!optionalStatements.succeed()) {
        return false;
    }
    auto& statements = optionalStatements.value();
    WCTAssert(!statements.empty());
    WCTAssert(m_additionalStatements.empty());
    // The last statement must be targeted at the main scheme
    if (!Super::prepare(statements.back())) {
        return false;
    }
    statements.pop_back();
    for (const auto& stmt : statements) {
        m_additionalStatements.emplace_back(getHandle());
        m_additionalStatements.back().enableAutoAddColumn();
        if (!m_additionalStatements.back().prepare(stmt)) {
            getHandleStatement()->finalize();
            return false;
        }
    }
    return true;
}

void MigratingStatementDecorator::finalize()
{
    Super::finalize();
    for (auto& handleStatement : m_additionalStatements) {
        handleStatement.finalize();
    }
    m_additionalStatements.clear();
    m_currentStatementType = StatementType::Invalid;
    m_rowidBindIndex = 0;
    clearMigrateStatus();
}

bool MigratingStatementDecorator::step()
{
    if (m_additionalStatements.size() > 0) {
        WCTAssert(dynamic_cast<InnerHandle*>(getHandle()) != nullptr);
        InnerHandle* handle = static_cast<InnerHandle*>(getHandle());
        return handle->runTransaction(
        [this](InnerHandle*) -> bool { return realStep(); });
    }
    return realStep();
}

bool MigratingStatementDecorator::realStep()
{
    if (m_additionalStatements.empty()) {
        return Super::step();
    }
    if (m_currentStatementType == StatementType::InsertSTMT) {
        return stepInsert();
    } else if (m_currentStatementType == StatementType::DeleteSTMT
               || m_currentStatementType == StatementType::UpdateSTMT) {
        return stepUpdateOrDelete();
    } else {
        for (auto& handleStatement : m_additionalStatements) {
            if (!handleStatement.step()) {
                return false;
            }
        }
        return Super::step();
    }
}

void MigratingStatementDecorator::reset()
{
    Super::reset();
    for (auto& handleStatement : m_additionalStatements) {
        handleStatement.reset();
    }
    if (m_primaryKeyIndex > 0) {
        m_assignedPrimaryKey = NullOpt;
    }
}

void MigratingStatementDecorator::clearBindings()
{
    Super::reset();
    for (auto& handleStatement : m_additionalStatements) {
        handleStatement.clearBindings();
    }
    if (m_primaryKeyIndex > 0) {
        m_assignedPrimaryKey = NullOpt;
    }
}

void MigratingStatementDecorator::bindInteger(const Integer& value, int index)
{
    WCTRemedialAssert(index != m_rowidBindIndex, "Binding index is out of range", return;);
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindInteger(value, index);
        }
    }
    if (getHandleStatement()->getBindParameterCount() >= index) {
        Super::bindInteger(value, index);
    }
    if (m_currentStatementType == StatementType::InsertSTMT
        && m_primaryKeyIndex > 0 && index == m_primaryKeyIndex) {
        m_assignedPrimaryKey = value;
    }
}

void MigratingStatementDecorator::bindDouble(const Float& value, int index)
{
    WCTRemedialAssert(index != m_rowidBindIndex, "Binding index is out of range", return;);
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindDouble(value, index);
        }
    }
    if (getHandleStatement()->getBindParameterCount() >= index) {
        Super::bindDouble(value, index);
    }
}

void MigratingStatementDecorator::bindText(const Text& value, int index)
{
    WCTRemedialAssert(index != m_rowidBindIndex, "Binding index is out of range", return;);
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindText(value, index);
        }
    }
    if (getHandleStatement()->getBindParameterCount() >= index) {
        Super::bindText(value, index);
    }
}

void MigratingStatementDecorator::bindText16(const char16_t* value, size_t valueLength, int index)
{
    WCTRemedialAssert(index != m_rowidBindIndex, "Binding index is out of range", return;);
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindText16(value, valueLength, index);
        }
    }
    if (getHandleStatement()->getBindParameterCount() >= index) {
        Super::bindText16(value, valueLength, index);
    }
}

void MigratingStatementDecorator::bindBLOB(const BLOB& value, int index)
{
    WCTRemedialAssert(index != m_rowidBindIndex, "Binding index is out of range", return;);
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindBLOB(value, index);
        }
    }
    if (getHandleStatement()->getBindParameterCount() >= index) {
        Super::bindBLOB(value, index);
    }
}

void MigratingStatementDecorator::bindNull(int index)
{
    WCTRemedialAssert(index != m_rowidBindIndex, "Binding index is out of range", return;);
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindNull(index);
        }
    }
    if (getHandleStatement()->getBindParameterCount() >= index) {
        Super::bindNull(index);
    }
    if (m_currentStatementType == StatementType::InsertSTMT
        && m_primaryKeyIndex > 0 && index == m_primaryKeyIndex) {
        m_assignedPrimaryKey = NullOpt;
    }
}

void MigratingStatementDecorator::bindPointer(void* ptr,
                                              int index,
                                              const Text& type,
                                              void (*destructor)(void*))
{
    WCTRemedialAssert(index != m_rowidBindIndex, "Binding index is out of range", return;);
    for (auto& handleStatement : m_additionalStatements) {
        if (handleStatement.getBindParameterCount() >= index) {
            handleStatement.bindPointer(ptr, index, type, destructor);
        }
    }
    if (getHandleStatement()->getBindParameterCount() >= index) {
        Super::bindPointer(ptr, index, type, destructor);
    }
}

#pragma mark - Insert
bool MigratingStatementDecorator::stepInsert()
{
    // Insert to source table
    auto iter = m_additionalStatements.begin();
    if (!iter->step()) {
        return false;
    }
    // The content inserting to source table is ignored.
    if (getHandle()->getChanges() == 0) {
        return true;
    }
    iter++;
    int64_t rowid = getHandle()->getLastInsertedRowID();
    WCTAssert(m_additionalStatements.size() >= 2);

    // Remove from source table
    iter->bindInteger(rowid, 1);
    if (!iter->step()) {
        return false;
    }
    iter++;

    // Insert to target table
    WCTAssert(m_migratingInfo != nullptr);
    if (m_migratingInfo->isAutoIncrement()) {
        return Super::step();
    }

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
    Super::bindInteger(maxId, m_rowidBindIndex);
    if (!Super::step()) {
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

void MigratingStatementDecorator::clearMigrateStatus()
{
    m_migratingInfo = nullptr;
    m_assignedPrimaryKey = NullOpt;
    m_primaryKeyIndex = 0;
}

#pragma mark - Update/Delete
bool MigratingStatementDecorator::stepUpdateOrDelete()
{
    WCTAssert(m_additionalStatements.size() == 2);
    auto iter = m_additionalStatements.begin();
    HandleStatement& selectStatement = *iter;
    if (!selectStatement.step()) {
        return false;
    }
    iter++;
    HandleStatement& sourceStatement = *iter;
    while (!selectStatement.done()) {
        int64_t rowid = selectStatement.getInteger();
        sourceStatement.reset();
        sourceStatement.bindInteger(rowid, m_rowidBindIndex);
        if (!sourceStatement.step()) {
            return false;
        }
        Super::reset();
        Super::bindInteger(rowid, m_rowidBindIndex);
        if (!Super::step()) {
            return false;
        }
        if (!selectStatement.step()) {
            return false;
        }
    }
    return true;
}

} //namespace WCDB
