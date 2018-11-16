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

#ifndef _WCDB_SQLITEASSEMBLER_HPP
#define _WCDB_SQLITEASSEMBLER_HPP

#include <WCDB/Assembler.hpp>
#include <WCDB/SQLiteBase.hpp>
#include <list>

namespace WCDB {

namespace Repair {

class SQLiteAssembler final : public Assembler, public SQLiteBase {
#pragma mark - Initialize
public:
    SQLiteAssembler();

#pragma mark - Assembler
public:
    void setPath(const String &path) override final;
    const String &getPath() const override final;

    bool markAsAssembling() override final;
    bool markAsAssembled() override final;

    bool markAsMilestone() override final;

    bool assembleTable(const String &tableName, const String &sql) override final;
    bool assembleCell(const Cell &cell) override final;
    bool assembleSQL(const String &sql) override final;

    void markAsDuplicated(bool duplicated) override final;

    const Error &getError() const override final;

protected:
    bool m_duplicated;

#pragma mark - Cell
protected:
    bool lazyPrepareCell();
    std::pair<bool, String> getAssembleSQL(const String &tableName);
    std::pair<bool, std::list<String>> getColumnNames(const String &tableName);

    String m_table;
    int m_primary;
    void *m_cellSTMT;

#pragma mark - Sequence
protected:
    bool markSequenceAsAssembling();
    bool assembleSequence(const String &tableName, int64_t sequence) override final;
    bool markSequenceAsAssembled();

    std::pair<bool, bool> updateSequence(const String &tableName, int64_t sequence);
    bool insertSequence(const String &tableName, int64_t sequence);

#pragma mark - SQLite Base
protected:
    bool open();
    void close();
};

} //namespace Repair

} //namespace WCDB

#endif /* _WCDB_SQLITEASSEMBLER_HPP */