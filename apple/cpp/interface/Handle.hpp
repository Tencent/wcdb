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
#include <WCDB/HandleOperation.hpp>
#include <WCDB/HandleStatement.hpp>
#include <WCDB/RecyclableHandle.hpp>
#include <WCDB/Statement.hpp>

namespace WCDB {

class InnerDatabase;
class Database;

class Handle : public HandleOperation, public StatementOperation {
    friend class Database;
    friend class HandleOperation;

#pragma mark - Basic
protected:
    Handle() = delete;
    Handle(const HandleStatement&) = delete;
    Handle& operator=(const HandleStatement&) = delete;

    Handle(InnerHandle* handle);
    Handle(RecyclableHandle handle);
    Handle(Recyclable<InnerDatabase*> database);

    InnerHandle* getOrGenerateHandle();
    InnerHandleStatement* getInnerHandleStatement() override final;
    RecyclableHandle getHandleHolder() override final;

private:
    Recyclable<InnerDatabase*> m_databaseHolder;
    RecyclableHandle m_handleHolder;
    InnerHandle* m_innerHandle;

public:
    Handle(Handle&& other);
    ~Handle() override final;
    long long getLastInsertedRowID();
    int getChanges();
    int getTotalChange();

#pragma mark - Multi Statement
public:
    HandleStatement& getOrCreateHandleStatementWithTag(const UnsafeStringView& tag);
    void finalizeAllStatement();

private:
    StringViewMap<HandleStatement> m_handleStatementDic;
};

} //namespace WCDB
