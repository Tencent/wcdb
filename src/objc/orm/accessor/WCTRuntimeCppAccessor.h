//
// Created by sanhuazhang on 2019/05/02
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

#import "WCTCppAccessor.h"
#import "WCTRuntimeBaseAccessor.h"

template<typename PropertyType, typename Enable = void>
class WCTRuntimeCppAccessor final {};

template<typename PropertyType>
class WCTRuntimeCppAccessor<PropertyType, typename std::enable_if<WCDB::ColumnInfo<PropertyType>::isBaseType>::type> final
: public WCTRuntimeAccessor<PropertyType>,
  public WCTCppAccessor<(WCDB::ColumnType) WCDB::ColumnInfo<PropertyType>::type> {
    static_assert(std::is_fundamental<PropertyType>::value || std::is_enum<PropertyType>::value,
                  "Only fundamental C/Cpp types is allowed. For other types, use ObjC types.");

protected:
    using CppAccessor = WCTCppAccessor<WCDB::ColumnInfo<PropertyType>::type>;
    using RuntimeAccessor = WCTRuntimeAccessor<PropertyType>;
    using InstanceType = typename RuntimeAccessor::InstanceType;
    using UnderlyingType = typename CppAccessor::UnderlyingType;

public:
    WCTRuntimeCppAccessor(Class cls, const WCDB::UnsafeStringView &propertyName)
    : RuntimeAccessor(cls, propertyName), CppAccessor()
    {
    }

    ~WCTRuntimeCppAccessor() override final = default;

    void setValue(InstanceType instance, UnderlyingType value) override final
    {
        this->setProperty(instance, (PropertyType) value);
    }

    UnderlyingType getValue(InstanceType instance) override final
    {
        return (UnderlyingType) this->getProperty(instance);
    }
};
