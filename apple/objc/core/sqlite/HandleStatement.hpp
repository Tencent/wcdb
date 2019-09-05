//
// Created by sanhuazhang on 2019/05/19
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

#include <WCDB/HandleRelated.hpp>
#include <WCDB/SQLiteDeclaration.h>
#include <WCDB/WINQ.h>

namespace WCDB {

class HandleStatement final : public HandleRelated {
    friend class AbstractHandle;

public:
    HandleStatement() = delete;
    HandleStatement(const HandleStatement &) = delete;
    HandleStatement &operator=(const HandleStatement &) = delete;

    HandleStatement(HandleStatement &&other);

    ~HandleStatement() override final;

    bool prepare(const Statement &statement);
    bool isPrepared();
    void finalize();

    bool step();
    bool done();
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
    void bindBLOB(const BLOB &value, int index);
    void bindNull(int index);

    Integer32 getInteger32(int index);
    Integer64 getInteger64(int index);
    Float getDouble(int index);
    Text getText(int index);
    const BLOB getBLOB(int index);

    ColumnType getType(int index);

    int getNumberOfColumns();
    const UnsafeStringView getOriginColumnName(int index);
    const UnsafeStringView getColumnName(int index);
    const UnsafeStringView getColumnTableName(int index);

    bool isReadonly();

protected:
    bool isBusy();
    HandleStatement(AbstractHandle *handle);

private:
    bool prepare(const UnsafeStringView &sql);
    sqlite3_stmt *m_stmt;
    bool m_done;
};

} //namespace WCDB
