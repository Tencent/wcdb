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
#include <WCDB/MigrationInfo.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Initialize
MigrationHandle::MigrationHandle(Migration& migration)
: Handle()
, Migration::Binder(migration)
, m_additionalStatement(getStatement())
, m_migrateStatement(getStatement())
, m_removeMigratedStatement(getStatement())
#ifdef DEBUG
, m_processing(false)
#endif
{
}

MigrationHandle::~MigrationHandle()
{
    returnStatement(m_additionalStatement);
    returnStatement(m_migrateStatement);
    returnStatement(m_removeMigratedStatement);
}

#pragma mark - Info Initializer
std::tuple<bool, bool, std::set<String>>
MigrationHandle::getColumnsForSourceTable(const MigrationUserInfo& userInfo)
{
    Schema schema = userInfo.getSchemaForSourceDatabase();
    bool succeed = true;
    bool integerPrimary = false;
    std::set<String> columns;
    do {
        if (!schema.syntax().isMain()) {
            std::set<String> attacheds;
            std::tie(succeed, attacheds)
            = getValues(MigrationInfo::getStatementForSelectingDatabaseList(), 1);
            if (succeed) {
                if (attacheds.find(schema.getDescription()) == attacheds.end()) {
                    succeed
                    = executeStatement(userInfo.getStatementForAttachingSchema());
                }
            }
            if (!succeed) {
                break;
            }
        }
        bool exists;
        std::tie(succeed, exists) = tableExists(schema, userInfo.getSourceTable());
        if (!succeed) {
            break;
        }
        if (exists) {
            std::vector<ColumnMeta> columnMetas;
            std::tie(succeed, columnMetas)
            = getTableMeta(schema, userInfo.getSourceTable());
            if (succeed) {
                integerPrimary = ColumnMeta::getIndexOfIntegerPrimary(columnMetas) >= 0;
                for (const auto& columnMeta : columnMetas) {
                    columns.emplace(columnMeta.name);
                }
            }
        }
    } while (false);
    return { succeed, integerPrimary, columns };
}

String MigrationHandle::getDatabasePath() const
{
    return getPath();
}

#pragma mark - Binder
bool MigrationHandle::rebindViews(const std::map<String, RecyclableMigrationInfo>& migratings)
{
    std::map<String, const MigrationInfo*> views2MigratingInfos;
    for (const auto& iter : migratings) {
        const MigrationInfo* migrating = iter.second.get();
        views2MigratingInfos.emplace(migrating->getUnionedView(), migrating);
    }

    std::set<String> existingViews;

    // get existing unioned views
    bool succeed;
    bool exists;
    std::tie(succeed, exists) = tableExists(Schema::temp(), Syntax::masterTable);
    if (!succeed) {
        return false;
    }
    if (exists) {
        std::tie(succeed, existingViews)
        = getValues(MigrationInfo::getStatementForSelectingUnionedView(), 0);
        if (!succeed) {
            return false;
        }
    }

    for (const auto& existingView : existingViews) {
        WCTInnerAssert(existingView.hasPrefix(MigrationInfo::getUnionedViewPrefix()));
        auto iter = views2MigratingInfos.find(existingView);
        if (iter != views2MigratingInfos.end()) {
            // it is already created
            views2MigratingInfos.erase(iter);
        } else {
            // it is no longer needed
            if (!executeStatement(MigrationInfo::getStatementForDroppingUnionedView(existingView))) {
                return false;
            }
        }
    }
    // create all needed views
    for (const auto& iter : views2MigratingInfos) {
        if (!executeStatement(iter.second->getStatementForCreatingUnionedView())) {
            return false;
        }
    }
    return true;
}

bool MigrationHandle::rebindSchemas(const std::map<String, RecyclableMigrationInfo>& migratings)
{
    std::map<String, const MigrationInfo*> schemas2MigratingInfos;
    for (const auto& iter : migratings) {
        const MigrationInfo* info = iter.second.get();
        if (info->isCrossDatabase()) {
            schemas2MigratingInfos.emplace(
            info->getSchemaForSourceDatabase().getDescription(), info);
        }
    }

    bool succeed;
    std::set<String> existingSchemas;
    std::tie(succeed, existingSchemas)
    = getValues(MigrationInfo::getStatementForSelectingDatabaseList(), 1);
    if (!succeed) {
        return false;
    }

    for (const auto& existingSchema : existingSchemas) {
        if (existingSchema.hasPrefix(MigrationInfo::getSchemaPrefix())) {
            auto iter = schemas2MigratingInfos.find(existingSchema);
            if (iter != schemas2MigratingInfos.end()) {
                // it is already attached
                schemas2MigratingInfos.erase(iter);
            } else {
                // it is not longer needed
                if (!executeStatement(
                    MigrationInfo::getStatementForDetachingSchema(existingSchema))) {
                    return false;
                }
            }
        }
    }
    // attach all needed schemas
    for (const auto& iter : schemas2MigratingInfos) {
        if (!executeStatement(iter.second->getStatementForAttachingSchema())) {
            return false;
        }
    }
    return true;
}

bool MigrationHandle::bindInfos(const std::map<String, RecyclableMigrationInfo>& migratings)
{
    return rebindViews(migratings) && rebindSchemas(migratings);
}

#pragma mark - Migration
std::pair<bool, std::list<Statement>> MigrationHandle::process(const Statement& originStatement)
{
#ifdef DEBUG
    m_processing = true;
#endif

    bool succeed = true;
    std::list<Statement> statements;
    do {
        startBinding();

        // It's dangerous to use origin statement after tampering since all the tokens are not fit.
        Statement falledBackStatement = originStatement;
        // fallback
        falledBackStatement.iterate(
        [&succeed, this](Syntax::Identifier& identifier, bool& stop) {
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
            case Syntax::Identifier::Type::Expression: {
                // main.table -> temp.unionedView
                Syntax::Expression& syntax = (Syntax::Expression&) identifier;
                switch (syntax.switcher) {
                case Syntax::Expression::Switch::Column:
                    succeed = tryFallbackToUnionedView(syntax.schema, syntax.table);
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
                if (!hintTable(syntax.table)) {
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
        succeed = Migration::Binder::stopBinding(succeed);
        if (!succeed) {
            break;
        }

        switch (originStatement.getType()) {
        case Syntax::Identifier::Type::InsertSTMT: {
            statements.push_back(falledBackStatement);
            const Syntax::InsertSTMT* migratedInsertSTMT
            = static_cast<const Syntax::InsertSTMT*>(originStatement.getSyntaxIdentifier());
            const String& falledBackTableName
            = static_cast<const Syntax::InsertSTMT*>(falledBackStatement.getSyntaxIdentifier())
              ->table;
            if (migratedInsertSTMT->table != falledBackTableName) {
                // it's safe to use origin statement since Conflict Action will not be changed during tampering.
                succeed = prepareMigrate(migratedInsertSTMT->table,
                                         migratedInsertSTMT->useConflictAction,
                                         migratedInsertSTMT->conflictAction);
            }
        } break;
        case Syntax::Identifier::Type::UpdateSTMT: {
            const String& migratedTableName
            = static_cast<const Syntax::UpdateSTMT*>(originStatement.getSyntaxIdentifier())
              ->table.table;
            const String& falledBackTableName
            = static_cast<const Syntax::UpdateSTMT*>(falledBackStatement.getSyntaxIdentifier())
              ->table.table;
            if (migratedTableName == falledBackTableName) {
                statements.push_back(falledBackStatement);
            } else {
                const MigrationInfo* info = getBoundInfo(migratedTableName);
                WCTInnerAssert(info != nullptr);
                // statement for source table
                statements.push_back(
                info->getStatementForLimitedUpdatingTable(falledBackStatement));
                // statement for migrated table
                statements.push_back(statements.back());
                Syntax::UpdateSTMT* stmt = static_cast<Syntax::UpdateSTMT*>(
                statements.back().getSyntaxIdentifier());
                stmt->table.table = migratedTableName;
                stmt->table.schema = Schema::main();
            }
        } break;
        case Syntax::Identifier::Type::DeleteSTMT: {
            const String& migratedTableName
            = static_cast<const Syntax::DeleteSTMT*>(originStatement.getSyntaxIdentifier())
              ->table.table;
            const String& falledBackTableName
            = static_cast<const Syntax::DeleteSTMT*>(falledBackStatement.getSyntaxIdentifier())
              ->table.table;
            if (migratedTableName == falledBackTableName) {
                statements.push_back(falledBackStatement);
            } else {
                const MigrationInfo* info = getBoundInfo(migratedTableName);
                WCTInnerAssert(info != nullptr);
                // statement for source table
                statements.push_back(
                info->getStatementForLimitedDeletingFromTable(falledBackStatement));
                // statement for migrated table
                statements.push_back(statements.back());
                Syntax::DeleteSTMT* stmt = static_cast<Syntax::DeleteSTMT*>(
                statements.back().getSyntaxIdentifier());
                stmt->table.table = migratedTableName;
                stmt->table.schema = Schema::main();
            }
        } break;
        case Syntax::Identifier::Type::DropTableSTMT: {
            const String& migratedTableName
            = static_cast<const Syntax::DropTableSTMT*>(originStatement.getSyntaxIdentifier())
              ->table;
            const String& falledBackTableName
            = static_cast<const Syntax::DropTableSTMT*>(falledBackStatement.getSyntaxIdentifier())
              ->table;
            statements.push_back(falledBackStatement);
            if (migratedTableName != falledBackTableName) {
                statements.push_back(statements.back());
                Syntax::DropTableSTMT* stmt = static_cast<Syntax::DropTableSTMT*>(
                statements.back().getSyntaxIdentifier());
                stmt->table = migratedTableName;
                stmt->schema = Schema::main();
            }
        } break;
        default:
            statements.push_back(falledBackStatement);
            break;
        }
    } while (false);
    if (!succeed) {
        statements.clear();
    }
#ifdef DEBUG
    m_processing = false;
#endif
    return { succeed, std::move(statements) };
}

bool MigrationHandle::tryFallbackToUnionedView(Syntax::Schema& schema, String& table)
{
    bool succeed = true;
    if (schema.isMain()) {
        const MigrationInfo* info;
        std::tie(succeed, info) = bindTable(table);
        if (succeed && info != nullptr) {
            schema = Schema::temp();
            table = info->getUnionedView();
        }
    }
    return succeed;
}

bool MigrationHandle::tryFallbackToSourceTable(Syntax::Schema& schema, String& table)
{
    bool succeed = true;
    if (schema.isMain()) {
        const MigrationInfo* info;
        std::tie(succeed, info) = bindTable(table);
        if (succeed && info != nullptr) {
            schema = info->getSchemaForSourceDatabase();
            table = info->getSourceTable();
        }
    }
    return succeed;
}

#pragma mark - Override
bool MigrationHandle::prepare(const Statement& statement)
{
    WCTInnerAssert(!m_processing);
    WCTRemedialAssert(!isPrepared(), "Last statement is not finalized.", finalize(););
    bool succeed;
    std::list<Statement> statements;
    std::tie(succeed, statements) = process(statement);
    if (!succeed) {
        return false;
    }
    WCTInnerAssert(statements.size() <= 2);
    if (m_mainStatement->prepare(statements.front())
        && (statements.size() == 1 || m_additionalStatement->prepare(statements.back()))) {
        return true;
    }
    finalize();
    return false;
}

bool MigrationHandle::isPrepared()
{
    return m_mainStatement->isPrepared();
}

void MigrationHandle::finalize()
{
    m_mainStatement->finalize();
    m_additionalStatement->finalize();
    finalizeMigrate();
}

bool MigrationHandle::step(bool& done)
{
    if (m_additionalStatement->isPrepared() || isMigratedPrepared()) {
        return runNestedTransaction(
        [this, &done](Handle*) -> bool { return realStep(done); });
    }
    return realStep(done);
}

bool MigrationHandle::realStep(bool& done)
{
    WCTInnerAssert(!(m_additionalStatement->isPrepared() && isMigratedPrepared()));
    return m_mainStatement->step(done)
           && (!m_additionalStatement->isPrepared() || m_additionalStatement->step())
           && (!isMigratedPrepared() || stepMigration(getLastInsertedRowID()));
}

void MigrationHandle::reset()
{
    m_mainStatement->reset();
    WCTInnerAssert(!(m_additionalStatement->isPrepared() && isMigratedPrepared()));
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->reset();
    }
    if (isMigratedPrepared()) {
        resetMigrate();
    }
}

void MigrationHandle::bindInteger32(const Integer32& value, int index)
{
    m_mainStatement->bindInteger32(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindInteger32(value, index);
    }
}

void MigrationHandle::bindInteger64(const Integer64& value, int index)
{
    m_mainStatement->bindInteger64(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindInteger64(value, index);
    }
}

void MigrationHandle::bindDouble(const Float& value, int index)
{
    m_mainStatement->bindDouble(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindDouble(value, index);
    }
}

void MigrationHandle::bindText(const Text& value, int index)
{
    m_mainStatement->bindText(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindText(value, index);
    }
}

void MigrationHandle::bindBLOB(const BLOB& value, int index)
{
    m_mainStatement->bindBLOB(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindBLOB(value, index);
    }
}

void MigrationHandle::bindNull(int index)
{
    m_mainStatement->bindNull(index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindNull(index);
    }
}

MigrationHandle::Integer32 MigrationHandle::getInteger32(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getInteger32(index);
}

MigrationHandle::Integer64 MigrationHandle::getInteger64(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getInteger64(index);
}

MigrationHandle::Float MigrationHandle::getDouble(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getDouble(index);
}

MigrationHandle::Text MigrationHandle::getText(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getText(index);
}

MigrationHandle::BLOB MigrationHandle::getBLOB(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getBLOB(index);
}

ColumnType MigrationHandle::getType(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getType(index);
}

const UnsafeString MigrationHandle::getOriginColumnName(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getOriginColumnName(index);
}

const UnsafeString MigrationHandle::getColumnName(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getColumnName(index);
}

const UnsafeString MigrationHandle::getColumnTableName(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getColumnTableName(index);
}

bool MigrationHandle::isStatementReadonly()
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->isReadonly();
}

int MigrationHandle::getNumberOfColumns()
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getNumberOfColumns();
}

#pragma mark - Migrate
bool MigrationHandle::isMigratedPrepared()
{
    WCTInnerAssert(m_migrateStatement->isPrepared()
                   == m_removeMigratedStatement->isPrepared());
    return m_migrateStatement->isPrepared() /* || m_removeMigratedStatement->isPrepared() */;
}

bool MigrationHandle::stepMigration(const int64_t& rowid)
{
    WCTInnerAssert(isMigratedPrepared());
    m_migrateStatement->bindInteger64(rowid, 1);
    m_removeMigratedStatement->bindInteger64(rowid, 1);
    return m_migrateStatement->step() && m_removeMigratedStatement->step();
}

void MigrationHandle::finalizeMigrate()
{
    m_migrateStatement->finalize();
    m_removeMigratedStatement->finalize();
}

void MigrationHandle::resetMigrate()
{
    WCTInnerAssert(isMigratedPrepared());
    m_migrateStatement->reset();
    m_removeMigratedStatement->reset();
}

bool MigrationHandle::prepareMigrate(const String& table,
                                     bool useConflictAction,
                                     Syntax::ConflictAction conflictAction)
{
    WCTInnerAssert(!isMigratedPrepared());
    const MigrationInfo* info = getBoundInfo(table);
    WCTInnerAssert(info != nullptr);
    return m_migrateStatement->prepare(info->getStatementForMigratingSpecifiedRow(
           useConflictAction, conflictAction))
           && m_removeMigratedStatement->prepare(info->getStatementForDeletingSpecifiedRow());
}

} //namespace WCDB
