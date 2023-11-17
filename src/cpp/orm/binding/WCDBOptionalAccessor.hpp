//
// Created by qiuwenchen on 2023/11/1.
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

#include "RunTimeAccessor.hpp"
#include <memory>

namespace WCDB {

template<class ORMType, typename FieldType>
class BaseRuntimeAccessor<ORMType, FieldType, typename std::enable_if<IsWCDBOptional<FieldType>::value>::type>
: public Accessor<ORMType, (ColumnType) ColumnInfo<typename FieldType::ValueType>::type> {
protected:
    using Super
    = Accessor<ORMType, (ColumnType) ColumnInfo<typename FieldType::ValueType>::type>;
    using UnderlyingType = typename Super::UnderlyingType;

    FieldType ORMType::*m_memberPointer;

public:
    BaseRuntimeAccessor(FieldType ORMType::*memberPointer)
    : m_memberPointer(memberPointer)
    {
        static_assert(IsFieldMemberPointer<FieldType ORMType::*>::value,
                      "Field should be initialize with a member field pointer, not a member function pointer");
    }
    virtual ~BaseRuntimeAccessor() override = default;

    UnderlyingType getValue(const ORMType &instance) const override final
    {
        return (UnderlyingType) (instance.*m_memberPointer).value();
    }
};

template<class ORMType, typename FieldType>
class RuntimeNullAccessor<ORMType, FieldType, typename std::enable_if<IsWCDBOptional<FieldType>::value>::type>
: public BaseRuntimeAccessor<ORMType, FieldType> {
protected:
    using Super = BaseRuntimeAccessor<ORMType, FieldType>;

public:
    RuntimeNullAccessor(FieldType ORMType::*memberPointer)
    : Super(memberPointer)
    {
    }
    virtual ~RuntimeNullAccessor() override = default;

    bool isNull(const ORMType &instance) const override
    {
        return !(instance.*Super::m_memberPointer).hasValue();
    }

    void setNull(ORMType &instance) const override
    {
        instance.*Super::m_memberPointer = NullOpt;
    }
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<
ORMType,
FieldType,
typename std::enable_if<IsWCDBOptional<FieldType>::value && !ColumnIsBLOBType<typename FieldType::ValueType>::value
                        && !ColumnIsTextType<typename FieldType::ValueType>::value
                        && !std::is_enum<typename FieldType::ValueType>::value>::type>
final : public RuntimeNullAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeNullAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeNullAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    ~RuntimeAccessor() override final = default;

    void setValue(ORMType &instance, const UnderlyingType &value) const override final
    {
        instance.*Super::m_memberPointer = Optional<typename FieldType::ValueType>(value);
    }
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<
ORMType,
FieldType,
typename std::enable_if<IsWCDBOptional<FieldType>::value && !ColumnIsBLOBType<typename FieldType::ValueType>::value
                        && !ColumnIsTextType<typename FieldType::ValueType>::value
                        && std::is_enum<typename FieldType::ValueType>::value>::type>
final : public RuntimeNullAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeNullAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeNullAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    ~RuntimeAccessor() override final = default;

    void setValue(ORMType &instance, const UnderlyingType &value) const override final
    {
        instance.*Super::m_memberPointer
        = Optional<typename FieldType::ValueType>((typename FieldType::ValueType) value);
    }
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<ORMType, FieldType, typename std::enable_if<IsWCDBOptional<FieldType>::value && ColumnIsTextType<typename FieldType::ValueType>::value>::type> final
: public RuntimeNullAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeNullAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;
    using ValueType = typename FieldType::ValueType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeNullAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    ~RuntimeAccessor() override final = default;

    void setValue(ORMType &instance, const UnderlyingType &value) const override final
    {
        instance.*Super::m_memberPointer = ValueType();
        ColumnIsTextType<typename FieldType::ValueType>::setToUnderlyingType(
        (instance.*Super::m_memberPointer).value(), value);
    }
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<ORMType, FieldType, typename std::enable_if<IsWCDBOptional<FieldType>::value && ColumnIsBLOBType<typename FieldType::ValueType>::value>::type> final
: public RuntimeNullAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeNullAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;
    using ValueType = typename FieldType::ValueType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeNullAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    ~RuntimeAccessor() override final = default;

    void setValue(ORMType &instance, const UnderlyingType &value) const override final
    {
        instance.*Super::m_memberPointer = ValueType();
        ColumnIsBLOBType<typename FieldType::ValueType>::setToUnderlyingType(
        (instance.*Super::m_memberPointer).value(), value);
    }
};

} //namespace WCDB
