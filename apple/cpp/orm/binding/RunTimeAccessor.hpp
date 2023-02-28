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

#include <WCDB/Accessor.hpp>

namespace WCDB {

template<class ORMType, typename FieldType, typename Enable = void>
class RuntimeAccessor final : public RuntimeGetterAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeGetterAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeGetterAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    virtual ~RuntimeAccessor() override final = default;

    void setValue(ORMType &instance, const UnderlyingType &value) const override
    {
        instance.*Super::m_memberPointer = (FieldType) value;
    }
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<ORMType, FieldType, typename std::enable_if<ColumnIsTextType<FieldType>::value && !std::is_array<FieldType>::value>::type> final
: public RuntimeGetterAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeGetterAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeGetterAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    ~RuntimeAccessor() override final = default;

    void setValue(ORMType &instance, const UnderlyingType &value) const override
    {
        instance.*Super::m_memberPointer
        = ColumnIsTextType<FieldType>::fromUnderlyingType(value);
    }
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<ORMType,
                      FieldType,
                      typename std::enable_if<ColumnIsTextType<FieldType>::value && std::is_array<FieldType>::value
                                              && !std::is_const<FieldType>::value>::type>
final : public RuntimeGetterAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeGetterAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeGetterAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    ~RuntimeAccessor() override final = default;

    void setValue(ORMType &instance, const UnderlyingType &value) const override
    {
        size_t size = std::min(value.size() + 1, sizeof(FieldType) / sizeof(char));
        memcpy(instance.*Super::m_memberPointer, value.data(), size);
    }
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<ORMType,
                      FieldType,
                      typename std::enable_if<ColumnIsTextType<FieldType>::value && std::is_array<FieldType>::value
                                              && std::is_const<FieldType>::value>::type>
final : public RuntimeGetterAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeGetterAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeGetterAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    ~RuntimeAccessor() override final = default;

    void setValue(ORMType &, const UnderlyingType &) const override {}
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<ORMType, FieldType, typename std::enable_if<ColumnIsBLOBType<FieldType>::value && !std::is_array<FieldType>::value>::type> final
: public RuntimeGetterAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeGetterAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeGetterAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    ~RuntimeAccessor() override final = default;

    void setValue(ORMType &instance, const UnderlyingType &value) const override
    {
        instance.*Super::m_memberPointer
        = ColumnIsBLOBType<FieldType>::fromUnderlyingType(value);
    }
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<ORMType,
                      FieldType,
                      typename std::enable_if<ColumnIsBLOBType<FieldType>::value && std::is_array<FieldType>::value
                                              && !std::is_const<FieldType>::value>::type>
final : public RuntimeGetterAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeGetterAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeGetterAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    ~RuntimeAccessor() override final = default;

    void setValue(ORMType &instance, const UnderlyingType &value) const override
    {
        size_t size = std::min(value.size() + 1, sizeof(FieldType) / sizeof(char));
        memcpy(instance.*Super::m_memberPointer, value.buffer(), size);
    }
};

template<class ORMType, typename FieldType>
class RuntimeAccessor<ORMType,
                      FieldType,
                      typename std::enable_if<ColumnIsBLOBType<FieldType>::value && std::is_array<FieldType>::value
                                              && std::is_const<FieldType>::value>::type>
final : public RuntimeGetterAccessor<ORMType, FieldType> {
private:
    using Super = RuntimeGetterAccessor<ORMType, FieldType>;
    using UnderlyingType = typename Super::UnderlyingType;

public:
    RuntimeAccessor(FieldType ORMType::*memberPointer)
    : RuntimeGetterAccessor<ORMType, FieldType>(memberPointer)
    {
    }
    ~RuntimeAccessor() override final = default;

    void setValue(ORMType &, const UnderlyingType &) const override {}
};

} //namespace WCDB
