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
#include <list>

namespace WCDB {

namespace Repair {

class SQLiteAssembler : public Assembler, public SharedThreadedErrorProne {
#pragma mark - Initialize
public:
    SQLiteAssembler(const std::string &path);

    typedef std::function<int(const std::string &, void *)>
        TableAssembledCallback;
    void
    setCallbackOnTableAssembled(const TableAssembledCallback &onTableAssembled);

    const std::string path;

protected:
    int onTableAssembled(const std::string &tableName);
    TableAssembledCallback m_onTableAssembled;

#pragma mark - Assembler
public:
    bool markAsAssembling() override;
    void markAsAssembled() override;

    bool markAsMilestone() override;

    bool assembleTable(const std::string &tableName,
                       const std::string &sql) override;
    bool assembleCell(const Cell &cell) override;

    const Error &getError() const override;

#pragma mark - Helper
protected:
    std::pair<bool, std::string> getAssembleSQL(const std::string &tableName);
    std::pair<bool, std::list<std::string>>
    getColumnNames(const std::string &tableName);

protected:
    bool lazyBeginTransaction();
    bool lazyCommitOrRollbackTransaction(bool commit = true);

private:
    bool m_transaction;

#pragma mark - Error
protected:
    void setThreadedError(int rc);
    void setThreadedError(int rc, const std::string &sql);

#pragma mark - SQLite Handle
protected:
    bool open();
    bool isOpened() const;
    void close();

    bool execute(const char *sql, bool ignoreError = false);

    void *m_handle;

#pragma mark - SQLite STMT
protected:
    bool prepare(const char *sql);
    bool step();
    bool step(bool &done);

    bool isPrepared() const;
    void finalize();

    void *m_stmt;
};

} //namespace Repair

} //namespace WCDB

#endif /* SQLiteAssembler_hpp */
