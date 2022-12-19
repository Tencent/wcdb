//
// Created by qiuwenchen on 2022/8/3.
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
#include <WCDB/HandleORMOperation.hpp>
#include <WCDB/HandleOperation.hpp>
#include <WCDB/PreparedStatement.hpp>
#include <WCDB/Statement.hpp>

namespace WCDB {

class Handle final : public StatementOperation, public HandleORMOperation {
    friend class Database;
    friend class HandleOperation;
    friend class TableOperation;
    friend class BaseChainCall;

#pragma mark - Basic
protected:
    Handle() = delete;
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    Handle(RecyclableHandle handle);
    Handle(Recyclable<InnerDatabase*> database);
    Handle(Recyclable<InnerDatabase*> database, InnerHandle* handle);

    InnerHandle* getOrGenerateHandle();
    HandleStatement* getInnerHandleStatement() override final;
    RecyclableHandle getHandleHolder() override final;
    Recyclable<InnerDatabase*> getDatabaseHolder() override final;

private:
    Recyclable<InnerDatabase*> m_databaseHolder;
    RecyclableHandle m_handleHolder;
    InnerHandle* m_innerHandle;

public:
    Handle(Handle&& other);
    ~Handle() override final;
    //Handles of current thread need to be destructed or invalidated before you close the database.
    void invalidate();
    long long getLastInsertedRowID();
    int getChanges();
    int getTotalChange();
    const Error& getError();

#pragma mark - Statement
    bool prepare(const Statement& statement);
    bool isPrepared();
    void finalize();

#pragma mark - Multi Statement
public:
    std::optional<PreparedStatement>
    getOrCreatePreparedStatement(const Statement& statement);
    void finalizeAllStatement();
};

} //namespace WCDB
