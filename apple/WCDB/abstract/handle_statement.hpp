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

#ifndef handle_statement_hpp
#define handle_statement_hpp

#include <WCDB/column_type.hpp>
#include <WCDB/describable.hpp>
#include <WCDB/error.hpp>
#include <WCDB/handle.hpp>
#include <mutex>
#include <sqlcipher/sqlite3.h>

namespace WCDB {

class StatementHandle {
public:
    bool step();

    bool isOK() const;
    const Error &getError() const;

    //bind, index begin with 1
    void resetBinding();

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isInteger32, void>::type
    bind(typename ColumnTypeInfo<T>::CType value, int index)
    {
        sqlite3_bind_int(m_stmt, index, value);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isInteger64, void>::type
    bind(typename ColumnTypeInfo<T>::CType value, int index)
    {
        sqlite3_bind_int64(m_stmt, index, value);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isFloat, void>::type
    bind(typename ColumnTypeInfo<T>::CType value, int index)
    {
        sqlite3_bind_double(m_stmt, index, value);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isText, void>::type
    bind(typename ColumnTypeInfo<T>::CType value, int index)
    {
        sqlite3_bind_text(m_stmt, index, value, -1, SQLITE_TRANSIENT);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isBLOB, void>::type
    bind(typename ColumnTypeInfo<T>::CType value, int size, int index)
    {
        sqlite3_bind_blob(m_stmt, index, value, size, SQLITE_TRANSIENT);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isNull, void>::type
    bind(int index)
    {
        sqlite3_bind_null(m_stmt, index);
    };

    //get value, index begin with 0
    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isInteger32,
                            typename ColumnTypeInfo<T>::CType>::type
    getValue(int index)
    {
        return (typename ColumnTypeInfo<T>::CType) sqlite3_column_int(m_stmt,
                                                                      index);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isInteger64,
                            typename ColumnTypeInfo<T>::CType>::type
    getValue(int index)
    {
        return (typename ColumnTypeInfo<T>::CType) sqlite3_column_int64(m_stmt,
                                                                        index);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isFloat,
                            typename ColumnTypeInfo<T>::CType>::type
    getValue(int index)
    {
        return (typename ColumnTypeInfo<T>::CType) sqlite3_column_double(m_stmt,
                                                                         index);
    }

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isText,
                            typename ColumnTypeInfo<T>::CType>::type
    getValue(int index)
    {
        return (typename ColumnTypeInfo<T>::CType) sqlite3_column_text(m_stmt,
                                                                       index);
    }

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isBLOB,
                            typename ColumnTypeInfo<T>::CType>::type
    getValue(int index, int &size)
    {
        size = sqlite3_column_bytes(m_stmt, index);
        return (typename ColumnTypeInfo<T>::CType) sqlite3_column_blob(m_stmt,
                                                                       index);
    }

    ColumnType getType(int index);

    int getColumnCount();
    const char *getColumnName(int index);
    const char *getColumnTableName(int index);

    long long getLastInsertedRowID();

    void finalize();

    ~StatementHandle();

protected:
    StatementHandle(sqlite3_stmt *stmt, const Handle &handle);
    const StatementHandle &operator=(const StatementHandle &other) = delete;
    StatementHandle(const StatementHandle &other) = delete;

    const Handle &m_handle;
    Error m_error;
    sqlite3_stmt *m_stmt;

    friend class Handle;
};

} //namespace WCDB

#endif /* handle_statement_hpp */
