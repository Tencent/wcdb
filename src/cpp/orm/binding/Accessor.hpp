//
// Created by qiuwenchen on 2022/8/26.
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
#include "MemberPointer.hpp"
#include "WCDBError.hpp"

namespace WCDB {

class BaseAccessor {
public:
    BaseAccessor();
    virtual ~BaseAccessor() = 0;
    virtual ColumnType getColumnType() const = 0;
};

template<class O, WCDB::ColumnType t>
class Accessor : public BaseAccessor {
protected:
    using UnderlyingType = typename WCDB::ColumnTypeInfo<t>::UnderlyingType;
    using ORMType = O;

public:
    virtual ~Accessor() override = default;
    ColumnType getColumnType() const override final { return t; };
    virtual void setValue(ORMType& instance, const UnderlyingType& value) const = 0;
    virtual UnderlyingType getValue(const ORMType& instance) const = 0;
};

template<class ORMType, typename FieldType>
class RuntimeGetterAccessor
: public Accessor<ORMType, (ColumnType) ColumnInfo<FieldType>::type> {
protected:
    using Super = Accessor<ORMType, (ColumnType) ColumnInfo<FieldType>::type>;
    using UnderlyingType = typename Super::UnderlyingType;

    FieldType ORMType::*m_memberPointer;

public:
    RuntimeGetterAccessor(FieldType ORMType::*memberPointer)
    : m_memberPointer(memberPointer)
    {
        static_assert(IsFieldMemberPointer<FieldType ORMType::*>::value,
                      "Field should be initialize with a member field pointer, not a member function pointer");
    }
    virtual ~RuntimeGetterAccessor() override = default;

    UnderlyingType getValue(const ORMType& instance) const override final
    {
        return (UnderlyingType) (instance.*m_memberPointer);
    }
};

} // namespace WCDB
