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
#include "ColumnType.hpp"
#include "ValueArray.hpp"
#include "WCDBOptional.hpp"
#if defined(__cplusplus) && __cplusplus > 201402L
#include <optional>
#endif

namespace WCDB {

template<class T>
struct IsSTDSharedPtr : std::false_type {};

template<class T>
struct IsSTDSharedPtr<std::shared_ptr<T>> : std::true_type {};

#if defined(__cplusplus) && __cplusplus > 201402L
template<class T>
struct IsSTDOptional : std::false_type {};

template<class T>
struct IsSTDOptional<std::optional<T>> : std::true_type {};
#endif

class WCDB_API Value {
public:
#pragma mark - Base Type
    template<class T, std::enable_if_t<ColumnIsIntegerType<T>::value, int> = 0>
    Value(const T& value) : m_type(Type::Integer)
    {
        m_intValue = ColumnIsIntegerType<T>::asUnderlyingType(value);
    }

    template<class T, std::enable_if_t<ColumnIsFloatType<T>::value, int> = 0>
    Value(const T& value) : m_type(Type::Float)
    {
        m_floatValue = ColumnIsFloatType<T>::asUnderlyingType(value);
    }

    template<class T, std::enable_if_t<ColumnIsTextType<T>::value, int> = 0>
    Value(const T& value) : m_type(Type::Text)
    {
        new ((void*) std::addressof(m_textValue))
        StringView(ColumnIsTextType<T>::asUnderlyingType(value));
    }

    template<class T, std::enable_if_t<ColumnIsBLOBType<T>::value, int> = 0>
    Value(const T& value) : m_type(Type::BLOB)
    {
        new ((void*) std::addressof(m_blobValue))
        Data(ColumnIsBLOBType<T>::asUnderlyingType(value));
    }

    Value(const std::nullptr_t& value) : m_type(Type::Null)
    {
        WCDB_UNUSED(value);
        m_intValue = 0;
    }

#pragma mark - Shared Pointer
    template<class T, std::enable_if_t<IsSTDSharedPtr<T>::value && ColumnIsIntegerType<typename T::element_type>::value, int> = 0>
    Value(const T& value) : m_type(Type::Integer)
    {
        if (value != nullptr) {
            m_intValue
            = ColumnIsIntegerType<typename T::element_type>::asUnderlyingType(*value);
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }

    template<class T, std::enable_if_t<IsSTDSharedPtr<T>::value && ColumnIsFloatType<typename T::element_type>::value, int> = 0>
    Value(const T& value) : m_type(Type::Float)
    {
        if (value != nullptr) {
            m_floatValue
            = ColumnIsFloatType<typename T::element_type>::asUnderlyingType(*value);
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }

    template<class T, std::enable_if_t<IsSTDSharedPtr<T>::value && ColumnIsTextType<typename T::element_type>::value, int> = 0>
    Value(const T& value) : m_type(Type::Text)
    {
        if (value != nullptr) {
            new ((void*) std::addressof(m_textValue)) StringView(
            ColumnIsTextType<typename T::element_type>::asUnderlyingType(*value));
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }

    template<class T, std::enable_if_t<IsSTDSharedPtr<T>::value && ColumnIsBLOBType<typename T::element_type>::value, int> = 0>
    Value(const T& value) : m_type(Type::BLOB)
    {
        if (value != nullptr) {
            new ((void*) std::addressof(m_blobValue))
            Data(ColumnIsBLOBType<typename T::element_type>::asUnderlyingType(*value));
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }

#pragma mark - STD Optional
#if defined(__cplusplus) && __cplusplus > 201402L
    template<class T, std::enable_if_t<IsSTDOptional<T>::value && ColumnIsIntegerType<typename T::value_type>::value, int> = 0>
    Value(const T& value) : m_type(Type::Integer)
    {
        if (value.has_value()) {
            m_intValue = ColumnIsIntegerType<typename T::value_type>::asUnderlyingType(
            value.value());
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }

    template<class T, std::enable_if_t<IsSTDOptional<T>::value && ColumnIsFloatType<typename T::value_type>::value, int> = 0>
    Value(const T& value) : m_type(Type::Float)
    {
        if (value.has_value()) {
            m_floatValue
            = ColumnIsFloatType<typename T::value_type>::asUnderlyingType(value.value());
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }

    template<class T, std::enable_if_t<IsSTDOptional<T>::value && ColumnIsTextType<typename T::value_type>::value, int> = 0>
    Value(const T& value) : m_type(Type::Text)
    {
        if (value.has_value()) {
            new ((void*) std::addressof(m_textValue)) StringView(
            ColumnIsTextType<typename T::value_type>::asUnderlyingType(value.value()));
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }

    template<class T, std::enable_if_t<IsSTDOptional<T>::value && ColumnIsBLOBType<typename T::value_type>::value, int> = 0>
    Value(const T& value) : m_type(Type::BLOB)
    {
        if (value.has_value()) {
            new ((void*) std::addressof(m_blobValue)) Data(
            ColumnIsBLOBType<typename T::value_type>::asUnderlyingType(value.value()));
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }
#endif

#pragma mark - WCDB Optional
    template<class T, std::enable_if_t<IsWCDBOptional<T>::value && ColumnIsIntegerType<typename T::ValueType>::value, int> = 0>
    Value(const T& value) : m_type(Type::Integer)
    {
        if (value.hasValue()) {
            m_intValue
            = ColumnIsIntegerType<typename T::ValueType>::asUnderlyingType(value.value());
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }

    template<class T, std::enable_if_t<IsWCDBOptional<T>::value && ColumnIsFloatType<typename T::ValueType>::value, int> = 0>
    Value(const T& value) : m_type(Type::Float)
    {
        if (value.hasValue()) {
            m_floatValue
            = ColumnIsFloatType<typename T::ValueType>::asUnderlyingType(value.value());
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }

    template<class T, std::enable_if_t<IsWCDBOptional<T>::value && ColumnIsTextType<typename T::ValueType>::value, int> = 0>
    Value(const T& value) : m_type(Type::Text)
    {
        if (value.hasValue()) {
            new ((void*) std::addressof(m_textValue)) StringView(
            ColumnIsTextType<typename T::ValueType>::asUnderlyingType(value.value()));
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }

    template<class T, std::enable_if_t<IsWCDBOptional<T>::value && ColumnIsBLOBType<typename T::ValueType>::value, int> = 0>
    Value(const T& value) : m_type(Type::BLOB)
    {
        if (value.hasValue()) {
            new ((void*) std::addressof(m_blobValue))
            Data(ColumnIsBLOBType<typename T::ValueType>::asUnderlyingType(value.value()));
        } else {
            m_type = Type::Null;
            m_intValue = 0;
        }
    }

#pragma mark - Basic
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
        int64_t m_intValue;
        double m_floatValue;
        StringView m_textValue;
        Data m_blobValue;
    };

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
