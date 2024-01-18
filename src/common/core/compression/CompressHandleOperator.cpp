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
        const StringView& columns = recordIter->second.first;
        bool columnMatched = true;
        for (const auto& compressingColumn : (*iter)->getColumnInfos()) {
            const StringView& columnName
            = compressingColumn.getColumn().syntax().name;
            size_t pos = columns.find(columnName);
            if (pos == UnsafeStringView::npos
                || (pos > 0 && columns.at(pos - 1) != CompressionRecordColumnSeperater)
                || (pos + columnName.size() < columns.size()
                    && columns.at(pos + columnName.size()) != CompressionRecordColumnSeperater)) {
                columnMatched = false;
                break;
            }
        }
        if (!columnMatched) {
            iter++;
            continue;
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
    tryReportPerformance();

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
        if (!compressedType.isNull()) {
            continue;
        }

        CompressedType toCompressedType = CompressedType::ZSTDDict;
        Optional<UnsafeData> compressedValue;
        UnsafeData data;
        if (valueType == ColumnType::Text) {
            const StringView& text = value.textValue();
            data = UnsafeData((unsigned char*) text.data(), text.length());
        } else {
            data = value.blobValue();
        }

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
        m_compressingTableInfo->getSelectUncompressRowIdStatement())) {
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

    size_t columnSize = 0;
    for (const auto& column : m_compressingTableInfo->getColumnInfos()) {
        columnSize += column.getColumn().syntax().name.size() + 1;
    }
    char* columns = (char*) malloc(columnSize + 1);
    if (columns == nullptr) {
        getHandle()->notifyError(Error::Code::NoMemory, nullptr, "Alloc compressed columns fail");
        return false;
    }
    int curIndex = 0;
    for (const auto& column : m_compressingTableInfo->getColumnInfos()) {
        const StringView& columnName = column.getColumn().syntax().name;
        memcpy(columns + curIndex, columnName.data(), columnName.size());
        curIndex += columnName.size();
        columns[curIndex] = CompressionRecordColumnSeperater;
        curIndex++;
    }
    columns[curIndex] = '\0';

    m_updateRecordStatement->bindText(m_compressingTableInfo->getTable(), 1);
    m_updateRecordStatement->bindText(
    UnsafeStringView(columns, curIndex > 0 ? curIndex - 1 : 0), 2);
    m_updateRecordStatement->bindInteger(
    m_compressingTableInfo->getMinCompressedRowid(), 3);
    bool ret = m_updateRecordStatement->step();
    free(columns);
    m_updateRecordStatement->reset();
    return ret;
}

void CompressHandleOperator::tryReportPerformance()
{
    if (m_performance.compressedCount + m_performance.uncompressedCount
        < CompressionUpdateRecordBatchCount) {
        return;
    }

    Error error(Error::Code::Notice, Error::Level::Notice, "Compression performance");
    error.infos.insert_or_assign(ErrorStringKeyPath, getHandle()->getPath());
    error.infos.insert_or_assign(ErrorIntKeyTag, (long) getHandle()->getTag());
    error.infos.insert_or_assign("CompressTime", m_performance.compressTime);
    error.infos.insert_or_assign("CompressedCount", m_performance.compressedCount);
    error.infos.insert_or_assign("UncompressedCount", m_performance.uncompressedCount);
    error.infos.insert_or_assign("OriginalSize", m_performance.originalSize);
    error.infos.insert_or_assign("CompressedSize", m_performance.compressedSize);
    error.infos.insert_or_assign("TotalSize", m_performance.totalSize);

    Notifier::shared().notify(error);

    m_performance = CompressionPerformance();
}

InnerHandle* CompressHandleOperator::getCurrentHandle() const
{
    return getHandle();
}

} //namespace WCDB
