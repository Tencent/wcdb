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

#ifndef HandleStatement_hpp
#define HandleStatement_hpp

#include <WCDB/WINQ.h>

#pragma GCC visibility push(hidden)

namespace WCDB {

class Handle;

class HandleStatement {
public:
    HandleStatement();

    void setup(Handle *handle, const Statement &statement, void *stmt);

    bool step(bool &done);
    bool step();
    void finalize();
    bool isStatementReadonly();
    void reset();

    using Integer32 = ColumnTypeInfo<ColumnType::Integer32>::UnderlyingType;
    using Integer64 = ColumnTypeInfo<ColumnType::Integer64>::UnderlyingType;
    using Text = ColumnTypeInfo<ColumnType::Text>::UnderlyingType;
    using Float = ColumnTypeInfo<ColumnType::Float>::UnderlyingType;
    using BLOB = ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType;

    void bindInteger32(const Integer32 &value, int index);
    void bindInteger64(const Integer64 &value, int index);
    void bindDouble(const Float &value, int index);
    void bindText(const Text &value, int index);
    void bindText(const Text &value, int length, int index);
    void bindBLOB(const BLOB &value, int index);
    void bindNull(int index);

    Integer32 getInteger32(int index);
    Integer64 getInteger64(int index);
    Float getDouble(int index);
    Text getText(int index);
    const BLOB getBLOB(int index);

    ColumnType getType(int index);

    int getColumnCount();
    const char *getColumnName(int index);
    const char *getColumnTableName(int index);

    bool isReadonly();
    bool isPrepared();

    const char *getSQL();

    const Statement &getStatement();

protected:
    friend class Handle;
    void setError(int rc, const std::string &sql);
    Handle *m_handle;

    Statement m_statement;
    void *m_stmt;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* HandleStatement_hpp */
