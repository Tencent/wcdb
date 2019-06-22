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
#include <WCDB/StringView.hpp>

namespace WCDB {

#pragma mark - Initialize
MigratingHandle::MigratingHandle(Migration& migration)
: Handle()
, Migration::Binder(migration)
, m_additionalStatement(getStatement())
, m_migrateStatement(getStatement())
, m_removeMigratedStatement(getStatement())
, m_processing(false)
{
}

MigratingHandle::~MigratingHandle()
{
    returnStatement(m_additionalStatement);
    returnStatement(m_migrateStatement);
    returnStatement(m_removeMigratedStatement);
}

#pragma mark - Info Initializer
std::optional<std::pair<bool, std::set<StringView>>>
MigratingHandle::getColumnsOfUserInfo(const MigrationUserInfo& userInfo)
{
    auto exists = tableExists(Schema::main(), userInfo.getTable());
    if (!exists.has_value()) {
        return std::nullopt;
    }

    bool integerPrimary = false;
    std::set<StringView> names;
    if (exists.value()) {
        auto optionalMetas = getTableMeta(Schema::main(), userInfo.getTable());
        if (!optionalMetas.has_value()) {
            return std::nullopt;
        }
        auto& metas = optionalMetas.value();
        integerPrimary = ColumnMeta::getIndexOfIntegerPrimary(metas) >= 0;
        for (const auto& meta : metas) {
            names.emplace(meta.name);
        }
    }
    return std::make_pair(integerPrimary, names);
}

std::optional<bool> MigratingHandle::sourceTableExists(const MigrationUserInfo& userInfo)
{
    Schema schema = userInfo.getSchemaForSourceDatabase();
    if (!schema.syntax().isMain()) {
        auto optionalAttacheds
        = getValues(MigrationInfo::getStatementForSelectingDatabaseList(), 1);
        if (!optionalAttacheds.has_value()) {
            return std::nullopt;
        }
        std::set<StringView>& attacheds = optionalAttacheds.value();
        if (attacheds.find(schema.getDescription()) == attacheds.end()) {
            if (!executeStatement(userInfo.getStatementForAttachingSchema())
                || !trySynchronousTransactionAfterAttached()) {
                return std::nullopt;
            }
        }
    }
    return tableExists(schema, userInfo.getSourceTable());
    ;
}

StringView MigratingHandle::getDatabasePath() const
{
    return getPath();
}

#pragma mark - Binder
bool MigratingHandle::rebindViews(const StringViewMap<const MigrationInfo*>& migratings)
{
    StringViewMap<const MigrationInfo*> views2MigratingInfos;
    for (const auto& iter : migratings) {
        const MigrationInfo* info = iter.second;
        WCTAssert(info != nullptr);
        views2MigratingInfos.emplace(info->getUnionedView(), info);
    }

    // get existing unioned views
    auto exists = tableExists(Schema::temp(), Syntax::masterTable);
    if (!exists.has_value()) {
        return false;
    }
    if (exists.value()) {
        auto existingViews
        = getValues(MigrationInfo::getStatementForSelectingUnionedView(), 0);
        if (!existingViews.has_value()) {
            return false;
        }

        for (const auto& existingView : existingViews.value()) {
            WCTAssert(existingView.hasPrefix(MigrationInfo::getUnionedViewPrefix()));
            auto iter = views2MigratingInfos.find(existingView);
            if (iter != views2MigratingInfos.end()) {
                // it is already created
                views2MigratingInfos.erase(iter);
            } else {
                // it is no longer needed
                if (!executeStatement(
                    MigrationInfo::getStatementForDroppingUnionedView(existingView))) {
                    return false;
                }
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

bool MigratingHandle::rebindSchemas(const StringViewMap<const MigrationInfo*>& migratings)
{
    StringViewMap<const MigrationInfo*> schemas2MigratingInfos;
    for (const auto& iter : migratings) {
        const MigrationInfo* info = iter.second;
        WCTAssert(info != nullptr);
        if (info->isCrossDatabase()) {
            schemas2MigratingInfos.emplace(
            info->getSchemaForSourceDatabase().getDescription(), info);
        }
    }

    auto existingSchemas
    = getValues(MigrationInfo::getStatementForSelectingDatabaseList(), 1);
    if (!existingSchemas.has_value()) {
        return false;
    }

    for (const auto& existingSchema : existingSchemas.value()) {
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
    }
    if (attached) {
        return trySynchronousTransactionAfterAttached();
    }
    return true;
}

bool MigratingHandle::bindInfos(const StringViewMap<const MigrationInfo*>& migratings)
{
    return rebindViews(migratings) && rebindSchemas(migratings);
}

bool MigratingHandle::trySynchronousTransactionAfterAttached()
{
    bool succeed = true;
    if (isInTransaction()) {
        markErrorAsIgnorable(Error::Code::Error);
        static const StringView* s_synchronousTransaction
        = new StringView(StatementBegin().beginImmediate().getDescription());
        succeed = executeSQL(*s_synchronousTransaction);
        WCTAssert(!succeed);
        if (!succeed && isErrorIgnorable()) {
            succeed = true;
        }
        markErrorAsUnignorable();
    }
    return succeed;
}

#pragma mark - Migration
std::pair<bool, std::list<Statement>> MigratingHandle::process(const Statement& originStatement)
{
    m_processing = true;

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
                const MigrationInfo* info = getBoundInfo(migratedTableName);
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
                const MigrationInfo* info = getBoundInfo(migratedTableName);
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
                const MigrationInfo* info = getBoundInfo(migratedSTMT.table);
                WCTAssert(info != nullptr);
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
    m_processing = false;
    return { succeed, std::move(statements) };
}

bool MigratingHandle::tryFallbackToUnionedView(Syntax::Schema& schema, StringView& table)
{
    if (schema.isMain()) {
        auto optionalInfo = bindTable(table);
        if (!optionalInfo.has_value()) {
            return false;
        }
        auto info = optionalInfo.value();
        if (info != nullptr) {
            schema = Schema::temp();
            table = info->getUnionedView();
        }
    }
    return true;
}

bool MigratingHandle::tryFallbackToSourceTable(Syntax::Schema& schema, StringView& table)
{
    if (schema.isMain()) {
        auto optionalInfo = bindTable(table);
        if (!optionalInfo.has_value()) {
            return false;
        }
        auto info = optionalInfo.value();
        if (info != nullptr) {
            schema = info->getSchemaForSourceDatabase();
            table = info->getSourceTable();
        }
    }
    return true;
}

#pragma mark - Override
bool MigratingHandle::prepare(const Statement& statement)
{
    WCTAssert(!m_processing);
    WCTRemedialAssert(!isPrepared(), "Last statement is not finalized.", finalize(););
    bool succeed;
    std::list<Statement> statements;
    std::tie(succeed, statements) = process(statement);
    if (!succeed) {
        return false;
    }
    WCTAssert(statements.size() <= 2);
    if (Super::prepare(statements.front())
        && (statements.size() == 1 || m_additionalStatement->prepare(statements.back()))) {
        return true;
    }
    finalize();
    return false;
}

void MigratingHandle::finalize()
{
    Super::finalize();
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
    WCTAssert(!(m_additionalStatement->isPrepared() && isMigratedPrepared()));
    return Super::step()
           && (!m_additionalStatement->isPrepared() || m_additionalStatement->step())
           && (!isMigratedPrepared() || stepMigration(getLastInsertedRowID()));
}

void MigratingHandle::reset()
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

void MigratingHandle::bindInteger32(const Integer32& value, int index)
{
    Super::bindInteger32(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindInteger32(value, index);
    }
    if (m_migrateStatement->isPrepared()) {
        WCTRemedialAssert(m_rowidIndexOfMigratingStatement == 0 || index != m_rowidIndexOfMigratingStatement,
                          "Binding index is out of range",
                          return;);
        m_migrateStatement->bindInteger32(value, index);
    }
}

void MigratingHandle::bindInteger64(const Integer64& value, int index)
{
    Super::bindInteger64(value, index);
    if (m_additionalStatement->isPrepared()) {
        m_additionalStatement->bindInteger64(value, index);
    }
    if (m_migrateStatement->isPrepared()) {
        WCTRemedialAssert(m_rowidIndexOfMigratingStatement == 0 || index != m_rowidIndexOfMigratingStatement,
                          "Binding index is out of range",
                          return;);
        m_migrateStatement->bindInteger64(value, index);
    }
}

void MigratingHandle::bindDouble(const Float& value, int index)
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

void MigratingHandle::bindText(const Text& value, int index)
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

void MigratingHandle::bindBLOB(const BLOB& value, int index)
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

void MigratingHandle::bindNull(int index)
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

#pragma mark - Migrate
bool MigratingHandle::isMigratedPrepared()
{
    WCTAssert(m_migrateStatement->isPrepared() == m_removeMigratedStatement->isPrepared());
    return m_migrateStatement->isPrepared() /* || m_removeMigratedStatement->isPrepared() */;
}

bool MigratingHandle::stepMigration(const int64_t& rowid)
{
    WCTAssert(isMigratedPrepared());
    m_removeMigratedStatement->bindInteger64(rowid, 1);
    if (m_rowidIndexOfMigratingStatement > 0) {
        m_migrateStatement->bindInteger64(rowid, m_rowidIndexOfMigratingStatement);
    }
    return m_removeMigratedStatement->step() && m_migrateStatement->step();
}

void MigratingHandle::finalizeMigrate()
{
    m_removeMigratedStatement->finalize();
    m_migrateStatement->finalize();
}

void MigratingHandle::resetMigrate()
{
    WCTAssert(isMigratedPrepared());
    m_removeMigratedStatement->reset();
    m_migrateStatement->reset();
}

bool MigratingHandle::prepareMigrate(const Syntax::InsertSTMT& migrated,
                                     const Syntax::InsertSTMT& falledBack)
{
    WCTAssert(!isMigratedPrepared());
    const MigrationInfo* info = getBoundInfo(migrated.table);
    WCTAssert(info != nullptr);
    m_rowidIndexOfMigratingStatement = info->getRowIDIndexOfMigratingStatement();
    return m_removeMigratedStatement->prepare(info->getStatementForDeletingSpecifiedRow())
           && m_migrateStatement->prepare(info->getStatementForMigrating(falledBack));
}

} //namespace WCDB
