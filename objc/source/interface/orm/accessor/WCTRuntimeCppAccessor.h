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

#import <WCDB/WCTCppAccessor.h>
#import <WCDB/WCTRuntimeBaseAccessor.h>

template<typename PropertyType, typename Enable = void>
class WCTRuntimeCppAccessor final {
};

template<typename PropertyType>
class WCTRuntimeCppAccessor<
PropertyType,
typename std::enable_if<WCDB::ColumnInfo<PropertyType>::isBaseType>::type>
: public WCTRuntimeAccessor<PropertyType>,
  public WCTCppAccessor<(
  WCDB::ColumnType) WCDB::ColumnInfo<PropertyType>::type> {
    static_assert(std::is_fundamental<PropertyType>::value, "Only fundamental C/Cpp types supported, for others, use ObjC types.");

protected:
    using CppAccessor = WCTCppAccessor<WCDB::ColumnInfo<PropertyType>::type>;
    using RuntimeAccessor = WCTRuntimeAccessor<PropertyType>;
    using InstanceType = typename RuntimeAccessor::InstanceType;
    using UnderlyingType = typename CppAccessor::UnderlyingType;
    using PropertyGetter = typename RuntimeAccessor::Getter;
    using PropertySetter = typename RuntimeAccessor::Setter;
    using ValueGetter = typename CppAccessor::Getter;
    using ValueSetter = typename CppAccessor::Setter;

public:
    WCTRuntimeCppAccessor(Class cls, const WCDB::String &propertyName)
    : RuntimeAccessor(cls, propertyName)
    , CppAccessor(generateValueGetter(), generateValueSetter())
    {
    }

protected:
    UnderlyingType convertPropertyTypeToCType(const PropertyType &property)
    {
        return (UnderlyingType) property;
    }

    PropertyType convertCTypeToPropertyType(UnderlyingType value)
    {
        return (PropertyType) value;
    }

    ValueGetter generateValueGetter()
    {
        return ^UnderlyingType(InstanceType instance) {
            return convertPropertyTypeToCType(this->getProperty(instance));
        };
    }

    ValueSetter generateValueSetter()
    {
        return ^(InstanceType instance, UnderlyingType value) {
            this->setProperty(instance, convertCTypeToPropertyType(value));
        };
    }
};
