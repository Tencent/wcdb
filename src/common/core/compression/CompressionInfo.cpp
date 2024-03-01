//
// Created by qiuwenchen on 2023/11/21.
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

#include "CompressionInfo.hpp"
#include "Assertion.hpp"
#include "CompressionCenter.hpp"
#include "CompressionConst.hpp"
#include "CoreConst.h"
#include "HandleStatement.hpp"
#include "InnerHandle.hpp"
#include "WINQ.h"

namespace WCDB {

#pragma mark - CompressionColumnInfo
CompressionColumnInfo::CompressionColumnInfo(const Column &column, CompressionType type)
: m_column(column)
, m_columnIndex(UINT16_MAX)
, m_typeColumnIndex(UINT16_MAX)
, m_matchColumnIndex(UINT16_MAX)
, m_compressionType(type)
, m_commonDictID(-1)
{
    std::ostringstream stringStream;
    stringStream << CompressionColumnTypePrefix << column.syntax().name;
    m_typeColumn = Column(StringView::createConstant(
    stringStream.str().data(), stringStream.str().length()));
}

CompressionColumnInfo::CompressionColumnInfo(const Column &column, const Column &matchColumn)
: m_column(column)
, m_columnIndex(UINT16_MAX)
, m_typeColumnIndex(UINT16_MAX)
, m_matchColumn(matchColumn)
, m_matchColumnIndex(UINT16_MAX)
, m_compressionType(CompressionType::VariousDict)
, m_commonDictID(-1)
{
    std::ostringstream stringStream;
    stringStream << CompressionColumnTypePrefix << column.syntax().name;
    m_typeColumn = Column(StringView::createConstant(
    stringStream.str().data(), stringStream.str().length()));
}

CompressionColumnInfo::CompressionColumnInfo(const CompressionColumnInfo &other)
: m_column(other.m_column)
, m_columnIndex(other.m_columnIndex.load())
, m_typeColumn(other.m_typeColumn)
, m_typeColumnIndex(other.m_typeColumnIndex.load())
, m_matchColumn(other.m_matchColumn)
, m_matchColumnIndex(other.m_matchColumnIndex.load())
, m_compressionType(other.m_compressionType)
, m_commonDictID(other.m_commonDictID)
, m_matchDicts(other.m_matchDicts)
{
}

CompressionColumnInfo::CompressionColumnInfo(CompressionColumnInfo &&other)
: m_column(std::move(other.m_column))
, m_columnIndex(other.m_columnIndex.load())
, m_typeColumn(std::move(other.m_typeColumn))
, m_typeColumnIndex(other.m_typeColumnIndex.load())
, m_matchColumn(std::move(other.m_matchColumn))
, m_matchColumnIndex(other.m_matchColumnIndex.load())
, m_compressionType(other.m_compressionType)
, m_commonDictID(other.m_commonDictID)
, m_matchDicts(std::move(other.m_matchDicts))
{
}

const Column &CompressionColumnInfo::getColumn() const
{
    return m_column;
}

void CompressionColumnInfo::setColumnIndex(uint16_t index) const
{
    m_columnIndex = index;
}

uint16_t CompressionColumnInfo::getColumnIndex() const
{
    WCTAssert(m_columnIndex != UINT16_MAX);
    return m_columnIndex;
}

const Column &CompressionColumnInfo::getTypeColumn() const
{
    return m_typeColumn;
}

void CompressionColumnInfo::setTypeColumnIndex(uint16_t index) const
{
    m_typeColumnIndex = index;
}

uint16_t CompressionColumnInfo::getTypeColumnIndex() const
{
    WCTAssert(m_typeColumnIndex != UINT16_MAX);
    return m_typeColumnIndex;
}

const Column &CompressionColumnInfo::getMatchColumn() const
{
    return m_matchColumn;
}

void CompressionColumnInfo::setMatchColumnIndex(uint16_t index) const
{
    m_matchColumnIndex = index;
}

uint16_t CompressionColumnInfo::getMatchColumnIndex() const
{
    WCTAssert(m_matchColumnIndex != UINT16_MAX);
    return m_matchColumnIndex;
}

CompressionType CompressionColumnInfo::getCompressionType() const
{
    return m_compressionType;
}

CompressionColumnInfo::DictId CompressionColumnInfo::getDictId() const
{
    WCTAssert(m_compressionType == CompressionType::Dict);
    return m_commonDictID;
}

CompressionColumnInfo::DictId
CompressionColumnInfo::getMatchDictId(const Integer &matchValue) const
{
    WCTAssert(m_compressionType == CompressionType::VariousDict);
    auto iter = m_matchDicts.find(matchValue);
    if (iter == m_matchDicts.end()) {
        return m_commonDictID;
    }
    return iter->second;
}

void CompressionColumnInfo::setCommonDict(DictId dictId)
{
    WCTAssert(m_compressionType != CompressionType::Normal);
    m_commonDictID = dictId;
}

void CompressionColumnInfo::addMatchDict(const Integer &matchValue, DictId dictId)
{
    WCTAssert(m_compressionType == CompressionType::VariousDict);
    m_matchDicts[matchValue] = dictId;
}

#pragma mark - CompressionTableBaseInfo
CompressionTableBaseInfo::CompressionTableBaseInfo(const UnsafeStringView &table)
: m_table(table)
{
}

bool CompressionTableBaseInfo::shouldCompress() const
{
    return m_compressingColumns.size() > 0;
}

const StringView &CompressionTableBaseInfo::getTable() const
{
    return m_table;
}

#pragma mark - CompressionTableUserInfo

CompressionTableUserInfo::CompressionTableUserInfo(const UnsafeStringView &table)
: CompressionTableBaseInfo(table)
{
}

void CompressionTableUserInfo::addCompressingColumn(const CompressionColumnInfo &info)
{
    for (auto iter = m_compressingColumns.begin(); iter != m_compressingColumns.end(); iter++) {
        if (iter->getColumn().syntax().name.equal(info.getColumn().syntax().name)) {
            m_compressingColumns.erase(iter);
            break;
        }
    }
    m_compressingColumns.push_back(info);
}

#pragma mark - CompressionTableInfo
CompressionTableInfo::CompressionTableInfo(const CompressionTableUserInfo &userInfo)
: CompressionTableBaseInfo(userInfo)
, m_minCompressedRowid(INT64_MAX)
, m_needCheckColumn(true)
{
}

CompressionTableInfo::ColumnInfoList &CompressionTableInfo::getColumnInfos() const
{
    return m_compressingColumns;
}

void CompressionTableInfo::setMinCompressedRowid(int64_t rowid) const
{
    m_minCompressedRowid = rowid;
}

int64_t CompressionTableInfo::getMinCompressedRowid() const
{
    return m_minCompressedRowid;
}

bool CompressionTableInfo::needCheckColumns() const
{
    return m_needCheckColumn;
}

void CompressionTableInfo::setNeedCheckColumns(bool needCheck) const
{
    m_needCheckColumn = needCheck;
}

#pragma mark - Compress Statements

StatementSelect CompressionTableInfo::getSelectUncompressRowIdStatement() const
{
    Expression condition;
    for (auto &column : m_compressingColumns) {
        if (condition.syntax().isValid()) {
            condition = condition || column.getTypeColumn().isNull();
        } else {
            condition = column.getTypeColumn().isNull();
        }
    }
    return StatementSelect()
    .select(Column::rowid())
    .from(m_table)
    .where(condition && Column::rowid() < BindParameter())
    .order(Column::rowid().asOrder(Order::DESC))
    .limit(CompressionBatchCount);
}

StatementSelect CompressionTableInfo::getSelectRowStatement() const
{
    return StatementSelect().select(Column::all()).from(m_table).where(Column::rowid() == BindParameter());
}

StatementDelete CompressionTableInfo::getDelectRowStatement() const
{
    return StatementDelete().deleteFrom(m_table).where(Column::rowid() == BindParameter());
}

StatementInsert CompressionTableInfo::getInsertNewRowStatement(size_t valueCount) const
{
    return StatementInsert().insertIntoTable(m_table).values(
    BindParameter::bindParameters(valueCount));
}

StatementSelect
CompressionTableInfo::getSelectUncompressRowStatement(ColumnInfoPtrList *columnList) const
{
    ResultColumns resultColumns;
    ColumnInfoIter columnIter(&m_compressingColumns, columnList);
    const CompressionColumnInfo *column = nullptr;
    while ((column = columnIter.nextInfo()) != nullptr) {
        resultColumns.push_back(column->getColumn());
        resultColumns.push_back(column->getTypeColumn());
        if (column->getCompressionType() == CompressionType::VariousDict) {
            resultColumns.push_back(column->getMatchColumn());
        }
    }
    return StatementSelect().select(resultColumns).from(m_table).where(Column::rowid() == BindParameter());
}

StatementUpdate
CompressionTableInfo::getUpdateUncompressRowStatement(ColumnInfoPtrList *columnList) const
{
    StatementUpdate update
    = StatementUpdate().update(m_table).where(Column::rowid() == BindParameter(1));
    int index = 2;
    ColumnInfoIter columnIter(&getColumnInfos(), columnList);
    const CompressionColumnInfo *column = nullptr;
    while ((column = columnIter.nextInfo()) != nullptr) {
        update.set(column->getColumn()).to(BindParameter(index++));
        update.set(column->getTypeColumn()).to(BindParameter(index++));
    }
    return update;
}

bool CompressionTableInfo::stepSelectAndUpdateUncompressRowStatement(
HandleStatement *select, HandleStatement *update, int64_t rowid, ColumnInfoPtrList *columnList) const
{
    select->reset();
    select->bindInteger(rowid);
    if (!select->step()) {
        return false;
    }
    if (select->done()) {
        return true;
    }
    update->reset();
    update->bindInteger(rowid);

    int preOffset = 0;
    int columnIndex = 0;
    ColumnInfoIter columnIter(&getColumnInfos(), columnList);
    const CompressionColumnInfo *column = nullptr;
    while ((column = columnIter.nextInfo()) != nullptr) {
        int selectIndex = 2 * columnIndex + preOffset;
        int updateIndex = 2 * columnIndex + 2;
        ColumnType valueType = select->getType(selectIndex);
        bool isUncompressed = select->getType(selectIndex + 1) == ColumnType::Null;

        if (valueType >= ColumnType::Text && isUncompressed) {
            UnsafeData value = select->getBLOB(selectIndex);
            Optional<UnsafeData> compressedValue;
            CompressedType compressedType = CompressedType::ZSTDDict;
            switch (column->getCompressionType()) {
            case CompressionType::Normal: {
                compressedType = CompressedType::ZSTDNormal;
                compressedValue = CompressionCenter::shared().compressContent(
                value, 0, static_cast<InnerHandle *>(select->getHandle()));
            } break;
            case CompressionType::Dict: {
                compressedValue = CompressionCenter::shared().compressContent(
                value, column->getDictId(), static_cast<InnerHandle *>(select->getHandle()));
            } break;
            case CompressionType::VariousDict: {
                int64_t matchValue = select->getInteger(selectIndex + 2);
                preOffset++;
                compressedValue = CompressionCenter::shared().compressContent(
                value,
                column->getMatchDictId(matchValue),
                static_cast<InnerHandle *>(select->getHandle()));
            } break;
            }
            if (compressedValue.failed()) {
                return false;
            }
            WCTAssert(compressedValue.value().size() <= value.size());

            if (compressedValue.value().size() < value.size()) {
                update->bindBLOB(compressedValue.value(), updateIndex);
                update->bindInteger(
                WCDBMergeCompressionType(compressedType, valueType), updateIndex + 1);
            } else {
                if (valueType == ColumnType::Text) {
                    update->bindText(select->getText(selectIndex), updateIndex);
                } else {
                    update->bindBLOB(value, updateIndex);
                }
                update->bindInteger(WCDBMergeCompressionType(CompressedType::None, valueType),
                                    updateIndex + 1);
            }
        } else {
            switch (valueType) {
            case ColumnType::Null:
                update->bindNull(updateIndex);
                break;
            case ColumnType::Integer:
                update->bindInteger(select->getInteger(selectIndex), updateIndex);
                break;
            case ColumnType::Float:
                update->bindDouble(select->getDouble(selectIndex), updateIndex);
                break;
            case ColumnType::Text:
                update->bindText(select->getText(selectIndex), updateIndex);
                break;
            case ColumnType::BLOB:
                update->bindBLOB(select->getBLOB(selectIndex), updateIndex);
                break;
            }
            update->bindInteger(select->getInteger(selectIndex + 1), updateIndex + 1);
        }
        columnIndex++;
    }
    if (!update->step()) {
        return false;
    }
    return true;
}

CompressionTableInfo::ColumnInfoIter::ColumnInfoIter(const ColumnInfoList *infoList,
                                                     ColumnInfoPtrList *infoPtrList)
: m_infoList(infoList), m_infoPtrList(infoPtrList)
{
    WCTAssert(infoList != nullptr);
    if (infoPtrList != nullptr) {
        m_infoPtrIter = infoPtrList->begin();
    } else {
        m_infoIter = infoList->begin();
    }
}

const CompressionColumnInfo *CompressionTableInfo::ColumnInfoIter::nextInfo()
{
    const CompressionColumnInfo *info = nullptr;
    if (m_infoPtrList != nullptr) {
        if (m_infoPtrIter != m_infoPtrList->end()) {
            info = *m_infoPtrIter;
            m_infoPtrIter++;
        }
    } else {
        if (m_infoIter != m_infoList->end()) {
            info = &(*m_infoIter);
            m_infoIter++;
        }
    }
    return info;
}

} // namespace WCDB
