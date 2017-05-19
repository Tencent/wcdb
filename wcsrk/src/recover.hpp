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

#ifndef recover_hpp
#define recover_hpp

#include <memory>
#include "value.hpp"

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

class RecoverStatement;

class Recover
{
public:
    Recover(sqlite3* handle);
    
    std::shared_ptr<RecoverStatement> prepare(const std::string& tableName, const size_t& valuesCount);
protected:
    sqlite3* m_handle;
};

class RecoverStatement
{
public:
    RecoverStatement(sqlite3_stmt* stmt);
    ~RecoverStatement();
    
    bool execWithValues(const std::shared_ptr<Values>& values);
protected:
    sqlite3_stmt* m_stmt;
    size_t m_transactionCapacity;
};

#endif /* recover_hpp */
