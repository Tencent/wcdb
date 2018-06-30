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

#ifndef SQLiteAssembler_hpp
#define SQLiteAssembler_hpp

#include <WCDB/Assembler.hpp>
#include <WCDB/SQLiteBase.hpp>
#include <list>

namespace WCDB {

namespace Repair {

class SQLiteAssembler : public Assembler, public SQLiteBase {
#pragma mark - Initialize
public:
    SQLiteAssembler();

#pragma mark - Assembler
public:
    void setPath(const std::string &path) override;
    const std::string &getPath() const override;

    bool markAsAssembling() override;
    bool markAsAssembled() override;

    bool markAsMilestone() override;

    bool assembleTable(const std::string &tableName,
                       const std::string &sql) override;
    bool assembleCell(const Cell &cell) override;

    void markAsDuplicated(bool duplicated) override;

    const Error &getError() const override;

protected:
    bool m_duplicated;

#pragma mark - Cell
protected:
    bool lazyPrepareCell();
    std::pair<bool, std::string> getAssembleSQL(const std::string &tableName);
    std::pair<bool, std::list<std::string>>
    getColumnNames(const std::string &tableName);

    std::string m_table;
    int m_primary;
    void *m_cellSTMT;

#pragma mark - Sequence
protected:
    bool markSequenceAsAssembling();
    bool assembleSequence(const std::string &tableName,
                          int64_t sequence) override;
    bool markSequenceAsAssembled();

    std::pair<bool, bool> updateSequence(const std::string &tableName,
                                         int64_t sequence);
    bool insertSequence(const std::string &tableName, int64_t sequence);

#pragma mark - SQLite Base
protected:
    bool open();
    void close();
};

} //namespace Repair

} //namespace WCDB

#endif /* SQLiteAssembler_hpp */
