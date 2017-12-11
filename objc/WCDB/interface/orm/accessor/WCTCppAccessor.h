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

#import <WCDB/WCTBaseAccessor.h>
#import <WCDB/abstract.h>

template <typename T, typename Enable = void>
struct ColumnIsCppType : public std::false_type {
};
template <typename T>
struct ColumnIsCppType<T,
                       typename std::enable_if<WCDB::ColumnInfo<T>::type !=
                                               WCDB::ColumnType::Null>::type>
    : public std::true_type {
};

template <WCTColumnType t>
class WCTCppAccessor : public WCTBaseAccessor {
public:
    using CType = typename WCDB::ColumnTypeInfo<(WCDB::ColumnType) t>::CType;
    using Setter = void (^)(InstanceType, CType);
    using Getter = CType (^)(InstanceType);

    WCTCppAccessor(Getter getter, Setter setter)
        : getValue(getter), setValue(setter)
    {
    }

    virtual WCTColumnType getColumnType() const override
    {
        return (WCTColumnType) t;
    };
    virtual WCTAccessorType getAccessorType() const override
    {
        return WCTAccessorCpp;
    }

    const Setter setValue;
    const Getter getValue;
};

template <>
class WCTCppAccessor<WCTColumnTypeBinary> : public WCTBaseAccessor {
public:
    using SizeType = typename WCDB::ColumnTypeInfo<(
        WCDB::ColumnType) WCTColumnTypeBinary>::SizeType;
    using CType = typename WCDB::ColumnTypeInfo<(
        WCDB::ColumnType) WCTColumnTypeBinary>::CType;
    using Setter = void (^)(InstanceType, CType, SizeType);
    using Getter = CType (^)(InstanceType, SizeType);

    WCTCppAccessor(Getter getter, Setter setter)
        : getValue(getter), setValue(setter)
    {
    }

    virtual WCTColumnType getColumnType() const override
    {
        return WCTColumnTypeBinary;
    }
    virtual WCTAccessorType getAccessorType() const override
    {
        return WCTAccessorCpp;
    }

    const Setter setValue;
    const Getter getValue;
};
