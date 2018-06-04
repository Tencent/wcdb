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

#import <WCDB/Assertion.hpp>
#import <WCDB/Error.hpp>
#import <WCDB/NSString+CppString.h>
#import <WCDB/WCTRuntimeObjCAccessor.h>

#if __has_feature(objc_arc)
#error This file should be compiled without ARC to get better performance. Please use -fno-objc-arc flag on this file.
#endif

WCTRuntimeObjCAccessor::WCTRuntimeObjCAccessor(Class instanceClass, const std::string &propertyName)
    : WCTRuntimeAccessor<id>(instanceClass, propertyName)
    , WCTObjCAccessor(generateValueGetter(instanceClass, propertyName), generateValueSetter(instanceClass, propertyName))
    , m_columnType(GetColumnType(instanceClass, propertyName))
{
    Class propertyClass = getPropertyClass(instanceClass, propertyName);
    WCTAssert([propertyClass conformsToProtocol:@protocol(WCTColumnCoding)], "Class %@ should conforms to protocol WCTColumnCoding.");
}

WCTRuntimeObjCAccessor::ValueGetter WCTRuntimeObjCAccessor::generateValueGetter(Class instanceClass, const std::string &propertyName)
{
    static const SEL archiveSelector = NSSelectorFromString(@"archivedWCTValue");
    Class propertyClass = getPropertyClass(instanceClass, propertyName);
    IMP implementation = getInstanceMethodImplementation(propertyClass, archiveSelector);
    auto block = ^(InstanceType instance) {
      using Archiver = OCType (*)(InstanceType, SEL);
      PropertyType property = getProperty(instance);
      OCType value = property ? ((Archiver) implementation)(property, archiveSelector) : nil;
      return value;
    };
    return [block copy];
}

WCTRuntimeObjCAccessor::ValueSetter WCTRuntimeObjCAccessor::generateValueSetter(Class instanceClass, const std::string &propertyName)
{
    static const SEL unarchiveSelector = NSSelectorFromString(@"unarchiveWithWCTValue:");
    Class propertyClass = getPropertyClass(instanceClass, propertyName);
    IMP implementation = getClassMethodImplementation(propertyClass, unarchiveSelector);
    auto block = ^(InstanceType instance, OCType value) {
      using Unarchiver = PropertyType (*)(Class, SEL, OCType);
      if (instance) {
          PropertyType property = ((Unarchiver) implementation)(propertyClass, unarchiveSelector, value);
          setProperty(instance, property);
      }
    };
    return [block copy];
}

WCDB::ColumnType WCTRuntimeObjCAccessor::GetColumnType(Class instanceClass, const std::string &propertyName)
{
    static const SEL columnTypeSelector = NSSelectorFromString(@"columnType");
    Class propertyClass = getPropertyClass(instanceClass, propertyName);
#ifdef DEBUG
    if (![propertyClass conformsToProtocol:@protocol(WCTColumnCoding)]) {
        NSString *message = [NSString stringWithFormat:@"[%@] should conform to WCTColumnCoding protocol, which is the class of [%@ %s]", NSStringFromClass(propertyClass), NSStringFromClass(instanceClass), propertyName.c_str()];
        WCTFatalError(message.cppString);
    }
#endif
    IMP implementation = getClassMethodImplementation(propertyClass, columnTypeSelector);
    using GetColumnTyper = WCDB::ColumnType (*)(Class, SEL);
    return ((GetColumnTyper) implementation)(propertyClass, columnTypeSelector);
}

WCDB::ColumnType WCTRuntimeObjCAccessor::getColumnType() const
{
    return m_columnType;
}
