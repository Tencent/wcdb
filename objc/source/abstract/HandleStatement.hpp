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

#include <WCDB/ColumnType.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/Handle.hpp>
#include <mutex>

namespace WCDB {

class HandleStatement {
public:
    bool step();

    bool isOK() const;
    const Error &getError() const;

    //bind, index begin with 1
    bool reset();

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isInteger32, void>::type
    bind(const typename ColumnTypeInfo<T>::UnderlyingType &value, int index)
    {
        bindInteger32(value, index);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isInteger64, void>::type
    bind(const typename ColumnTypeInfo<T>::UnderlyingType &value, int index)
    {
        bindInteger64(value, index);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isFloat, void>::type
    bind(const typename ColumnTypeInfo<T>::UnderlyingType &value, int index)
    {
        bindDouble(value, index);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isText, void>::type
    bind(const typename ColumnTypeInfo<T>::UnderlyingType &value, int index)
    {
        bindText(value, index);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isBLOB, void>::type
    bind(const typename ColumnTypeInfo<T>::UnderlyingType &value, int index)
    {
        bindBLOB(value, index);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isNull, void>::type
    bind(int index)
    {
        bindNull(index);
    };

    //get value, index begin with 0
    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isInteger32,
                            typename ColumnTypeInfo<T>::UnderlyingType>::type
    getValue(int index)
    {
        return getInteger32(index);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isInteger64,
                            typename ColumnTypeInfo<T>::UnderlyingType>::type
    getValue(int index)
    {
        return getInteger64(index);
    };

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isFloat,
                            typename ColumnTypeInfo<T>::UnderlyingType>::type
    getValue(int index)
    {
        return getDouble(index);
    }

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isText,
                            typename ColumnTypeInfo<T>::UnderlyingType>::type
    getValue(int index)
    {
        return getText(index);
    }

    template <ColumnType T>
    typename std::enable_if<ColumnTypeInfo<T>::isBLOB,
                            typename ColumnTypeInfo<T>::UnderlyingType>::type
    getValue(int index)
    {
        return getBLOB(index);
    }

    ColumnType getType(int index);

    int getColumnCount();
    const char *getColumnName(int index);
    const char *getColumnTableName(int index);

    long long getLastInsertedRowID();

    bool finalize();

    int getChanges();

    ~HandleStatement();

protected:
    void bindInteger32(
        const ColumnTypeInfo<ColumnType::Integer32>::UnderlyingType &value,
        int index);
    void bindInteger64(
        const ColumnTypeInfo<ColumnType::Integer64>::UnderlyingType &value,
        int index);
    void
    bindDouble(const ColumnTypeInfo<ColumnType::Float>::UnderlyingType &value,
               int index);
    void bindText(const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &value,
                  int index);
    void bindBLOB(const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &value,
                  int index);
    void bindNull(int index);

    ColumnTypeInfo<ColumnType::Integer32>::UnderlyingType
    getInteger32(int index);
    ColumnTypeInfo<ColumnType::Integer64>::UnderlyingType
    getInteger64(int index);
    ColumnTypeInfo<ColumnType::Float>::UnderlyingType getDouble(int index);
    ColumnTypeInfo<ColumnType::Text>::UnderlyingType getText(int index);
    ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType getBLOB(int index);

    HandleStatement(void *stmt, const Handle &handle);
    const HandleStatement &operator=(const HandleStatement &other) = delete;
    HandleStatement(const HandleStatement &other) = delete;

    const Handle &m_handle;
    Error m_error;
    void *m_stmt;

    friend class Handle;
};

} //namespace WCDB

#endif /* HandleStatement_hpp */
