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

#import <Foundation/Foundation.h>
#import <WCDB/WCTCppAccessor.h>
#import <WCDB/WCTRuntimeBaseAccessor.h>

template <typename PropertyType, typename Enable = void>
class WCTRuntimeCppAccessor {
};

template <typename PropertyType>
class WCTRuntimeCppAccessor<
    PropertyType,
    typename std::enable_if<WCDB::ColumnInfo<PropertyType>::isBaseType>::type>
    : public WCTRuntimeAccessor<PropertyType>,
      public WCTCppAccessor<(
          WCTColumnType) WCDB::ColumnInfo<PropertyType>::type> {
protected:
    using CppAccessor =
        WCTCppAccessor<(WCTColumnType) WCDB::ColumnInfo<PropertyType>::type>;
    using RuntimeAccessor = WCTRuntimeAccessor<PropertyType>;
    using InstanceType = typename RuntimeAccessor::InstanceType;
    using CType = typename CppAccessor::CType;
    using PropertyGetter = typename RuntimeAccessor::Getter;
    using PropertySetter = typename RuntimeAccessor::Setter;
    using ValueGetter = typename CppAccessor::Getter;
    using ValueSetter = typename CppAccessor::Setter;

public:
    WCTRuntimeCppAccessor(Class cls, const std::string &propertyName)
        : RuntimeAccessor(cls, propertyName)
        , CppAccessor(generateValueGetter(), generateValueSetter())
    {
    }

protected:
    CType convertPropertyTypeToCType(const PropertyType &property)
    {
        return (CType) property;
    }

    PropertyType convertCTypeToPropertyType(CType value)
    {
        return (PropertyType) value;
    }

    ValueGetter generateValueGetter()
    {
        return ^(InstanceType instance) {
          return convertPropertyTypeToCType(this->getProperty(instance));
        };
    }

    ValueSetter generateValueSetter()
    {
        return ^(InstanceType instance, CType value) {
          this->setProperty(instance, convertCTypeToPropertyType(value));
        };
    }
};

template <typename PropertyType>
class WCTRuntimeCppAccessor<
    PropertyType,
    typename std::enable_if<WCDB::ColumnInfo<PropertyType>::isBLOB>::type>
    : public WCTRuntimeAccessor<PropertyType>,
      public WCTCppAccessor<WCDB::ColumnInfo<PropertyType>::type> {
public:
    using CppAccessor = WCTCppAccessor<WCDB::ColumnInfo<PropertyType>::type>;
    using RuntimeAccessor = WCTRuntimeAccessor<PropertyType>;
    using InstanceType = typename RuntimeAccessor::InstanceType;
    using CType = typename CppAccessor::CType;
    using SizeType = typename CppAccessor::SizeType;
    using PropertyGetter = typename RuntimeAccessor::Getter;
    using PropertySetter = typename RuntimeAccessor::Setter;
    using ValueGetter = typename CppAccessor::Getter;
    using ValueSetter = typename CppAccessor::Setter;

    WCTRuntimeCppAccessor(Class cls, const std::string &propertyName)
        : RuntimeAccessor(cls, propertyName)
        , CppAccessor(generateValueGetter(), generateValueSetter())
    {
    }

    // no default implementation for BLOB
    //    CType convertPropertyTypeToCType(const PropertyType& property, SizeType& size);
    //    PropertyType convertCTypeToPropertyType(CType value, SizeType size);

    ValueGetter generateValueGetter()
    {
        return ^(InstanceType instance, SizeType &size) {
          return convertPropertyTypeToCType(getProperty(instance), size);
        };
    }

    ValueSetter generateValueSetter()
    {
        return ^(InstanceType instance, CType value, SizeType size) {
          setProperty(instance, convertCTypeToPropertyType(value, size));
        };
    }
};
