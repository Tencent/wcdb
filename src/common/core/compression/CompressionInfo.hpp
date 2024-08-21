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

#pragma once

#include "Column.hpp"
#include "ColumnType.hpp"
#include "StringView.hpp"
#include "ZSTDDict.hpp"
#include <atomic>
#include <list>
#include <memory>
#include <unordered_map>

namespace WCDB {

class HandleStatement;
class CompressionTableInfo;

enum class CompressionType {
    Normal,
    Dict,
    VariousDict,
};

class CompressionColumnInfo {
    friend CompressionTableInfo;

public:
    using DictId = ZSTDDict::DictId;
    using Integer = ColumnTypeInfo<ColumnType::Integer>::UnderlyingType;
    CompressionColumnInfo() = delete;
    CompressionColumnInfo(const Column &column, CompressionType type);
    CompressionColumnInfo(const Column &column, const Column &matchColumn);
    CompressionColumnInfo(const CompressionColumnInfo &other);
    CompressionColumnInfo(CompressionColumnInfo &&other);
    CompressionColumnInfo &operator=(const CompressionColumnInfo &other);
    CompressionColumnInfo &operator=(CompressionColumnInfo &&other);

    const Column &getColumn() const;
    void setColumnIndex(uint16_t index) const;
    uint16_t getColumnIndex() const;

    const Column &getTypeColumn() const;
    void setTypeColumnIndex(uint16_t index) const;
    uint16_t getTypeColumnIndex() const;

    const Column &getMatchColumn() const;
    void setMatchColumnIndex(uint16_t index) const;
    uint16_t getMatchColumnIndex() const;

    CompressionType getCompressionType() const;
    DictId getDictId() const;
    DictId getMatchDictId(const Integer &matchValue) const;

    void setCommonDict(DictId dictId);
    void addMatchDict(const Integer &matchValue, DictId dictId);

private:
    Column m_column;
    mutable std::atomic_ushort m_columnIndex;
    Column m_typeColumn;
    mutable std::atomic_ushort m_typeColumnIndex;
    Column m_matchColumn;
    mutable std::atomic_ushort m_matchColumnIndex;

    CompressionType m_compressionType;
    DictId m_commonDictID;
    std::unordered_map<Integer, DictId> m_matchDicts;
};

class CompressionTableBaseInfo {
public:
    CompressionTableBaseInfo(const UnsafeStringView &table);
    bool shouldCompress() const;
    const StringView &getTable() const;

    typedef const std::list<CompressionColumnInfo> ColumnInfoList;
    ColumnInfoList &getColumnInfos() const;

protected:
    StringView m_table;
    std::list<CompressionColumnInfo> m_compressingColumns;
    bool m_replaceCompression;
};

class CompressionTableUserInfo : public CompressionTableBaseInfo {
public:
    CompressionTableUserInfo(const UnsafeStringView &table);
    CompressionTableUserInfo(const UnsafeStringView &table,
                             const std::list<CompressionColumnInfo> &columns);
    void addCompressingColumn(const CompressionColumnInfo &info);
    void enableReplaceCompresssion();
};

class CompressionTableInfo : public CompressionTableBaseInfo {
public:
    CompressionTableInfo() = delete;
    CompressionTableInfo(const UnsafeStringView &table) = delete;
    CompressionTableInfo(const CompressionTableUserInfo &userInfo);
    void addCompressingColumn(const CompressionColumnInfo &info);

    void setMinCompressedRowid(int64_t rowid) const;
    int64_t getMinCompressedRowid() const;

    bool needCheckColumns() const;
    void setNeedCheckColumns(bool needCheck) const;

    StringView getCompressionDescription() const;
    bool shouldReplaceCompression() const;

private:
    mutable int64_t m_minCompressedRowid;
    mutable bool m_needCheckColumn;

#pragma mark - Compress Statements
public:
    typedef const std::list<const CompressionColumnInfo *> ColumnInfoPtrList;
    /*
     SELECT rowid FROM compressingTable
     WHERE rowid < ?
     (WCDB_CT_compressingColumnA IS NULL OR WCDB_CT_compressingColumnB IS NULL ...)
     ORDER BY rowid DESC
     LIMIT 10
     */
    StatementSelect getSelectNeedCompressRowIdStatement() const;

    /*
     SELECT * FROM compressingTable WHERE rowid = ?
     */
    StatementSelect getSelectRowStatement() const;

    /*
     DELECT FROM compressingTable WHERE rowid = ?
     */
    StatementDelete getDelectRowStatement() const;

    /*
     INSERT INTO compressingTable VALUES(?1, ?2, ...)
     */
    StatementInsert getInsertNewRowStatement(size_t valueCount) const;

    /*
     SELECT compressingColumnA, WCDB_CT_compressingColumnA,
     compressingColumnB, WCDB_CT_compressingColumnB ...
     FROM compressingTable
     WHERE rowid == ?
     */
    StatementSelect
    getSelectUncompressRowStatement(ColumnInfoPtrList *columnList = nullptr) const;

    /*
     UPDATE compressingTable
     SET compressingColumnA = ?2, WCDB_CT_compressingColumnA = ?3,
     compressingColumnB = ?4, WCDB_CT_compressingColumnB = ?5 ...
     WHERE rowid == ?1
     */
    StatementUpdate
    getUpdateCompressColumnStatement(ColumnInfoPtrList *columnList = nullptr) const;

    bool stepSelectAndUpdateUncompressRowStatement(HandleStatement *select,
                                                   HandleStatement *update,
                                                   int64_t rowid,
                                                   ColumnInfoPtrList *columnList
                                                   = nullptr) const;

#pragma mark - Revert compression
public:
    /*
     SELECT rowid FROM compressingTable
     WHERE rowid < ? AND (WCDB_CT_compressingColumnA NOTNULL OR WCDB_CT_compressingColumnB NOTNULL ...)
     ORDER BY rowid DESC
     */
    StatementSelect getSelectCompressedRowIdStatement(int64_t maxRowId) const;

    /*
     SELECT wcdb_decompress(compressingColumnA, WCDB_CT_compressingColumnA),
     wcdb_decompress(compressingColumnB, WCDB_CT_compressingColumnB) ...
     FROM compressingTable
     WHERE rowid == ?
     */
    StatementSelect
    getSelectCompressedRowStatement(ColumnInfoPtrList *columnList = nullptr) const;

    bool stepSelectAndUpdateCompressedRowStatement(HandleStatement *select,
                                                   HandleStatement *update,
                                                   int64_t rowid,
                                                   ColumnInfoPtrList *columnList
                                                   = nullptr) const;

private:
    class ColumnInfoIter {
    public:
        ColumnInfoIter(ColumnInfoList *infoList, ColumnInfoPtrList *infoPtrList);
        const CompressionColumnInfo *nextInfo();

    private:
        ColumnInfoList *m_infoList;
        ColumnInfoPtrList *m_infoPtrList;
        ColumnInfoList::const_iterator m_infoIter;
        ColumnInfoPtrList::const_iterator m_infoPtrIter;
    };
};

} // namespace WCDB
