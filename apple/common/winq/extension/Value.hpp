//
// Created by qiuwenchen on 2022/8/10.
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
#include <WCDB/ColumnType.hpp>
#include <WCDB/Optional.hpp>
#include <WCDB/ValueArray.hpp>

namespace WCDB {

class Value {
public:
    template<class T, std::enable_if_t<ColumnIsIntegerType<T>::value, int> = 0>
    Value(const T& value) : m_type(Type::Integer)
    {
        m_value.intValue = ColumnIsIntegerType<T>::asUnderlyingType(value);
    }

    template<class T, std::enable_if_t<ColumnIsFloatType<T>::value, int> = 0>
    Value(const T& value) : m_type(Type::Float)
    {
        m_value.floatValue = ColumnIsFloatType<T>::asUnderlyingType(value);
    }

    template<class T, std::enable_if_t<ColumnIsTextType<T>::value, int> = 0>
    Value(const T& value) : m_type(Type::Text)
    {
        m_value.textValue = new StringView(ColumnIsTextType<T>::asUnderlyingType(value));
    }

    template<class T, std::enable_if_t<ColumnIsBLOBType<T>::value, int> = 0>
    Value(const T& value) : m_type(Type::BLOB)
    {
        m_value.blobValue = new Data(ColumnIsBLOBType<T>::asUnderlyingType(value));
    }

    template<class T, std::enable_if_t<ColumnIsNullType<T>::value, int> = 0>
    Value(const T& value) : m_type(Type::Null)
    {
        WCDB_UNUSED(value);
        m_value.intValue = 0;
    }

    Value();
    Value(const Value& other);
    Value(Value&& other);
    ~Value();

    Value& operator=(const Value& other);
    Value& operator=(Value&& other);

    bool operator==(const Value& other) const;
    template<class T, std::enable_if_t<std::is_constructible<Value, T>::value, int> = 0>
    bool operator==(const T& other) const
    {
        return operator==(Value(other));
    }

    bool operator!=(const Value& other) const;
    template<class T, std::enable_if_t<std::is_constructible<Value, T>::value, int> = 0>
    bool operator!=(const T& other) const
    {
        return operator!=(Value(other));
    }

    template<class T, std::enable_if_t<ColumnIsIntegerType<T>::value, int> = 0>
    operator T() const
    {
        return (T) intValue();
    }
    template<class T, std::enable_if_t<ColumnIsFloatType<T>::value, int> = 0>
    operator T() const
    {
        return (T) floatValue();
    }
    operator bool() const;
    operator StringView() const;
    operator std::string() const;
    operator Data() const;

    using Type = ColumnType;
    Type getType() const;

    bool boolValue() const;
    int64_t intValue() const;
    double floatValue() const;
    StringView textValue() const;
    Data blobValue() const;
    bool isNull() const;
    bool isEmpty() const;

private:
    Type m_type;
    union {
        int64_t intValue;
        double floatValue;
        StringView* textValue;
        Data* blobValue;
    } m_value;

    void clearValue();
    void copyValue(const Value& other);
    void moveValue(Value& other);
};

typedef ValueArray<Value> OneRowValue;
typedef ValueArray<Value> OneColumnValue;
typedef ValueArray<ValueArray<Value>> MultiRowsValue;

template<typename ValueType>
using OptionalValueArray = Optional<ValueArray<ValueType>>;

typedef Optional<Value> OptionalValue;
typedef OptionalValueArray<Value> OptionalOneRow;
typedef OptionalValueArray<Value> OptionalOneColumn;
typedef OptionalValueArray<ValueArray<Value>> OptionalMultiRows;

} // namespace WCDB
