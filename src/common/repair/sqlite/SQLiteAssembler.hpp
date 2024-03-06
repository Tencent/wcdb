//
// Created by sanhuazhang on 2018/05/19
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

#if WCDB_USE_BUILTIN_SQLITE_REPAIR

#include "Assembler.hpp"
#include "SQLiteBase.hpp"
#include "WCDBOptional.hpp"
#include <list>

namespace WCDB {

namespace Repair {

class SQLiteAssembler final : public Assembler, public SQLiteBase {
#pragma mark - Initialize
public:
    SQLiteAssembler();
    ~SQLiteAssembler() override;

#pragma mark - Assembler
public:
    void setPath(const UnsafeStringView &path) override final;
    const StringView &getPath() const override final;

    bool markAsAssembling() override final;
    bool markAsAssembled() override final;

    bool markAsMilestone() override final;

    bool assembleTable(const UnsafeStringView &tableName,
                       const UnsafeStringView &sql) override final;
    bool assembleCell(const Cell &cell) override final;
    bool assembleSQL(const UnsafeStringView &sql) override final;

    const Error &getError() const override final;

#pragma mark - Cell
protected:
    bool lazyPrepareCell();
    Optional<StringView> getAssembleSQL(const UnsafeStringView &tableName);
    Optional<std::list<StringView>> getColumnNames(const UnsafeStringView &tableName);

    StringView m_table;
    int m_primary;
    void *m_cellSTMT;

#pragma mark - Sequence
protected:
    bool markSequenceAsAssembling();
    bool assembleSequence(const UnsafeStringView &tableName, int64_t sequence) override final;
    bool markSequenceAsAssembled();

    Optional<bool> updateSequence(const UnsafeStringView &tableName, int64_t sequence);
    bool insertSequence(const UnsafeStringView &tableName, int64_t sequence);

#pragma mark - SQLite Base
protected:
    bool open();
    void close();
};

} //namespace Repair

} //namespace WCDB

#endif /* WCDB_USE_BUILTIN_SQLITE_REPAIR */
