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

namespace WCDB {

CompressHandleOperator::CompressHandleOperator(InnerHandle* handle)
: HandleOperator(handle)
, m_compressionRecordTableCreated(false)
, m_compressedCount(0)
, m_compressingTableInfo(nullptr)
, m_selectRowidStatement(handle->getStatement())
, m_selectValueStatement(handle->getStatement())
, m_updateValueStatement(handle->getStatement())
, m_updateRecordStatement(handle->getStatement())
{
}

CompressHandleOperator::~CompressHandleOperator()
{
    finalizeCompressionStatements();
    m_updateRecordStatement->finalize();
    getHandle()->returnStatement(m_selectRowidStatement);
    getHandle()->returnStatement(m_selectValueStatement);
    getHandle()->returnStatement(m_updateValueStatement);
    getHandle()->returnStatement(m_updateRecordStatement);
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
    if (!m_compressionRecordTableCreated) {
        auto tableExist = handle->tableExists(CompressionRecord::tableName);
        if (tableExist.failed()) {
            return false;
        }
        if (!tableExist.value()) {
            for (auto compressionTableInfo : allTableInfos) {
                compressionTableInfo->setMinCompressedRowid(INT64_MAX);
            }
            return true;
        }
    }
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
    for (auto row : allRecords.value()) {
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
        for (auto& compressingColumn : (*iter)->getColumnInfos()) {
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
            (*iter)->setMinCompressedRowid(INT64_MAX);
            iter++;
            continue;
        }
        if (recordIter->second.second <= 0) {
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
    InnerHandle* handle = getHandle();
    WCTAssert(handle != nullptr);
    auto exists = handle->tableExists(info->getTable());
    if (!exists.succeed()) {
        return NullOpt;
    }
    if (!exists.value()) {
        handle->execute(StatementDelete()
                        .deleteFrom(CompressionRecord::tableName)
                        .where(Column(CompressionRecord::columnTable) == info->getTable()));
        return true;
    }
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
    int compressCount = 0;
    if (!handle->runTransaction([&rowids, &compressCount, this](InnerHandle* handle) {
            for (const auto& rowid : rowids.value()) {
                if (!compressRow(rowid)) {
                    return false;
                }
                compressCount++;
                if (handle->checkHasBusyRetry()) {
                    break;
                }
            }
            return true;
        })) {
        resetCompressionStatements();
        return NullOpt;
    }
    bool compressed = false;
    if (compressCount == rowids.value().size() && rowids.value().size() < CompressionBatchCount) {
        m_compressingTableInfo->setMinCompressedRowid(0);
        compressed = true;
    } else {
        m_compressingTableInfo->setMinCompressedRowid(rowids.value().at(compressCount - 1));
    }
    m_compressedCount += compressCount;
    if (m_compressedCount >= CompressionUpdateRecordBatchCount || compressed) {
        updateCompressionRecord();
    }
    resetCompressionStatements();
    return compressed;
}

bool CompressHandleOperator::compressRow(int64_t rowid)
{
    WCTAssert(m_selectValueStatement->isPrepared() && m_updateValueStatement->isPrepared());
    WCTAssert(getHandle()->isInTransaction());
    WCTAssert(m_selectValueStatement->getNumberOfColumns() >= 2);
    WCTAssert(rowid < m_compressingTableInfo->getMinCompressedRowid());
    return m_compressingTableInfo->stepSelectAndUpdateUncompressRowStatement(
    m_selectValueStatement, m_updateValueStatement, rowid);
}

bool CompressHandleOperator::prepareCompressionStatements()
{
    if (!m_selectRowidStatement->isPrepared()
        && !m_selectRowidStatement->prepare(
        m_compressingTableInfo->getSelectUncompressRowIdStatement())) {
        return false;
    }
    if (!m_selectValueStatement->isPrepared()
        && !m_selectValueStatement->prepare(
        m_compressingTableInfo->getSelectUncompressRowStatement())) {
        return false;
    }
    if (!m_updateValueStatement->isPrepared()
        && !m_updateValueStatement->prepare(
        m_compressingTableInfo->getUpdateUncompressRowStatement())) {
        return false;
    }
    return true;
}

void CompressHandleOperator::resetCompressionStatements()
{
    m_selectRowidStatement->reset();
    m_selectValueStatement->reset();
    m_updateValueStatement->reset();
}

void CompressHandleOperator::finalizeCompressionStatements()
{
    m_selectRowidStatement->finalize();
    m_selectValueStatement->finalize();
    m_updateValueStatement->finalize();
}

bool CompressHandleOperator::updateCompressionRecord()
{
    InnerHandle* handle = getHandle();
    if (!m_compressionRecordTableCreated) {
        auto exists = handle->tableExists(CompressionRecord::tableName);
        if (exists.failed()) {
            return false;
        }
        if (!exists.value()
            && !handle->execute(CompressionRecord::getCreateTableStatement())) {
            return false;
        }
        m_compressionRecordTableCreated = true;
    }
    if (!m_updateRecordStatement->isPrepared()
        && !m_updateRecordStatement->prepare(CompressionRecord::getInsertValueStatement())) {
        return false;
    }

    size_t columnSize = 0;
    for (const auto& column : m_compressingTableInfo->getColumnInfos()) {
        columnSize += column.getColumn().syntax().name.size() + 1;
    }
    char* columns = (char*) malloc(columnSize);
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

InnerHandle* CompressHandleOperator::getCurrentHandle() const
{
    return getHandle();
}

} //namespace WCDB
