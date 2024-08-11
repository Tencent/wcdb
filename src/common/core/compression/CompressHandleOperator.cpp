//
// Created by qiuwenchen on 2023/11/26.
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

#include "CompressHandleOperator.hpp"
#include "Assertion.hpp"
#include "CompressionCenter.hpp"
#include "CompressionConst.hpp"
#include "CompressionRecord.hpp"
#include "CoreConst.h"
#include "Notifier.hpp"
#include "Time.hpp"
#include <stdlib.h>
#include <string.h>

namespace WCDB {

CompressHandleOperator::CompressHandleOperator(InnerHandle* handle)
: HandleOperator(handle)
, m_compressedCount(0)
, m_compressingTableInfo(nullptr)
, m_insertParameterCount(0)
, m_selectRowidStatement(handle->getStatement(DecoratorAllType))
, m_selectRowStatement(handle->getStatement(DecoratorAllType))
, m_deleteRowStatement(handle->getStatement(DecoratorAllType))
, m_insertNewRowStatement(handle->getStatement(DecoratorAllType))
, m_updateRecordStatement(handle->getStatement(DecoratorAllType))
{
}

CompressHandleOperator::~CompressHandleOperator()
{
    finalizeCompressionStatements();
    m_updateRecordStatement->finalize();
    InnerHandle* handle = getHandle();
    WCTAssert(handle != nullptr);
    handle->returnStatement(m_selectRowidStatement);
    handle->returnStatement(m_selectRowStatement);
    handle->returnStatement(m_deleteRowStatement);
    handle->returnStatement(m_insertNewRowStatement);
    handle->returnStatement(m_updateRecordStatement);
}

#pragma mark - Stepper
Optional<StringViewSet> CompressHandleOperator::getAllTables()
{
    Column name("name");
    Column type("type");
    StringView sqlitePattern
    = StringView::formatted("%s%%", Syntax::builtinTablePrefix.data());
    StringView wcdbPattern
    = StringView::formatted("%s%%", Syntax::builtinWCDBTablePrefix.data());
    return getHandle()->getValues(
    StatementSelect()
    .select(name)
    .from(TableOrSubquery::master())
    .where(type == "table" && name.notLike(sqlitePattern) && name.notLike(wcdbPattern)),
    0);
}

bool CompressHandleOperator::filterComplessingTables(std::set<const CompressionTableInfo*>& allTableInfos)
{
    InnerHandle* handle = getHandle();
    WCTAssert(handle != nullptr);
    StatementSelect select
    = StatementSelect().select(Column::all()).from(CompressionRecord::tableName);
    if (!handle->prepare(select)) {
        return false;
    }
    auto allRecords = handle->getAllRows();
    handle->finalize();
    if (allRecords.failed()) {
        return false;
    }
    if (allRecords.value().size() == 0) {
        return true;
    }
    if (allRecords.value().front().size() != 3) {
        StringView msg = StringView::formatted(
        "Invalid compression record size: %llu", allRecords.value().size());
        handle->notifyError(Error::Code::Error, nullptr, msg);
        return false;
    }
    StringViewMap<std::pair<StringView, int64_t>> allRecordsMap;
    for (auto& row : allRecords.value()) {
        allRecordsMap.emplace(row[0].textValue(),
                              { row[1].textValue(), row[2].intValue() });
    }
    for (auto iter = allTableInfos.begin(); iter != allTableInfos.end();) {
        auto recordIter = allRecordsMap.find((*iter)->getTable());
        if (recordIter == allRecordsMap.end()) {
            iter++;
            continue;
        }
        const StringView& compression = recordIter->second.first;
        if (!(*iter)->shouldReplaceCompression()) {
            auto compressedColumns = parseColumns(compression);
            bool columnMatched = true;
            for (const auto& compressingColumn : (*iter)->getColumnInfos()) {
                const StringView& columnName
                = compressingColumn.getColumn().syntax().name;
                if (compressedColumns.find(columnName) == compressedColumns.end()) {
                    columnMatched = false;
                    break;
                }
            }
            if (!columnMatched) {
                iter++;
                continue;
            }
        } else {
            StringView curCompression = (*iter)->getCompressionDescription();
            if (!curCompression.equal(compression)) {
                iter++;
                continue;
            }
        }
        if (recordIter->second.second <= 0) {
            (*iter)->setMinCompressedRowid(0);
            iter = allTableInfos.erase(iter);
        } else {
            (*iter)->setMinCompressedRowid(recordIter->second.second);
            iter++;
        }
    }
    return true;
}

std::set<StringView> CompressHandleOperator::parseColumns(const StringView& compressRecord)
{
    std::set<StringView> columnNames;
    size_t start = 0;
    size_t end = compressRecord.find(",");

    while (end != UnsafeStringView::npos) {
        UnsafeStringView subInput = compressRecord.subStr(start, end - start);
        size_t colonPos = subInput.find(":");
        if (colonPos != UnsafeStringView::npos) {
            columnNames.insert(subInput.subStr(0, colonPos - 1));
        } else {
            columnNames.insert(StringView(subInput));
        }

        start = end + 1;
        end = compressRecord.find(",", start);
    }

    UnsafeStringView subInput = compressRecord.subStr(start);
    size_t colonPos = subInput.find(":");
    if (colonPos != UnsafeStringView::npos) {
        columnNames.insert(subInput.subStr(0, colonPos - 1));
    } else {
        columnNames.insert(StringView(subInput));
    }

    return columnNames;
}

Optional<bool> CompressHandleOperator::compressRows(const CompressionTableInfo* info)
{
    WCTAssert(info != nullptr);
    int64_t start = Time::currentThreadCPUTimeInMicroseconds();
    if (m_compressingTableInfo != info) {
        finalizeCompressionStatements();
        m_compressedCount = 0;
        m_compressingTableInfo = info;
    }
    if (!prepareCompressionStatements()) {
        return NullOpt;
    }
    m_selectRowidStatement->bindInteger(m_compressingTableInfo->getMinCompressedRowid());
    auto rowids = m_selectRowidStatement->getOneColumn();
    if (rowids.failed()) {
        m_selectRowidStatement->reset();
        return NullOpt;
    }
    auto compressed = doCompressRows(rowids.value());
    if (compressed.failed()) {
        return NullOpt;
    }
    if (!compressed.value()) {
        return false;
    }

    bool compressionFinish = false;
    if (rowids.value().size() < CompressionBatchCount) {
        m_compressingTableInfo->setMinCompressedRowid(0);
        compressionFinish = true;
    } else {
        m_compressingTableInfo->setMinCompressedRowid(rowids.value().back());
    }
    m_compressedCount += rowids.value().size();
    if (m_compressedCount >= CompressionUpdateRecordBatchCount || compressionFinish) {
        updateCompressionRecord();
    }

    m_performance.compressTime += Time::currentThreadCPUTimeInMicroseconds() - start;
    if (m_performance.compressedCount + m_performance.uncompressedCount > CompressionUpdateRecordBatchCount
        || (compressionFinish
            && m_performance.compressedCount + m_performance.uncompressedCount > 0)) {
        reportPerformance(info->getTable());
    }

    return compressionFinish;
}

Optional<bool> CompressHandleOperator::doCompressRows(const OneColumnValue& rowids)
{
    bool interrupted = false;
    bool ret = getHandle()->runTransaction([&](InnerHandle* handle) {
        MultiRowsValue rows;
        for (const auto& rowid : rowids) {
            m_selectRowStatement->reset();
            m_selectRowStatement->bindInteger(rowid);
            if (!m_selectRowStatement->step()) {
                return false;
            }
            if (m_selectRowStatement->done()) {
                continue;
            }
            auto row = m_selectRowStatement->getOneRow();

            if (!compressRow(row)) {
                return false;
            }

            rows.push_back(std::move(row));

            m_deleteRowStatement->reset();
            m_deleteRowStatement->bindInteger(rowid);
            if (!m_deleteRowStatement->step()) {
                return false;
            }
            if (handle->checkHasBusyRetry()) {
                interrupted = true;
                handle->notifyError(Error::Code::Notice, "", "Interrupt compression due to busy");
                return false;
            }
        }
        for (const auto& row : rows) {
            if (handle->checkHasBusyRetry()) {
                interrupted = true;
                handle->notifyError(Error::Code::Notice, "", "Interrupt compression due to busy");
                return false;
            }
            if (!m_insertNewRowStatement->isPrepared() || row.size() != m_insertParameterCount) {
                m_insertNewRowStatement->finalize();
                m_insertParameterCount = row.size();
                if (!m_insertNewRowStatement->prepare(m_compressingTableInfo->getInsertNewRowStatement(
                    m_insertParameterCount))) {
                    return false;
                }
            }
            m_insertNewRowStatement->reset();
            m_insertNewRowStatement->bindRow(row);
            if (!m_insertNewRowStatement->step()) {
                return false;
            }
        }
        return true;
    });
    resetCompressionStatements();
    if (!ret && !interrupted) {
        return NullOpt;
    }
    return !interrupted;
}

bool CompressHandleOperator::compressRow(OneRowValue& row)
{
    for (const auto& column : m_compressingTableInfo->getColumnInfos()) {
        if (column.getColumnIndex() >= row.size()) {
            getHandle()->notifyError(
            Error::Code::Error,
            nullptr,
            StringView::formatted("Compressing column %s with index index %u out of range",
                                  column.getColumn().syntax().name.data(),
                                  column.getColumnIndex()));
            return false;
        }
        Value& value = row[column.getColumnIndex()];
        ColumnType valueType = value.getType();

        if (column.getTypeColumnIndex() >= row.size()) {
            getHandle()->notifyError(
            Error::Code::Error,
            nullptr,
            StringView::formatted("Compressing type column %s with index index %u out of range",
                                  column.getTypeColumn().syntax().name.data(),
                                  column.getTypeColumnIndex()));
            return false;
        }

        Value& compressedType = row[column.getTypeColumnIndex()];
        if (valueType < ColumnType::Text) {
            compressedType = CompressedType::None;
            continue;
        }
        UnsafeData data;
        if (valueType == ColumnType::Text) {
            const StringView& text = value.textValue();
            data = UnsafeData((unsigned char*) text.data(), text.length());
        } else {
            data = value.blobValue();
        }
        CompressedType originCompressionType
        = WCDBGetCompressedType(compressedType.intValue());
        if (originCompressionType != CompressedType::None) {
            if (!m_compressingTableInfo->shouldReplaceCompression()) {
                continue;
            }
            auto decompressed = CompressionCenter::shared().decompressContent(
            data, originCompressionType == CompressedType::ZSTDDict, getHandle());
            if (!decompressed.hasValue()) {
                return false;
            }
            valueType = WCDBGetOriginType(compressedType.intValue());
            if (valueType == ColumnType::Text) {
                value = StringView((const char*) decompressed.value().buffer(),
                                   decompressed.value().size());
                const StringView& text = value.textValue();
                data = UnsafeData((unsigned char*) text.data(), text.length());
            } else {
                value = decompressed.value();
                data = value.blobValue();
            }
        }

        CompressedType toCompressedType = CompressedType::ZSTDDict;
        Optional<UnsafeData> compressedValue;

        switch (column.getCompressionType()) {
        case CompressionType::Normal: {
            toCompressedType = CompressedType::ZSTDNormal;
            compressedValue
            = CompressionCenter::shared().compressContent(data, 0, getHandle());
        } break;
        case CompressionType::Dict: {
            compressedValue = CompressionCenter::shared().compressContent(
            data, column.getDictId(), getHandle());
        } break;
        case CompressionType::VariousDict: {
            if (column.getMatchColumnIndex() >= row.size()) {
                getHandle()->notifyError(
                Error::Code::Error,
                nullptr,
                StringView::formatted("Compressing match column %s with index index %u out of range",
                                      column.getMatchColumn().syntax().name.data(),
                                      column.getMatchColumnIndex()));
                return false;
            }
            Value& matchValue = row[column.getMatchColumnIndex()];
            compressedValue = CompressionCenter::shared().compressContent(
            data, column.getMatchDictId(matchValue), getHandle());
        } break;
        }

        if (compressedValue.failed()) {
            return false;
        }
        WCTAssert(compressedValue.value().size() <= data.size());

        m_performance.totalSize += data.size();
        if (compressedValue.value().size() < data.size()) {
            value = compressedValue.value();
            if (!CompressionCenter::shared().testContentCanBeDecompressed(
                value.blobValue(), toCompressedType == CompressedType::ZSTDDict, getHandle())) {
                return false;
            }
            compressedType = WCDBMergeCompressionType(toCompressedType, valueType);

            m_performance.compressedCount++;
            m_performance.compressedSize += compressedValue.value().size();
            m_performance.originalSize += data.size();
        } else {
            m_performance.uncompressedCount++;
            compressedType = WCDBMergeCompressionType(CompressedType::None, valueType);
        }
    }
    return true;
}

bool CompressHandleOperator::prepareCompressionStatements()
{
    if (!m_selectRowidStatement->isPrepared()
        && !m_selectRowidStatement->prepare(
        m_compressingTableInfo->getSelectNeedCompressRowIdStatement())) {
        return false;
    }
    if (!m_selectRowStatement->isPrepared()
        && !m_selectRowStatement->prepare(m_compressingTableInfo->getSelectRowStatement())) {
        return false;
    }
    if (!m_deleteRowStatement->isPrepared()
        && !m_deleteRowStatement->prepare(m_compressingTableInfo->getDelectRowStatement())) {
        return false;
    }
    return true;
}

void CompressHandleOperator::resetCompressionStatements()
{
    if (m_selectRowidStatement->isPrepared()) {
        m_selectRowidStatement->reset();
    }
    if (m_selectRowStatement->isPrepared()) {
        m_selectRowStatement->reset();
    }
    if (m_deleteRowStatement->isPrepared()) {
        m_deleteRowStatement->reset();
    }
    if (m_insertNewRowStatement->isPrepared()) {
        m_insertNewRowStatement->reset();
    }
}

void CompressHandleOperator::finalizeCompressionStatements()
{
    m_insertParameterCount = 0;
    m_selectRowidStatement->finalize();
    m_selectRowStatement->finalize();
    m_deleteRowStatement->finalize();
    m_insertNewRowStatement->finalize();
}

bool CompressHandleOperator::updateCompressionRecord()
{
    if (!m_updateRecordStatement->isPrepared()
        && !m_updateRecordStatement->prepare(CompressionRecord::getInsertValueStatement())) {
        return false;
    }

    m_updateRecordStatement->bindText(m_compressingTableInfo->getTable(), 1);
    m_updateRecordStatement->bindText(
    m_compressingTableInfo->getCompressionDescription(), 2);
    m_updateRecordStatement->bindInteger(
    m_compressingTableInfo->getMinCompressedRowid(), 3);
    bool ret = m_updateRecordStatement->step();
    m_updateRecordStatement->reset();
    return ret;
}

void CompressHandleOperator::reportPerformance(const UnsafeStringView& table)
{
    Error error(Error::Code::Notice, Error::Level::Notice, "Compression performance");
    error.infos.insert_or_assign(ErrorStringKeyPath, getHandle()->getPath());
    error.infos.insert_or_assign(ErrorIntKeyTag, (long) getHandle()->getTag());
    error.infos.insert_or_assign("CompressTime", m_performance.compressTime);
    error.infos.insert_or_assign("CompressedCount", m_performance.compressedCount);
    error.infos.insert_or_assign("UncompressedCount", m_performance.uncompressedCount);
    error.infos.insert_or_assign("OriginalSize", m_performance.originalSize);
    error.infos.insert_or_assign("CompressedSize", m_performance.compressedSize);
    error.infos.insert_or_assign("TotalSize", m_performance.totalSize);
    error.infos.insert_or_assign("Table", table);

    Notifier::shared().notify(error);

    m_performance = CompressionPerformance();
}

InnerHandle* CompressHandleOperator::getCurrentHandle() const
{
    return getHandle();
}

bool CompressHandleOperator::rollbackCompression(const CompressionTableInfo* info)
{
    clearProgress();
    auto compressedColumns = getCompressedColumns(info);
    if (compressedColumns.failed()) {
        return false;
    }
    if (compressedColumns.value().size() == 0) {
        finishProgress();
        return true;
    }

    int64_t maxRowId = 0;
    int64_t curMaxRowid = std::numeric_limits<int64_t>::max();
    bool succeed = true;
    while (curMaxRowid > 0) {
        auto nextMaxRowid = batchRollbackCompression(
        info, compressedColumns.value(), maxRowId, curMaxRowid);
        succeed = nextMaxRowid.succeed() && getHandle()->checkpoint();
        if (!succeed) {
            return false;
        }
        curMaxRowid = nextMaxRowid.value();
    }
    if (succeed) {
        succeed
        = execute(CompressionRecord::getDeleteRecordStatement(info->getTable()));
        succeed = succeed && getHandle()->checkpoint();
    }
    if (succeed) {
        finishProgress();
    }
    return succeed;
}

Optional<int64_t> CompressHandleOperator::batchRollbackCompression(
const CompressionTableInfo* info,
const std::list<const CompressionColumnInfo*>& compressedColumns,
int64_t& maxRowId,
int64_t curRowId)
{
    bool ret = getHandle()->runTransaction([&](InnerHandle*) {
        HandleStatement* selectRowid = getHandle()->getStatement(DecoratorAllType);
        HandleStatement* selectCompressedColumn
        = getHandle()->getStatement(DecoratorAllType);
        HandleStatement* updateCompressedColumn
        = getHandle()->getStatement(DecoratorAllType);

        bool succeed
        = selectRowid->prepare(info->getSelectCompressedRowIdStatement(curRowId));
        succeed = succeed
                  && selectCompressedColumn->prepare(
                  info->getSelectCompressedRowStatement(&compressedColumns));
        succeed = succeed
                  && updateCompressedColumn->prepare(
                  info->getUpdateCompressColumnStatement(&compressedColumns));

        int updatedRowCount = 0;
        if (succeed) {
            while ((succeed = selectRowid->step()) && !selectRowid->done()) {
                curRowId = selectRowid->getInteger();
                if (maxRowId == 0) {
                    maxRowId = curRowId;
                }

                succeed = info->stepSelectAndUpdateCompressedRowStatement(
                selectCompressedColumn, updateCompressedColumn, curRowId);
                if (!succeed) {
                    break;
                }

                if (maxRowId > 0 && !updateProgress(((double) maxRowId - curRowId) / maxRowId)) {
                    succeed = false;
                    break;
                }

                if (++updatedRowCount > 10 * CompressionBatchCount) {
                    break;
                }
            }
            if (selectRowid->done()) {
                curRowId = 0;
            }
        }

        selectRowid->finalize();
        selectCompressedColumn->finalize();
        updateCompressedColumn->finalize();
        getHandle()->returnStatement(selectRowid);
        getHandle()->returnStatement(selectCompressedColumn);
        getHandle()->returnStatement(updateCompressedColumn);
        return succeed;
    });
    if (ret) {
        return curRowId;
    } else {
        return NullOpt;
    }
}

bool CompressHandleOperator::deleteCompressionRecord()
{
    return execute(CompressionRecord::getDropTableStatement());
}

bool CompressHandleOperator::execute(const Statement& statement)
{
    HandleStatement* handleStatement = getHandle()->getStatement(DecoratorAllType);
    bool succeed = handleStatement->prepare(statement) && handleStatement->step();
    handleStatement->finalize();
    getHandle()->returnStatement(handleStatement);
    return succeed;
}

Optional<std::list<const CompressionColumnInfo*>>
CompressHandleOperator::getCompressedColumns(const CompressionTableInfo* info)
{
    auto optionalMetas = getHandle()->getTableMeta(Schema::main(), info->getTable());
    if (!optionalMetas.succeed()) {
        return NullOpt;
    }
    auto& metas = optionalMetas.value();
    std::list<StringView> curColumns;
    for (const auto& meta : metas) {
        curColumns.push_back(meta.name);
    }

    std::list<const CompressionColumnInfo*> compressedColumns;
    for (auto& compressingColumn : info->getColumnInfos()) {
        for (const auto& column : curColumns) {
            if (column.equal(compressingColumn.getTypeColumn().syntax().name)) {
                compressedColumns.push_back(&compressingColumn);
            }
        }
    }
    return compressedColumns;
}

} //namespace WCDB
