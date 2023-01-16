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

#include <WCDB/Assertion.hpp>
#include <WCDB/Core.hpp>
#include <WCDB/MigratingHandle.hpp>
#include <WCDB/MigratingHandleStatement.hpp>
#include <WCDB/SQLite.h>
#include <WCDB/StringView.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {
MigratingHandleStatement::MigratingHandleStatement(MigratingHandleStatement&& other)
: HandleStatement(std::move(other))
, m_processing(other.m_processing)
, m_additionalStatement(other.m_additionalStatement)
, m_migrateStatement(other.m_migrateStatement)
, m_removeMigratedStatement(other.m_removeMigratedStatement)
, m_rowidIndexOfMigratingStatement(other.m_rowidIndexOfMigratingStatement)
{
    other.m_processing = false;
    other.m_additionalStatement = nullptr;
    other.m_migrateStatement = nullptr;
    other.m_removeMigratedStatement = nullptr;
    other.m_rowidIndexOfMigratingStatement = 0;
}

MigratingHandleStatement::MigratingHandleStatement(MigratingHandle* handle)
: HandleStatement(handle)
, m_processing(false)
, m_additionalStatement(std::make_shared<HandleStatement>(handle))
, m_migrateStatement(std::make_shared<HandleStatement>(handle))
, m_removeMigratedStatement(std::make_shared<HandleStatement>(handle))
, m_rowidIndexOfMigratingStatement(0)
{
    m_additionalStatement->enableAutoAddColumn();
    m_migrateStatement->enableAutoAddColumn();
    m_removeMigratedStatement->enableAutoAddColumn();
}

MigratingHandleStatement::~MigratingHandleStatement()
{
    finalize();
}

#pragma mark - Migration
std::optional<std::list<Statement>>
MigratingHandleStatement::process(const Statement& originStatement)
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
        // fallback
        falledBackStatement.iterate([&succeed, this, migratingHandle](
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
                    succeed = tryFallbackToUnionedView(syntax.column.schema,
                                                       syntax.column.table);
                    break;
                case Syntax::Expression::Switch::In:
                    if (syntax.inSwitcher == Syntax::Expression::SwitchIn::Table) {
                        succeed = tryFallbackToUnionedView(syntax.schema, syntax.table);
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
            statements.push_back(falledBackStatement);
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
            if (!migratedInsertSTMT.isTargetingSameTable(falledBackSTMT)) {
                // it's safe to use origin statement since Conflict Action will not be changed during tampering.
                succeed = prepareMigrate(migratedInsertSTMT, falledBackSTMT);
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
                // statement for source table
                statements.push_back(
                info->getStatementForLimitedUpdatingTable(falledBackStatement));
                // statement for migrated table
                statements.push_back(statements.back());
                Syntax::UpdateSTMT& stmt
                = static_cast<Syntax::UpdateSTMT&>(statements.back().syntax());
                stmt.table.table = migratedTableName;
                stmt.table.schema = Schema::main();
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
                // statement for source table
                statements.push_back(
                info->getStatementForLimitedDeletingFromTable(falledBackStatement));
                // statement for migrated table
                statements.push_back(statements.back());
                Syntax::DeleteSTMT& stmt
                = static_cast<Syntax::DeleteSTMT&>(statements.back().syntax());
                stmt.table.table = migratedTableName;
                stmt.table.schema = Schema::main();
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
            }
        } break;
        default:
            statements.push_back(falledBackStatement);
            break;
        }
    } while (false);
    m_processing = false;
    if (succeed) {
        return std::move(statements);
    } else {
        return std::nullopt;
    }
}

bool MigratingHandleStatement::tryFallbackToUnionedView(Syntax::Schema& schema, StringView& table)
{
    if (schema.isMain()) {
        MigratingHandle* migratingHandle = dynamic_cast<MigratingHandle*>(getHandle());
        WCTAssert(migratingHandle != nullptr);
        auto optionalInfo = migratingHandle->bindTable(table);
        if (!optionalInfo.has_value()) {
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
        if (!optionalInfo.has_value()) {
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
    if (!optionalStatements.has_value()) {
        return false;
    }
    auto& statements = optionalStatements.value();
    WCTAssert(statements.size() <= 2);
    if (Super::prepare(statements.front())
        && (statements.size() == 1 || m_additionalStatement->prepare(statements.back()))) {
        return true;
    }
    finalize();
    return false;
}

void MigratingHandleStatement::finalize()
{
    Super::finalize();
    if (m_additionalStatement != nullptr) {
        m_additionalStatement->finalize();
    }
    finalizeMigrate();
}

bool MigratingHandleStatement::step()
{
    if (m_additionalStatement->isPrepared() || isMigratedPrepared()) {
        MigratingHandle* migratingHandle = dynamic_cast<MigratingHandle*>(getHandle());
        WCTAssert(migratingHandle != nullptr);
        return migratingHandle->runNestedTransaction(
        [this](InnerHandle*) -> bool { return realStep(); });
    }
    return realStep();
}

bool MigratingHandleStatement::realStep()
{
    WCTAssert(!(m_additionalStatement->isPrepared() && isMigratedPrepared()));
    return Super::step()
           && (!m_additionalStatement->isPrepared() || m_additionalStatement->step())
           && (!isMigratedPrepared() || stepMigration(getHandle()->getLastInsertedRowID()));
}

void MigratingHandleStatement::reset()
{
    Super::reset();
    WCTAssert(!(m_additionalStatement->isPrepared() && isMigratedPrepared()));
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->reset();
    }
    if (isMigratedPrepared()) {
        resetMigrate();
    }
}

void MigratingHandleStatement::bindInteger(const Integer& value, int index)
{
    Super::bindInteger(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindInteger(value, index);
    }
    if (m_migrateStatement->isPrepared()) {
        WCTRemedialAssert(m_rowidIndexOfMigratingStatement == 0 || index != m_rowidIndexOfMigratingStatement,
                          "Binding index is out of range",
                          return;);
        m_migrateStatement->bindInteger(value, index);
    }
}

void MigratingHandleStatement::bindDouble(const Float& value, int index)
{
    Super::bindDouble(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindDouble(value, index);
    }
    if (m_migrateStatement->isPrepared()) {
        WCTRemedialAssert(m_rowidIndexOfMigratingStatement == 0 || index != m_rowidIndexOfMigratingStatement,
                          "Binding index is out of range",
                          return;);
        m_migrateStatement->bindDouble(value, index);
    }
}

void MigratingHandleStatement::bindText(const Text& value, int index)
{
    Super::bindText(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindText(value, index);
    }
    if (m_migrateStatement->isPrepared()) {
        WCTRemedialAssert(m_rowidIndexOfMigratingStatement == 0 || index != m_rowidIndexOfMigratingStatement,
                          "Binding index is out of range",
                          return;);
        m_migrateStatement->bindText(value, index);
    }
}

void MigratingHandleStatement::bindBLOB(const BLOB& value, int index)
{
    Super::bindBLOB(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindBLOB(value, index);
    }
    if (m_migrateStatement->isPrepared()) {
        WCTRemedialAssert(m_rowidIndexOfMigratingStatement == 0 || index != m_rowidIndexOfMigratingStatement,
                          "Binding index is out of range",
                          return;);
        m_migrateStatement->bindBLOB(value, index);
    }
}

void MigratingHandleStatement::bindNull(int index)
{
    Super::bindNull(index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindNull(index);
    }
    if (m_migrateStatement->isPrepared()) {
        WCTRemedialAssert(m_rowidIndexOfMigratingStatement == 0 || index != m_rowidIndexOfMigratingStatement,
                          "Binding index is out of range",
                          return;);
        m_migrateStatement->bindNull(index);
    }
}

void MigratingHandleStatement::bindPointer(void* ptr,
                                           int index,
                                           const Text& type,
                                           void (*destructor)(void*))
{
    Super::bindPointer(ptr, index, type, destructor);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindPointer(ptr, index, type, destructor);
    }
    if (m_migrateStatement->isPrepared()) {
        WCTRemedialAssert(m_rowidIndexOfMigratingStatement == 0 || index != m_rowidIndexOfMigratingStatement,
                          "Binding index is out of range",
                          return;);
        m_migrateStatement->bindPointer(ptr, index, type, destructor);
    }
}

#pragma mark - Migrate
bool MigratingHandleStatement::isMigratedPrepared()
{
    WCTAssert(m_migrateStatement->isPrepared() == m_removeMigratedStatement->isPrepared());
    return m_migrateStatement->isPrepared() /* || m_removeMigratedStatement->isPrepared() */;
}

bool MigratingHandleStatement::stepMigration(const int64_t& rowid)
{
    WCTAssert(isMigratedPrepared());
    m_removeMigratedStatement->bindInteger(rowid, 1);
    if (m_rowidIndexOfMigratingStatement > 0) {
        m_migrateStatement->bindInteger(rowid, m_rowidIndexOfMigratingStatement);
    }
    if (!m_removeMigratedStatement->step()) {
        return false;
    }
    if (!m_migrateStatement->step()) {
        if (getHandle()->getError().code() == Error::Code::Constraint) {
            Error error
            = Error(Error::Code::Warning,
                    Error::Level::Warning,
                    StringView::formatted("UNIQUE constraint failed with rowId %d, rowidIndex %d",
                                          rowid,
                                          m_rowidIndexOfMigratingStatement));
            Notifier::shared().notify(error);
        }
        return false;
    }
    return true;
}

void MigratingHandleStatement::finalizeMigrate()
{
    if (m_removeMigratedStatement != nullptr) {
        m_removeMigratedStatement->finalize();
    }
    if (m_migrateStatement != nullptr) {
        m_migrateStatement->finalize();
    }
}

void MigratingHandleStatement::resetMigrate()
{
    WCTAssert(isMigratedPrepared());
    m_removeMigratedStatement->reset();
    m_migrateStatement->reset();
}

bool MigratingHandleStatement::prepareMigrate(const Syntax::InsertSTMT& migrated,
                                              const Syntax::InsertSTMT& falledBack)
{
    WCTAssert(!isMigratedPrepared());
    MigratingHandle* migratingHandle = dynamic_cast<MigratingHandle*>(getHandle());
    WCTAssert(migratingHandle != nullptr);
    const MigrationInfo* info = migratingHandle->getBoundInfo(migrated.table);
    WCTAssert(info != nullptr);
    m_rowidIndexOfMigratingStatement = info->getRowIDIndexOfMigratingStatement();
    return m_removeMigratedStatement->prepare(info->getStatementForDeletingSpecifiedRow())
           && m_migrateStatement->prepare(info->getStatementForMigrating(falledBack));
}

} //namespace WCDB
