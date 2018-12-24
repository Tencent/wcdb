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
: ConfigurableHandle()
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

#pragma mark - Bind
bool MigrationHandle::rebind(const std::map<String, RecyclableMigrationInfo>& migratings)
{
    bool succeed;

    // views
    std::map<String, const MigrationInfo*> infosToCreateView; // view -> info
    for (const auto& iter : migratings) {
        const MigrationInfo* info = iter.second.get();
        infosToCreateView.emplace(info->getUnionedView(), info);
    }

    std::set<String> createdViews;

    bool exists;
    std::tie(succeed, exists) = tableExists(Schema::temp(), Syntax::masterTable);
    if (!succeed) {
        return false;
    }
    if (exists) {
        std::tie(succeed, createdViews)
        = getValues(MigrationInfo::getStatementForSelectingUnionedView(), 0);
        if (!succeed) {
            return false;
        }
    }

    for (const auto& createdView : createdViews) {
        WCTInnerAssert(createdView.hasPrefix(MigrationInfo::getUnionedViewPrefix()));
        auto iter = infosToCreateView.find(createdView);
        if (iter != infosToCreateView.end()) {
            // it is already created
            infosToCreateView.erase(iter);
        } else {
            // it is no longer needed
            if (!executeStatement(MigrationInfo::getStatementForDroppingUnionedView(createdView))) {
                return false;
            }
        }
    }
    // create all needed views
    for (const auto& iter : infosToCreateView) {
        const MigrationInfo* infoToCreateView = iter.second;
        if (!executeStatement(infoToCreateView->getStatementForCreatingUnionedView())) {
            return false;
        }
    }

    // schemas
    std::map<String, const MigrationInfo*> infosToAttachSchema; // schema -> info
    for (const auto& iter : migratings) {
        const MigrationInfo* info = iter.second.get();
        if (!info->isSameDatabaseMigration()) {
            infosToAttachSchema.emplace(
            info->getSchemaForOriginDatabase().getDescription(), info);
        }
    }

    std::set<String> attachedSchemas;
    std::tie(succeed, attachedSchemas)
    = getValues(MigrationInfo::getStatementForSelectingDatabaseList(), 1);
    if (!succeed) {
        return false;
    }

    for (const auto& attachedSchema : attachedSchemas) {
        if (attachedSchema.hasPrefix(MigrationInfo::getSchemaPrefix())) {
            auto iter = infosToAttachSchema.find(attachedSchema);
            if (iter != infosToAttachSchema.end()) {
                // it is already attached
                infosToAttachSchema.erase(iter);
            } else {
                // it is not longer needed
                if (!executeStatement(
                    MigrationInfo::getStatementForDetachingSchema(attachedSchema))) {
                    return false;
                }
            }
        }
    }
    // attach all needed schemas
    for (const auto& iter : infosToAttachSchema) {
        const MigrationInfo* infoToAttachSchema = iter.second;
        if (!executeStatement(infoToAttachSchema->getStatementForAttachingSchema())) {
            return false;
        }
    }

    return true;
}

std::pair<bool, std::set<String>>
MigrationHandle::getOriginColumns(const MigrationUserInfo& userInfo)
{
    // schema has no need to be detached since schema will be managed automatically when rebind.
    if (!userInfo.isSameDatabaseMigration()
        && !executeStatement(userInfo.getStatementForAttachingSchema())) {
        return { false, {} };
    }
    return getColumns(userInfo.getSchemaForOriginDatabase(), userInfo.getOriginTable());
}

String MigrationHandle::getMigratedDatabasePath() const
{
    return getPath();
}

#pragma mark - Migration
std::pair<bool, std::list<Statement>> MigrationHandle::process(const Statement& statement)
{
#ifdef DEBUG
    m_processing = true;
#endif

    bool succeed = true;
    std::list<Statement> statements;
    do {
        Statement falledBackStatement = statement;
        // fallback
        falledBackStatement.iterate(
        [&succeed, this](Syntax::Identifier& identifier, bool& stop) {
            switch (identifier.getType()) {
            case Syntax::Identifier::Type::TableOrSubquery: {
                // main.migratedTable -> temp.unionedView
                Syntax::TableOrSubquery& syntax = (Syntax::TableOrSubquery&) identifier;
                if (syntax.switcher == Syntax::TableOrSubquery::Switch::Table
                    && syntax.schema.isMain()) {
                    const MigrationInfo* info;
                    std::tie(succeed, info) = prepareInfo(syntax.tableOrFunction);
                    if (succeed && info) {
                        syntax.schema = Schema::temp();
                        syntax.tableOrFunction = info->getUnionedView();
                    }
                }
            } break;
            case Syntax::Identifier::Type::QualifiedTableName: {
                // main.migratedTable -> schemaForOriginDatabase.originTable
                Syntax::QualifiedTableName& syntax = (Syntax::QualifiedTableName&) identifier;
                if (syntax.schema.isMain()) {
                    const MigrationInfo* info;
                    std::tie(succeed, info) = prepareInfo(syntax.table);
                    if (succeed && info) {
                        syntax.schema = info->getSchemaForOriginDatabase();
                        syntax.table = info->getOriginTable();
                    }
                }
            } break;
            case Syntax::Identifier::Type::DropTableSTMT: {
                // main.migratedTable -> schemaForOriginDatabase.originTable
                Syntax::DropTableSTMT& syntax = (Syntax::DropTableSTMT&) identifier;
                if (syntax.schema.isMain()) {
                    const MigrationInfo* info;
                    std::tie(succeed, info) = prepareInfo(syntax.table);
                    if (succeed && info) {
                        syntax.schema = info->getSchemaForOriginDatabase();
                        syntax.table = info->getOriginTable();
                    }
                }
            } break;
            default:
                break;
            }
            if (!succeed) {
                stop = true;
            }
        });
        if (succeed) {
            // rebind
            succeed = Migration::Binder::rebind();
        }
        if (!succeed) {
            break;
        }

        switch (statement.getType()) {
        case Syntax::Identifier::Type::InsertSTMT: {
            statements.push_back(falledBackStatement);
            const Syntax::InsertSTMT* originSyntax
            = static_cast<const Syntax::InsertSTMT*>(statement.getSyntaxIdentifier());
            const Syntax::InsertSTMT* falledBackSyntax
            = static_cast<const Syntax::InsertSTMT*>(falledBackStatement.getSyntaxIdentifier());
            if (originSyntax->table != falledBackSyntax->table) {
                succeed = prepareMigrate(falledBackStatement);
            }
        } break;
        case Syntax::Identifier::Type::UpdateSTMT: {
            const Syntax::UpdateSTMT* originSyntax
            = static_cast<const Syntax::UpdateSTMT*>(statement.getSyntaxIdentifier());
            const Syntax::UpdateSTMT* falledBackSyntax
            = static_cast<const Syntax::UpdateSTMT*>(falledBackStatement.getSyntaxIdentifier());
            if (originSyntax->table.table == falledBackSyntax->table.table) {
                statements.push_back(falledBackStatement);
            } else {
                const MigrationInfo* info = getBoundInfo(originSyntax->table.table);
                WCTInnerAssert(info != nullptr);
                statements.push_back(
                info->getStatementForLimitedUpdatingTable(falledBackStatement));
                statements.push_back(info->getStatementForLimitedUpdatingTable(statement));
            }
        } break;
        case Syntax::Identifier::Type::DeleteSTMT: {
            const Syntax::DeleteSTMT* originSyntax
            = static_cast<const Syntax::DeleteSTMT*>(statement.getSyntaxIdentifier());
            const Syntax::DeleteSTMT* falledBackSyntax
            = static_cast<const Syntax::DeleteSTMT*>(falledBackStatement.getSyntaxIdentifier());
            if (originSyntax->table.table == falledBackSyntax->table.table) {
                statements.push_back(falledBackStatement);
            } else {
                const MigrationInfo* info = getBoundInfo(originSyntax->table.table);
                WCTInnerAssert(info != nullptr);
                statements.push_back(
                info->getStatementForLimitedDeletingFromTable(falledBackStatement));
                statements.push_back(info->getStatementForLimitedDeletingFromTable(statement));
            }
        }
        case Syntax::Identifier::Type::DropTableSTMT: {
            const Syntax::DropTableSTMT* originSyntax
            = static_cast<const Syntax::DropTableSTMT*>(statement.getSyntaxIdentifier());
            const Syntax::DropTableSTMT* falledBackSyntax
            = static_cast<const Syntax::DropTableSTMT*>(
            falledBackStatement.getSyntaxIdentifier());
            statements.push_back(statement);
            if (originSyntax->table != falledBackSyntax->table) {
                statements.push_back(falledBackStatement);
            }
        } break;
        default:
            statements.push_back(falledBackStatement);
            break;
        }
    } while (false);
    if (!succeed) {
        clearPrepared();
        statements.clear();
    }
#ifdef DEBUG
    m_processing = false;
#endif
    return { succeed, std::move(statements) };
}

#pragma mark - Override
bool MigrationHandle::execute(const Statement& statement)
{
    WCTInnerAssert(!m_processing);
    bool succeed;
    std::list<Statement> statements;
    std::tie(succeed, statements) = process(statement);
    if (!succeed) {
        return false;
    }
    if (statements.size() > 1 || isMigratedPrepared()) {
        succeed = runNestedTransaction([this, &statements](Handle*) -> bool {
            return realExecute(statements);
        });
    } else {
        succeed = realExecute(statements);
    }
    if (!succeed && isMigratedPrepared()) {
        finalizeMigrate();
    }
    return succeed;
}

bool MigrationHandle::realExecute(const std::list<Statement>& statements)
{
    WCTInnerAssert(!statements.empty());
    for (const auto& statement : statements) {
        if (!Handle::execute(statement)) {
            return false;
        }
    }
    bool succeed = true;
    if (isMigratedPrepared()) {
        WCTInnerAssert(statements.size() == 1);
        succeed = stepMigrate(getLastInsertedRowID());
        finalizeMigrate();
    }
    return succeed;
}

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
           && (!isMigratedPrepared() || stepMigrate(getLastInsertedRowID()));
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

int MigrationHandle::getColumnCount()
{
    WCTInnerAssert(!m_additionalStatement->isPrepared());
    return m_mainStatement->getColumnCount();
}

#pragma mark - Migrate
bool MigrationHandle::isMigratedPrepared()
{
    WCTInnerAssert(m_migrateStatement->isPrepared()
                   == m_removeMigratedStatement->isPrepared());
    return m_migrateStatement->isPrepared() /* || m_removeMigratedStatement->isPrepared() */;
}

bool MigrationHandle::stepMigrate(const int64_t& rowid)
{
    WCTInnerAssert(isMigratedPrepared());
    m_migrateStatement->bindInteger64(rowid, 1);
    m_removeMigratedStatement->bindInteger64(rowid, 1);
    return m_migrateStatement->step() && m_removeMigratedStatement->step();
}

void MigrationHandle::finalizeMigrate()
{
    m_migrateStatement->finalize();
    m_additionalStatement->finalize();
}

void MigrationHandle::resetMigrate()
{
    WCTInnerAssert(isMigratedPrepared());
    m_migrateStatement->reset();
    m_additionalStatement->reset();
}

bool MigrationHandle::prepareMigrate(const Statement& statement)
{
    WCTInnerAssert(statement.getType() == Syntax::Identifier::Type::InsertSTMT);
    WCTInnerAssert(!isMigratedPrepared());
    const MigrationInfo* info = getBoundInfo(
    static_cast<Syntax::InsertSTMT*>(statement.getSyntaxIdentifier())->table);
    WCTInnerAssert(info != nullptr);
    return m_migrateStatement->prepare(info->getStatementForMigratingSpecifiedRow(statement))
           && m_migrateStatement->prepare(info->getStatementForDeletingSpecifiedRow());
}

} //namespace WCDB
