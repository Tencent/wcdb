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

#import <WCDB/WCTRuntimeObjCAccessor.h>
#import <WCDB/error.hpp>

#ifndef WCDB_COCOAPODS
#if __has_feature(objc_arc)
#error This file should be compiled without ARC to get better performance. Please use -fno-objc-arc flag on this file.
#endif
#endif

WCTRuntimeObjCAccessor::WCTRuntimeObjCAccessor(Class instanceClass, const std::string &propertyName)
    : WCTRuntimeAccessor<id>(instanceClass, propertyName)
    , WCTObjCAccessor(generateValueGetter(instanceClass, propertyName), generateValueSetter(instanceClass, propertyName))
    , m_columnType(GetColumnType(instanceClass, propertyName))
{
    Class propertyClass = GetPropertyClass(instanceClass, propertyName);
    if (![propertyClass conformsToProtocol:@protocol(WCTColumnCoding)]) {
        WCDB::Error::Abort([NSString stringWithFormat:@"Class %@ should conforms to protocol WCTColumnCoding", NSStringFromClass(propertyClass)].UTF8String);
    }
}

WCTRuntimeObjCAccessor::ValueGetter WCTRuntimeObjCAccessor::generateValueGetter(Class instanceClass, const std::string &propertyName)
{
    static const SEL ArchiveSelector = NSSelectorFromString(@"archivedWCTValue");
    Class propertyClass = GetPropertyClass(instanceClass, propertyName);
    IMP implementation = GetInstanceMethodImplementation(propertyClass, ArchiveSelector);
    auto block = ^(InstanceType instance) {
      using Archiver = OCType (*)(InstanceType, SEL);
      PropertyType property = getProperty(instance);
      OCType value = property ? ((Archiver) implementation)(property, ArchiveSelector) : nil;
      return value;
    };
    return [block copy];
}

WCTRuntimeObjCAccessor::ValueSetter WCTRuntimeObjCAccessor::generateValueSetter(Class instanceClass, const std::string &propertyName)
{
    static const SEL UnarchiveSelector = NSSelectorFromString(@"unarchiveWithWCTValue:");
    Class propertyClass = GetPropertyClass(instanceClass, propertyName);
    IMP implementation = GetClassMethodImplementation(propertyClass, UnarchiveSelector);
    auto block = ^(InstanceType instance, OCType value) {
      using Unarchiver = PropertyType (*)(Class, SEL, OCType);
      if (instance) {
          PropertyType property = ((Unarchiver) implementation)(propertyClass, UnarchiveSelector, value);
          setProperty(instance, property);
      }
    };
    return [block copy];
}

WCTColumnType WCTRuntimeObjCAccessor::GetColumnType(Class instanceClass, const std::string &propertyName)
{
    static const SEL ColumnTypeSelector = NSSelectorFromString(@"columnTypeForWCDB");
    Class propertyClass = GetPropertyClass(instanceClass, propertyName);
    if (![propertyClass conformsToProtocol:@protocol(WCTColumnCoding)]) {
        WCDB::Error::Abort([NSString stringWithFormat:@"[%@] should conform to WCTColumnCoding protocol, which is the class of [%@ %s]", NSStringFromClass(propertyClass), NSStringFromClass(instanceClass), propertyName.c_str()].UTF8String);
    }
    IMP implementation = GetClassMethodImplementation(propertyClass, ColumnTypeSelector);
    using GetColumnTyper = WCTColumnType (*)(Class, SEL);
    return ((GetColumnTyper) implementation)(propertyClass, ColumnTypeSelector);
}

WCTColumnType WCTRuntimeObjCAccessor::getColumnType() const
{
    return m_columnType;
}
