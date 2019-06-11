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
#include <WCDB/MigratingHandle.hpp>
#include <WCDB/MigrationInfo.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Initialize
MigratingHandle::MigratingHandle(Migration& migration)
: Handle()
, Migration::Binder(migration)
, m_additionalStatement(getStatement())
, m_migrateStatement(getStatement())
, m_removeMigratedStatement(getStatement())
#ifdef WCDB_DEBUG
, m_processing(false)
#endif
{
}

MigratingHandle::~MigratingHandle()
{
    returnStatement(m_additionalStatement);
    returnStatement(m_migrateStatement);
    returnStatement(m_removeMigratedStatement);
}

#pragma mark - Info Initializer
std::tuple<bool, bool, std::set<String>>
MigratingHandle::getColumnsOfUserInfo(const MigrationUserInfo& userInfo)
{
    bool succeed = true;
    bool integerPrimary = false;
    std::set<String> columns;
    do {
        bool exists;
        std::tie(succeed, exists) = tableExists(Schema::main(), userInfo.getTable());
        if (!succeed) {
            break;
        }
        if (exists) {
            std::vector<ColumnMeta> columnMetas;
            std::tie(succeed, columnMetas)
            = getTableMeta(Schema::main(), userInfo.getTable());
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

std::pair<bool, bool> MigratingHandle::sourceTableExists(const MigrationUserInfo& userInfo)
{
    bool succeed = true;
    bool exists = false;
    do {
        Schema schema = userInfo.getSchemaForSourceDatabase();
        if (!schema.syntax().isMain()) {
            std::set<String> attacheds;
            std::tie(succeed, attacheds)
            = getValues(MigrationInfo::getStatementForSelectingDatabaseList(), 1);
            if (succeed) {
                if (attacheds.find(schema.getDescription()) == attacheds.end()) {
                    succeed
                    = executeStatement(userInfo.getStatementForAttachingSchema());
                    if (succeed) {
                        resetSynchronousToDefaultForNewlyAttached(
                        userInfo.getSchemaForSourceDatabase());
                        succeed = runSchemaTransactionForNewlyAttached();
                    }
                }
            }
            if (!succeed) {
                break;
            }
        }
        std::tie(succeed, exists) = tableExists(schema, userInfo.getSourceTable());
    } while (false);
    return { succeed, exists };
}

String MigratingHandle::getDatabasePath() const
{
    return getPath();
}

#pragma mark - Binder
bool MigratingHandle::rebindViews(const std::map<String, const MigrationInfo*>& migratings)
{
    std::map<String, const MigrationInfo*> views2MigratingInfos;
    for (const auto& iter : migratings) {
        const MigrationInfo* info = iter.second;
        WCTInnerAssert(info != nullptr);
        views2MigratingInfos.emplace(info->getUnionedView(), info);
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

bool MigratingHandle::rebindSchemas(const std::map<String, const MigrationInfo*>& migratings)
{
    std::map<String, const MigrationInfo*> schemas2MigratingInfos;
    for (const auto& iter : migratings) {
        const MigrationInfo* info = iter.second;
        WCTInnerAssert(info != nullptr);
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
                if (!isInTransaction()) {
                    // it is not longer needed
                    if (!executeStatement(
                        MigrationInfo::getStatementForDetachingSchema(existingSchema))) {
                        return false;
                    }
                }
            }
        }
    }
    bool attached = false;
    // attach all needed schemas
    for (const auto& iter : schemas2MigratingInfos) {
        if (!executeStatement(iter.second->getStatementForAttachingSchema())) {
            return false;
        }
        attached = true;

        resetSynchronousToDefaultForNewlyAttached(iter.second->getSchemaForSourceDatabase());
    }
    if (attached) {
        return runSchemaTransactionForNewlyAttached();
    }
    return true;
}

bool MigratingHandle::bindInfos(const std::map<String, const MigrationInfo*>& migratings)
{
    return rebindViews(migratings) && rebindSchemas(migratings);
}

bool MigratingHandle::runSchemaTransactionForNewlyAttached()
{
    bool succeed = true;
    if (isInTransaction()) {
        markErrorAsIgnorable(Error::Code::Error);
        succeed = beginTransaction(true);
        WCTInnerAssert(!succeed);
        if (!succeed && isErrorIgnorable()) {
            succeed = true;
        }
        markErrorAsUnignorable();
    }
    return succeed;
}

void MigratingHandle::resetSynchronousToDefaultForNewlyAttached(const Schema& newSchema)
{
    WCTInnerAssert(!newSchema.syntax().isMain());
    // invoke setting syncronous since SQLite will set synchrounous full by default for attached pager.
    executeStatement(
    StatementPragma().pragma(Pragma::synchronous()).schema(newSchema).to(SQLITE_DEFAULT_SYNCHRONOUS));
}

#pragma mark - Migration
std::pair<bool, std::list<Statement>> MigratingHandle::process(const Statement& originStatement)
{
#ifdef WCDB_DEBUG
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
                if (!hintThatTableWillBeCreated(syntax.table)) {
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
            const Syntax::InsertSTMT* falledBackSTMT
            = static_cast<const Syntax::InsertSTMT*>(falledBackStatement.getSyntaxIdentifier());
            if (!migratedInsertSTMT->isTargetingSameTable(*falledBackSTMT)) {
                // it's safe to use origin statement since Conflict Action will not be changed during tampering.
                succeed = prepareMigrate(migratedInsertSTMT->table,
                                         migratedInsertSTMT->conflictAction);
            }
        } break;
        case Syntax::Identifier::Type::UpdateSTMT: {
            const Syntax::QualifiedTableName& migratedTable
            = static_cast<const Syntax::UpdateSTMT*>(originStatement.getSyntaxIdentifier())
              ->table;
            const Syntax::QualifiedTableName& falledBackTable
            = static_cast<const Syntax::UpdateSTMT*>(falledBackStatement.getSyntaxIdentifier())
              ->table;
            if (migratedTable.isTargetingToSameTable(falledBackTable)) {
                statements.push_back(falledBackStatement);
            } else {
                const String& migratedTableName = migratedTable.table;
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
            const Syntax::QualifiedTableName& migratedTable
            = static_cast<const Syntax::DeleteSTMT*>(originStatement.getSyntaxIdentifier())
              ->table;
            const Syntax::QualifiedTableName& falledBackTable
            = static_cast<const Syntax::DeleteSTMT*>(falledBackStatement.getSyntaxIdentifier())
              ->table;
            if (migratedTable.isTargetingToSameTable(falledBackTable)) {
                statements.push_back(falledBackStatement);
            } else {
                const String& migratedTableName = migratedTable.table;
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
            const Syntax::DropTableSTMT* migratedSTMT
            = static_cast<const Syntax::DropTableSTMT*>(originStatement.getSyntaxIdentifier());
            const Syntax::DropTableSTMT* falledBackSTMT
            = static_cast<const Syntax::DropTableSTMT*>(
            falledBackStatement.getSyntaxIdentifier());
            statements.push_back(originStatement);
            if (!migratedSTMT->isTargetingSameTable(*falledBackSTMT)) {
                // Don't drop source table. Instead, delete all contents from source table and wait the stepper do the dropping work.
                const MigrationInfo* info = getBoundInfo(migratedSTMT->table);
                WCTInnerAssert(info != nullptr);
                statements.push_back(info->getStatementForDeletingFromTable(falledBackStatement));
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
#ifdef WCDB_DEBUG
    m_processing = false;
#endif
    return { succeed, std::move(statements) };
}

bool MigratingHandle::tryFallbackToUnionedView(Syntax::Schema& schema, String& table)
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

bool MigratingHandle::tryFallbackToSourceTable(Syntax::Schema& schema, String& table)
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
bool MigratingHandle::prepare(const Statement& statement)
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

bool MigratingHandle::isPrepared()
{
    return m_mainStatement->isPrepared();
}

void MigratingHandle::finalize()
{
    m_mainStatement->finalize();
    m_additionalStatement->finalize();
    finalizeMigrate();
    stopReferenced();
}

bool MigratingHandle::step()
{
    if (m_additionalStatement->isPrepared() || isMigratedPrepared()) {
        return runNestedTransaction(
        [this](Handle*) -> bool { return realStep(); });
    }
    return realStep();
}

bool MigratingHandle::realStep()
{
    WCTInnerAssert(!(m_additionalStatement->isPrepared() && isMigratedPrepared()));
    return m_mainStatement->step()
           && (!m_additionalStatement->isPrepared() || m_additionalStatement->step())
           && (!isMigratedPrepared() || stepMigration(getLastInsertedRowID()));
}

bool MigratingHandle::done()
{
    return m_mainStatement->done();
}

void MigratingHandle::reset()
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

void MigratingHandle::bindInteger32(const Integer32& value, int index)
{
    m_mainStatement->bindInteger32(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindInteger32(value, index);
    }
}

void MigratingHandle::bindInteger64(const Integer64& value, int index)
{
    m_mainStatement->bindInteger64(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindInteger64(value, index);
    }
}

void MigratingHandle::bindDouble(const Float& value, int index)
{
    m_mainStatement->bindDouble(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindDouble(value, index);
    }
}

void MigratingHandle::bindText(const Text& value, int index)
{
    m_mainStatement->bindText(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindText(value, index);
    }
}

void MigratingHandle::bindBLOB(const BLOB& value, int index)
{
    m_mainStatement->bindBLOB(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindBLOB(value, index);
    }
}

void MigratingHandle::bindNull(int index)
{
    m_mainStatement->bindNull(index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindNull(index);
    }
}

MigratingHandle::Integer32 MigratingHandle::getInteger32(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getInteger32(index);
}

MigratingHandle::Integer64 MigratingHandle::getInteger64(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getInteger64(index);
}

MigratingHandle::Float MigratingHandle::getDouble(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getDouble(index);
}

MigratingHandle::Text MigratingHandle::getText(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getText(index);
}

MigratingHandle::BLOB MigratingHandle::getBLOB(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getBLOB(index);
}

ColumnType MigratingHandle::getType(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getType(index);
}

const UnsafeString MigratingHandle::getOriginColumnName(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getOriginColumnName(index);
}

const UnsafeString MigratingHandle::getColumnName(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getColumnName(index);
}

const UnsafeString MigratingHandle::getColumnTableName(int index)
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getColumnTableName(index);
}

bool MigratingHandle::isStatementReadonly()
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->isReadonly();
}

int MigratingHandle::getNumberOfColumns()
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getNumberOfColumns();
}

#pragma mark - Migrate
bool MigratingHandle::isMigratedPrepared()
{
    WCTInnerAssert(m_migrateStatement->isPrepared()
                   == m_removeMigratedStatement->isPrepared());
    return m_migrateStatement->isPrepared() /* || m_removeMigratedStatement->isPrepared() */;
}

bool MigratingHandle::stepMigration(const int64_t& rowid)
{
    WCTInnerAssert(isMigratedPrepared());
    m_migrateStatement->bindInteger64(rowid, 1);
    m_removeMigratedStatement->bindInteger64(rowid, 1);
    return m_migrateStatement->step() && m_removeMigratedStatement->step();
}

void MigratingHandle::finalizeMigrate()
{
    m_migrateStatement->finalize();
    m_removeMigratedStatement->finalize();
}

void MigratingHandle::resetMigrate()
{
    WCTInnerAssert(isMigratedPrepared());
    m_migrateStatement->reset();
    m_removeMigratedStatement->reset();
}

bool MigratingHandle::prepareMigrate(const String& table, Syntax::ConflictAction conflictAction)
{
    WCTInnerAssert(!isMigratedPrepared());
    const MigrationInfo* info = getBoundInfo(table);
    WCTInnerAssert(info != nullptr);
    return m_migrateStatement->prepare(info->getStatementForMigratingSpecifiedRow(conflictAction))
           && m_removeMigratedStatement->prepare(info->getStatementForDeletingSpecifiedRow());
}

} //namespace WCDB
