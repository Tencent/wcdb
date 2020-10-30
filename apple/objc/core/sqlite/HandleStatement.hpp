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

class HandleStatement : public HandleRelated {
    friend class AbstractHandle;

public:
    HandleStatement() = delete;
    HandleStatement(const HandleStatement &) = delete;
    HandleStatement &operator=(const HandleStatement &) = delete;

    HandleStatement(HandleStatement &&other);
    HandleStatement(AbstractHandle *handle);

    virtual ~HandleStatement() override;

    virtual bool prepare(const Statement &statement);
    virtual bool prepare(const UnsafeStringView &sql);
    virtual bool isPrepared();
    virtual void finalize();

    virtual bool step();
    virtual bool done();
    virtual void reset();

    using Integer = ColumnTypeInfo<ColumnType::Integer>::UnderlyingType;
    using Text = ColumnTypeInfo<ColumnType::Text>::UnderlyingType;
    using Float = ColumnTypeInfo<ColumnType::Float>::UnderlyingType;
    using BLOB = ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType;

    virtual void bindInteger(const Integer &value, int index);
    virtual void bindDouble(const Float &value, int index);
    virtual void bindText(const Text &value, int index);
    virtual void bindBLOB(const BLOB &value, int index);
    virtual void bindNull(int index);
    virtual void bindPointer(void *ptr, int index, const Text &type, void(*destructor)(void*));
    int bindParameterIndex(const Text &parameterName);

    virtual Integer getInteger(int index);
    virtual Float getDouble(int index);
    virtual Text getText(int index);
    virtual const BLOB getBLOB(int index);

    virtual ColumnType getType(int index);

    virtual int getNumberOfColumns();
    virtual const UnsafeStringView getOriginColumnName(int index);
    virtual const UnsafeStringView getColumnName(int index);
    virtual const UnsafeStringView getColumnTableName(int index);

    virtual bool isReadonly();

protected:
    virtual bool isBusy();

private:
    sqlite3_stmt *m_stmt;
    bool m_done;
};

} //namespace WCDB
