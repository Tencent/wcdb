//
// Created by sanhuazhang on 2019/05/19
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

#include "HandleStatement.hpp"
#include "AbstractHandle.hpp"
#include "Assertion.hpp"
#include "BaseBinding.hpp"
#include "CommonCore.hpp"
#include "CompressingHandleDecorator.hpp"
#include "CompressionConst.hpp"
#include "DecorativeHandle.hpp"
#include "InnerHandle.hpp"
#include "MigratingHandleDecorator.hpp"
#include "MigrationInfo.hpp"
#include "SQLite.h"
#include "WINQ.h"
#include <iomanip>
#include <string.h>

namespace WCDB {

HandleStatement::HandleStatement(HandleStatement &&other)
: HandleRelated(other.getHandle())
, m_stmt(other.m_stmt)
, m_done(other.m_done)
, m_newTable(other.m_newTable)
, m_modifiedTable(other.m_modifiedTable)
, m_needAutoAddColumn(other.m_needAutoAddColumn)
, m_sql(other.m_sql)
, m_fullTrace(other.m_fullTrace)
, m_needReport(other.m_needReport)
, m_stepCount(other.m_stepCount)
{
    other.m_done = false;
    other.m_stmt = nullptr;
}

HandleStatement::HandleStatement(AbstractHandle *handle)
: HandleRelated(handle)
, m_stmt(nullptr)
, m_done(false)
, m_needAutoAddColumn(false)
, m_fullTrace(handle->isFullSQLEnable())
, m_needReport(false)
, m_stepCount(0)
{
}

HandleStatement::~HandleStatement()
{
    finalize();
}

bool HandleStatement::prepare(const Statement &statement)
{
    if (getHandle()->needMonitorTable()) {
        analysisStatement(statement);
    }

    const StringView &sql = statement.getDescription();
    if (m_needAutoAddColumn) {
        cacheCurrentTransactionError();
    }
    if (prepareSQL(sql)) {
        return true;
    }

    if (!m_needAutoAddColumn) {
        return false;
    }

    auto statementType = statement.getType();
    if (statementType != Syntax::Identifier::Type::InsertSTMT
        && statementType != Syntax::Identifier::Type::DeleteSTMT
        && statementType != Syntax::Identifier::Type::UpdateSTMT
        && statementType != Syntax::Identifier::Type::SelectSTMT) {
        return false;
    }

    const Error &error = getHandle()->getError();
    if (error.code() != Error::Code::Error || (int) error.getExtCode() != 0) {
        return false;
    }
    const StringView &msg = error.getMessage();
    if (!msg.hasPrefix("no such column: ")
        && !(statement.getType() == Syntax::Identifier::Type::InsertSTMT
             && msg.hasPrefix("table ") && msg.contain(" has no column named "))) {
        return false;
    }

    InnerHandle *innerHandle = dynamic_cast<InnerHandle *>(getHandle());
    if (!innerHandle) {
        return false;
    }

    StringView columnName;
    StringView tableName;
    StringView schemaName;
    const BaseBinding *binding = nullptr;
    if (!tryExtractColumnInfo(statement, msg, columnName, tableName, schemaName, &binding)) {
        return false;
    }
    if (binding != nullptr) {
        if (!binding->tryRecoverColumn(columnName, tableName, schemaName, sql, innerHandle)) {
            return false;
        }
    } else {
        WCTAssert(columnName.hasPrefix(CompressionColumnTypePrefix));
        DecorativeHandle *decorativeHandle
        = dynamic_cast<DecorativeHandle *>(getHandle());
        if (decorativeHandle != nullptr
            && decorativeHandle->containDecorator(DecoratorCompressingHandle)) {
            CompressingHandleDecorator *compressingDecorator
            = decorativeHandle->getDecorator<CompressingHandleDecorator>(DecoratorCompressingHandle);
            if (!compressingDecorator->tryFixCompressingColumn(tableName)) {
                return false;
            }
        }
    }

    resumeCacheTransactionError();
    return prepareSQL(sql);
}

void HandleStatement::analysisStatement(const Statement &statement)
{
    m_modifiedTable.clear();
    m_newTable.clear();
    switch (statement.getType()) {
    case Syntax::Identifier::Type::InsertSTMT: {
        const Syntax::InsertSTMT &insertSTMT
        = static_cast<const Syntax::InsertSTMT &>(statement.syntax());
        m_modifiedTable = insertSTMT.table;
    } break;
    case Syntax::Identifier::Type::UpdateSTMT: {
        const Syntax::UpdateSTMT &updateSTMT
        = static_cast<const Syntax::UpdateSTMT &>(statement.syntax());
        m_modifiedTable = updateSTMT.table.table;
    } break;
    case Syntax::Identifier::Type::DeleteSTMT: {
        const Syntax::DeleteSTMT &deleteSTMT
        = static_cast<const Syntax::DeleteSTMT &>(statement.syntax());
        m_modifiedTable = deleteSTMT.table.table;
    } break;
    case Syntax::Identifier::Type::AlterTableSTMT: {
        const Syntax::AlterTableSTMT &alterSTMT
        = static_cast<const Syntax::AlterTableSTMT &>(statement.syntax());
        if (alterSTMT.switcher == Syntax::AlterTableSTMT::Switch::RenameTable) {
            m_modifiedTable = alterSTMT.newTable;
        }
    } break;
    case Syntax::Identifier::Type::CreateTableSTMT: {
        const Syntax::CreateTableSTMT &createSTMT
        = static_cast<const Syntax::CreateTableSTMT &>(statement.syntax());
        m_newTable = createSTMT.table;
    } break;
    case Syntax::Identifier::Type::CreateVirtualTableSTMT: {
        const Syntax::CreateVirtualTableSTMT &createSTMT
        = static_cast<const Syntax::CreateVirtualTableSTMT &>(statement.syntax());
        m_newTable = createSTMT.table;
    } break;
    default:
        break;
    }
}

bool HandleStatement::tryExtractColumnInfo(const Statement &statement,
                                           const StringView &msg,
                                           StringView &columnName,
                                           StringView &tableName,
                                           StringView &schemaName,
                                           const BaseBinding **binding)
{
    StringView columnInfo[3];
    const char *msgStr = msg.data();
    bool isInsert = false;
    if (msg.hasPrefix("no such column: ")) {
        int index = 2;
        size_t preLocation = msg.length();
        int i = (int) msg.length() - 2;
        for (; i >= 17; i--) {
            if (msgStr[i] == ' ') {
                return false;
            }
            if (msgStr[i] == '.') {
                columnInfo[index] = StringView(msgStr + i + 1, preLocation - i - 1);
                preLocation = i;
                index--;
            }
            if (index < 0) {
                return false;
            }
        }
        columnInfo[index] = StringView(msgStr + i, preLocation - i);
    } else {
        isInsert = true;
        offset_t firstLoc = msg.find(" has no column named ");
        const UnsafeStringView tablePart(msgStr + 6, firstLoc - 6);
        offset_t secondLoc = tablePart.find(".");
        if (secondLoc == UnsafeStringView::npos) {
            columnInfo[1] = tablePart;
        } else {
            columnInfo[0] = StringView(tablePart.data(), secondLoc);
            columnInfo[1] = StringView(tablePart.data() + secondLoc + 1,
                                       tablePart.length() - secondLoc - 1);
        }
        columnInfo[2]
        = StringView(msg.data() + firstLoc + 21, msg.length() - firstLoc - 21);
    }

    schemaName = columnInfo[0];
    tableName = columnInfo[1];
    columnName = columnInfo[2];
    if (columnName.length() == 0) {
        return false;
    }

    bool tableSpecified = tableName.length() > 0;
    bool schemaSpecified = schemaName.length() > 0;

    if (schemaSpecified) {
        if (tableName.hasPrefix(MigrationInfo::getUnionedViewPrefix())
            && schemaName.compare(Syntax::tempSchema) == 0) {
            size_t prefixLength = strlen(MigrationInfo::getUnionedViewPrefix());
            tableName = StringView(tableName.data() + prefixLength,
                                   tableName.length() - prefixLength);
            schemaName = "";
            schemaSpecified = false;
        } else if (schemaName.hasPrefix(MigrationDatabaseInfo::getSchemaPrefix())) {
            schemaName = "";
            schemaSpecified = false;
        }
    }

    WCTAssert(tableSpecified || !schemaSpecified);
    bool findTable = !tableSpecified;
    bool isCompressingColumn = columnName.hasPrefix(CompressionColumnTypePrefix);
    Statement copyStatement = statement;
    bool invalidStatement = false;

    copyStatement.syntax();
    copyStatement.iterate([&](Syntax::Identifier &identifier, bool isBegin, bool &stop) {
        if (!isBegin) {
            return;
        }
        StringView curTableName;
        Syntax::Schema curSchema;
        bool needCheckTable = false;
        switch (identifier.getType()) {
        case Syntax::Identifier::Type::Column: {
            Syntax::Column &column = (Syntax::Column &) identifier;
            if (isCompressingColumn) {
                return;
            }
            if (column.name.compare(columnName) != 0) {
                return;
            }
            if (!isInsert) {
                if ((tableSpecified && column.table.compare(tableName) != 0)
                    || (!tableSpecified && column.table.length() > 0)) {
                    return;
                }
                if ((schemaSpecified && column.schema.name.compare(schemaName) != 0)
                    || (!schemaSpecified && tableSpecified && !column.schema.isMain())) {
                    return;
                }
            }
            const BaseBinding *newBinding = column.getTableBinding();
            if (newBinding != nullptr && *binding != nullptr && newBinding != *binding) {
                invalidStatement = true;
                stop = true;
                return;
            }
            *binding = newBinding;
        } break;
        case Syntax::Identifier::Type::QualifiedTableName: {
            Syntax::QualifiedTableName &qualifiedTable
            = (Syntax::QualifiedTableName &) identifier;
            curTableName = qualifiedTable.table;
            curSchema = qualifiedTable.schema;
            needCheckTable = true;
        } break;
        case Syntax::Identifier::Type::TableOrSubquery: {
            Syntax::TableOrSubquery &tableOrSubquery = (Syntax::TableOrSubquery &) identifier;
            if (tableOrSubquery.switcher == Syntax::TableOrSubquery::Switch::Table) {
                curTableName = tableOrSubquery.tableOrFunction;
                curSchema = tableOrSubquery.schema;
                needCheckTable = true;
            }
        } break;
        case Syntax::Identifier::Type::InsertSTMT: {
            Syntax::InsertSTMT &insert = (Syntax::InsertSTMT &) identifier;
            curTableName = insert.table;
            curSchema = insert.schema;
            needCheckTable = true;
        } break;
        default:
            break;
        }
        if (!needCheckTable) {
            return;
        }

        if (curTableName.hasPrefix(MigrationInfo::getUnionedViewPrefix())
            && curSchema.isTemp()) {
            size_t prefixLength = strlen(MigrationInfo::getUnionedViewPrefix());
            curTableName = StringView(curTableName.data() + prefixLength,
                                      curTableName.length() - prefixLength);
            curSchema = Schema::main();
        }
        if (curSchema.name.hasPrefix(MigrationDatabaseInfo::getSchemaPrefix())) {
            curSchema = Schema::main();
        }

        if (!tableSpecified) {
            if (tableName.length() > 0 && tableName.compare(curTableName) != 0) {
                DecorativeHandle *decorativeHandle
                = dynamic_cast<DecorativeHandle *>(getHandle());
                if (decorativeHandle != nullptr
                    && decorativeHandle->containDecorator(DecoratorMigratingHandle)) {
                    MigratingHandleDecorator *migratingDecorator
                    = decorativeHandle->getDecorator<MigratingHandleDecorator>(DecoratorMigratingHandle);
                    if (!migratingDecorator->checkSourceTable(tableName, curTableName)
                        && !migratingDecorator->checkSourceTable(curTableName, tableName)) {
                        invalidStatement = true;
                    }
                } else {
                    invalidStatement = true;
                }
            }
            if (schemaName.length() > 0 && curSchema.name.compare(schemaName) != 0) {
                invalidStatement = true;
            }
            if (invalidStatement) {
                stop = true;
                return;
            }
            tableName = curTableName;
            if (!curSchema.isMain()) {
                schemaName = curSchema.name;
            }
        } else if (!findTable && curTableName.compare(tableName) == 0) {
            if ((schemaName.length() == 0 && curSchema.isMain())
                || (schemaName.length() > 0 && curSchema.name.compare(schemaName) == 0)) {
                findTable = true;
            }
        }
    });
    return (*binding != nullptr || isCompressingColumn) && findTable && !invalidStatement;
}

bool HandleStatement::prepareSQL(const UnsafeStringView &sql)
{
    WCTRemedialAssert(!isPrepared(), "Last statement is not finalized.", finalize(););
    WCTAssert(sql.length() > 0);

    bool result = APIExit(
    sqlite3_prepare_v2(getRawHandle(), sql.data(), -1, &m_stmt, nullptr), sql);
    m_done = false;
    m_fullTrace = getHandle()->isFullSQLEnable();
    if (!result) {
        m_stmt = nullptr;
    } else {
        if (m_fullTrace) {
            clearReport();
        }
        if (isBusyTraceEnable()) {
            m_sql = sql;
        }
    }
    return result;
}

void HandleStatement::reset()
{
    WCTAssert(isPrepared());
    tryReportSQL();
    APIExit(sqlite3_reset(m_stmt));
}

void HandleStatement::clearBindings()
{
    WCTAssert(isPrepared());
    APIExit(sqlite3_clear_bindings(m_stmt));
}

bool HandleStatement::done()
{
    return m_done;
}

bool HandleStatement::step()
{
    WCTAssert(isPrepared());

    if (isBusyTraceEnable()) {
        setCurrentSQL(m_sql);
    }

    int rc = sqlite3_step(m_stmt);
    m_done = rc == SQLITE_DONE;

    if (m_fullTrace) {
        m_needReport = true;
    }

    if (m_done) {
        if (getHandle()->needMonitorTable()
            && (!m_newTable.empty() || !m_modifiedTable.empty())) {
            getHandle()->postTableNotification(m_newTable, m_modifiedTable);
        }
        tryReportSQL();
    } else {
        m_stepCount++;
    }

    const char *sql = nullptr;
    if (isPrepared()) {
        // There will be privacy issues if use sqlite3_expanded_sql
        sql = sqlite3_sql(m_stmt);
    }
    return APIExit(rc, sql);
}

void HandleStatement::finalize()
{
    if (m_stmt != nullptr) {
        tryReportSQL();
        // no need to call APIExit since it returns old code only.
        sqlite3_finalize(m_stmt);
        m_stmt = nullptr;
        resetCurrentSQL(m_sql);
        m_sql.clear();
    }
}

int HandleStatement::getNumberOfColumns()
{
    WCTAssert(isPrepared());
    return sqlite3_column_count(m_stmt);
}

const UnsafeStringView HandleStatement::getOriginColumnName(int index)
{
    WCTAssert(isPrepared());
    return sqlite3_column_origin_name(m_stmt, index);
}

const UnsafeStringView HandleStatement::getColumnName(int index)
{
    WCTAssert(isPrepared());
    return sqlite3_column_name(m_stmt, index);
}

const UnsafeStringView HandleStatement::getColumnTableName(int index)
{
    WCTAssert(isPrepared());
    return sqlite3_column_table_name(m_stmt, index);
}

int HandleStatement::getBindParameterCount()
{
    WCTAssert(isPrepared());
    return sqlite3_bind_parameter_count(m_stmt);
}

ColumnType HandleStatement::getType(int index)
{
    WCTAssert(isPrepared());
    switch (sqlite3_column_type(m_stmt, index)) {
    case SQLITE_INTEGER:
        return ColumnType::Integer;
    case SQLITE_FLOAT:
        return ColumnType::Float;
    case SQLITE_BLOB:
        return ColumnType::BLOB;
    case SQLITE_TEXT:
        return ColumnType::Text;
    default:
        return ColumnType::Null;
    }
}

void HandleStatement::bindInteger(const Integer &value, int index)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    bool succeed = APIExit(sqlite3_bind_int64(m_stmt, index, value));
    if (succeed && m_fullTrace) {
        m_stream << index << ":" << value << ";";
    }
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

void HandleStatement::bindDouble(const Float &value, int index)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    bool succeed = APIExit(sqlite3_bind_double(m_stmt, index, value));
    if (succeed && m_fullTrace) {
        m_stream << index << ":" << value << ";";
    }
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

void HandleStatement::bindText(const Text &value, int index)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    // use SQLITE_STATIC if auto_commit?
    bool succeed = APIExit(sqlite3_bind_text(
    m_stmt, index, value.data(), (int) value.length(), SQLITE_TRANSIENT));
    if (succeed && m_fullTrace) {
        m_stream << index << ":";
        if (value.length() < 20) {
            m_stream << value;
        } else {
            m_stream << UnsafeStringView(value.data(), 20) << "...";
        }
        m_stream << ";";
    }
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

void HandleStatement::bindText16(const char16_t *value, size_t valueLength, int index)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    bool succeed = APIExit(sqlite3_bind_text16(
    m_stmt, index, value, (int) valueLength * 2, SQLITE_TRANSIENT));
    if (succeed && m_fullTrace) {
        m_stream << index << ":";
        char buffer[81];
        m_stream << UnsafeStringView::createFromUTF16(
        value, valueLength < 20 ? valueLength : 20, buffer);
        if (valueLength > 20) {
            m_stream << "...";
        }
        m_stream << ";";
    }
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

void HandleStatement::bindBLOB(const BLOB &value, int index)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    // TODO: use SQLITE_STATIC to get better performance?
    bool succeed = APIExit(sqlite3_bind_blob(
    m_stmt, index, value.buffer(), (int) value.size(), SQLITE_TRANSIENT));
    if (succeed && m_fullTrace) {
        m_stream << index << ":";
        auto flags = m_stream.flags();
        m_stream << std::hex << std::setfill('0');
        size_t length = value.size() > 10 ? 10 : value.size();
        for (int i = 0; i < length; i++) {
            int c = value.buffer()[i];
            m_stream << std::setw(2) << c;
        }
        m_stream.flags(flags);
        if (value.size() > 10) {
            m_stream << "...";
        }
        m_stream << ";";
    }
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

void HandleStatement::bindNull(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    bool succeed = APIExit(sqlite3_bind_null(m_stmt, index));
    if (succeed && m_fullTrace) {
        m_stream << index << ":"
                 << ";";
    }
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

void HandleStatement::bindPointer(void *ptr, int index, const Text &type, void (*destructor)(void *))
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    bool succeed
    = APIExit(sqlite3_bind_pointer(m_stmt, index, ptr, type.data(), destructor));
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

int HandleStatement::bindParameterIndex(const Text &parameterName)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    int index = sqlite3_bind_parameter_index(m_stmt, parameterName.data());
    return index;
}

HandleStatement::Integer HandleStatement::getInteger(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(isBusy());
    return static_cast<Integer>(sqlite3_column_int64(m_stmt, index));
}

HandleStatement::Float HandleStatement::getDouble(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(isBusy());
    return static_cast<Float>(sqlite3_column_double(m_stmt, index));
}

HandleStatement::Text HandleStatement::getText(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(isBusy());
    return UnsafeStringView(
    reinterpret_cast<const char *>(sqlite3_column_text(m_stmt, index)),
    sqlite3_column_bytes(m_stmt, index));
}

const char16_t *HandleStatement::getText16(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(isBusy());
    return reinterpret_cast<const char16_t *>(sqlite3_column_text16(m_stmt, index));
}

size_t HandleStatement::getText16Length(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(isBusy());
    return sqlite3_column_bytes16(m_stmt, index) / 2;
}

const HandleStatement::BLOB HandleStatement::getBLOB(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(isBusy());
    return BLOB::immutable(
    reinterpret_cast<const unsigned char *>(sqlite3_column_blob(m_stmt, index)),
    sqlite3_column_bytes(m_stmt, index));
}

void HandleStatement::bindValue(const Value &value, int index)
{
    switch (value.getType()) {
    case ColumnType::Null:
        return bindNull(index);
    case ColumnType::Integer:
        return bindInteger(value, index);
    case ColumnType::Float:
        return bindDouble(value, index);
    case ColumnType::Text:
        return bindText(value, index);
    case ColumnType::BLOB:
        return bindBLOB(value, index);
    }
}

void HandleStatement::bindRow(const OneRowValue &row)
{
    for (int i = 1; i <= row.size(); i++) {
        bindValue(row[i - 1], i);
    }
}

Value HandleStatement::getValue(int index)
{
    switch (getType(index)) {
    case ColumnType::Null:
        return Value(nullptr);
    case ColumnType::Integer:
        return Value(getInteger(index));
    case ColumnType::Float:
        return Value(getDouble(index));
    case ColumnType::Text:
        return Value(getText(index));
    case ColumnType::BLOB:
        return Value(getBLOB(index));
    }
}

OptionalOneColumn HandleStatement::getOneColumn(int index)
{
    OptionalOneColumn result;
    bool succeed = false;
    while ((succeed = step()) && !done()) {
        if (!result.succeed()) {
            result = OneColumnValue();
        }
        result->push_back(getValue(index));
    }
    if (!succeed) {
        return NullOpt;
    }
    return !result.hasValue() ? OneColumnValue() : result;
}

OneRowValue HandleStatement::getOneRow()
{
    OneRowValue result;
    int count = getNumberOfColumns();
    for (int i = 0; i < count; i++) {
        result.push_back(getValue(i));
    }
    return result;
}

OptionalMultiRows HandleStatement::getAllRows()
{
    OptionalMultiRows result;
    bool succeed = false;
    while ((succeed = step()) && !done()) {
        if (!result.succeed()) {
            result = MultiRowsValue();
        }
        result->push_back(getOneRow());
    }
    if (!succeed) {
        return NullOpt;
    }
    return !result.hasValue() ? MultiRowsValue() : result;
}

signed long long HandleStatement::getColumnSize(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(isBusy());
    return sqlite3_column_bytes(m_stmt, index);
}

bool HandleStatement::isBusy()
{
    WCTAssert(isPrepared());
    return sqlite3_stmt_busy(m_stmt) != 0;
}

void HandleStatement::enableAutoAddColumn()
{
    m_needAutoAddColumn = true;
}

bool HandleStatement::isReadOnly()
{
    WCTAssert(isPrepared());
    return sqlite3_stmt_readonly(m_stmt) != 0;
}

bool HandleStatement::isPrepared()
{
    return m_stmt != nullptr;
}

#pragma mark - Full trace sql
void HandleStatement::tryReportSQL()
{
    if (!m_fullTrace || !m_needReport || !isPrepared()) {
        return;
    }
    UnsafeStringView sql = sqlite3_sql(m_stmt);
    if (sql.hasPrefix("SELECT")) {
        m_stream << "RowCount:" << m_stepCount;
    } else if (sql.hasPrefix("INSERT")) {
        m_stream << "LastInsertedId:" << getHandle()->getLastInsertedRowID();
    } else if (sql.hasPrefix("DELETE") || sql.hasPrefix("UPDATE")) {
        m_stream << "Changes:" << getHandle()->getChanges();
    }
    getHandle()->postSQLNotification(sql, m_stream.str());
    clearReport();
}

void HandleStatement::clearReport()
{
    m_stream.str("");
    m_stream.clear();
    m_needReport = false;
    m_stepCount = 0;
}

} //namespace WCDB
