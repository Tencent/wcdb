//
// Created by qiuwenchen on 2022/9/7.
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

#include "Accessor.hpp"

namespace WCDB {

template<class ORMType, typename FieldType, typename Enable = void>
class BaseRuntimeAccessor
: public Accessor<ORMType, (ColumnType) ColumnInfo<FieldType>::type> {
protected:
    using Super = Accessor<ORMType, (ColumnType) ColumnInfo<FieldType>::type>;
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
        return (UnderlyingType) (instance.*m_memberPointer);
    }
};

template<class ORMType, typename FieldType, typename Enable = void>
class RuntimeNullAccessor : public BaseRuntimeAccessor<ORMType, FieldType> {
protected:
    using Super = BaseRuntimeAccessor<ORMType, FieldType>;

public:
    RuntimeNullAccessor(FieldType ORMType::*memberPointer)
    : Super(memberPointer)
    {
    }
    virtual ~RuntimeNullAccessor() override = default;

    bool isNull(const ORMType &) const override final { return false; }

    void setNull(ORMType &) const override final {}
};

template<class ORMType, typename FieldType>
class RuntimeNullAccessor<ORMType,
                          FieldType,
                          typename std::enable_if<std::is_integral<FieldType>::value || std::is_enum<FieldType>::value
                                                  || std::is_floating_point<FieldType>::value>::type>
: public BaseRuntimeAccessor<ORMType, FieldType> {
protected:
    using Super = BaseRuntimeAccessor<ORMType, FieldType>;

public:
    RuntimeNullAccessor(FieldType ORMType::*memberPointer)
    : Super(memberPointer)
    {
    }
    virtual ~RuntimeNullAccessor() override = default;

    bool isNull(const ORMType &) const override final { return false; }

    void setNull(ORMType &instance) const override final
    {
        instance.*Super::m_memberPointer = (FieldType) NULL;
    }
};

template<class ORMType, typename FieldType>
class RuntimeNullAccessor<ORMType, FieldType, typename std::enable_if<std::is_pointer<FieldType>::value>::type>
: public BaseRuntimeAccessor<ORMType, FieldType> {
protected:
    using Super = BaseRuntimeAccessor<ORMType, FieldType>;

public:
    RuntimeNullAccessor(FieldType ORMType::*memberPointer)
    : Super(memberPointer)
    {
    }
    virtual ~RuntimeNullAccessor() override = default;

    bool isNull(const ORMType &instance) const override final
    {
        return instance.*Super::m_memberPointer == nullptr;
    }

    void setNull(ORMType &instance) const override final
    {
        instance.*Super::m_memberPointer = nullptr;
    }
};

template<class ORMType, typename FieldType>
class RuntimeNullAccessor<ORMType, FieldType, typename std::enable_if<std::is_array<FieldType>::value && !std::is_const<FieldType>::value>::type>
: public BaseRuntimeAccessor<ORMType, FieldType> {
protected:
    using Super = BaseRuntimeAccessor<ORMType, FieldType>;

public:
    RuntimeNullAccessor(FieldType ORMType::*memberPointer)
    : Super(memberPointer)
    {
    }
    virtual ~RuntimeNullAccessor() override = default;

    bool isNull(const ORMType &instance) const override final
    {
        return instance.*Super::m_memberPointer == nullptr;
    }

    void setNull(ORMType &instance) const override final
    {
        size_t length = sizeof(FieldType) / sizeof(char);
        memset(instance.*Super::m_memberPointer, 0, length);
    }
};

template<class ORMType, typename FieldType, typename Enable = void>
class RuntimeAccessor final : public RuntimeNullAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeNullAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeNullAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    virtual ~RuntimeAccessor() override final = default;

    void setValue(ORMType &instance, const UnderlyingType &value) const override final
    {
        instance.*Super::m_memberPointer = (FieldType) value;
    }
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<ORMType, FieldType, typename std::enable_if<ColumnIsTextType<FieldType>::value>::type> final
: public RuntimeNullAccessor<ORMType, FieldType> {
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
        ColumnIsTextType<FieldType>::setToUnderlyingType(
        instance.*Super::m_memberPointer, value);
    }
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<ORMType, FieldType, typename std::enable_if<ColumnIsBLOBType<FieldType>::value>::type> final
: public RuntimeNullAccessor<ORMType, FieldType> {
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
        ColumnIsBLOBType<FieldType>::setToUnderlyingType(
        instance.*Super::m_memberPointer, value);
    }
};

} //namespace WCDB
