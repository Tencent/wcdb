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
#import <WCDB/Console.hpp>
#import <WCDB/Core.h>
#import <WCDB/StringView.hpp>
#import <WCDB/WCTRuntimeBaseAccessor.h>
#import <objc/runtime.h>

WCTRuntimeBaseAccessor::WCTRuntimeBaseAccessor(Class cls, const WCDB::UnsafeStringView &propertyName)
: WCTRuntimeBaseAccessor(cls, propertyName, class_getProperty(cls, propertyName.data()))
{
}

WCTRuntimeBaseAccessor::WCTRuntimeBaseAccessor(Class cls, const WCDB::UnsafeStringView &propertyName, objc_property_t property)
: m_selForGetter(getGetterSelector(property, propertyName))
, m_selForSetter(getSetterSelector(property, propertyName))
, m_impForGetter(getInstanceMethodImplementation(cls, m_selForGetter))
, m_impForSetter(getInstanceMethodImplementation(cls, m_selForSetter))
{
}

WCTRuntimeBaseAccessor::~WCTRuntimeBaseAccessor() = default;

SEL WCTRuntimeBaseAccessor::getGetterSelector(objc_property_t property, const WCDB::UnsafeStringView &propertyName)
{
    WCTAssert(property != nil);
    WCTAssert(!propertyName.empty());

    SEL selector = nil;
    char *getter = property_copyAttributeValue(property, "G");
    if (getter != nullptr) {
        selector = sel_registerName(getter);
        free(getter);
    } else {
        selector = sel_registerName(propertyName.data());
    }
    WCTAssert(selector != nil);
    return selector;
}

SEL WCTRuntimeBaseAccessor::getSetterSelector(objc_property_t property, const WCDB::UnsafeStringView &propertyName)
{
    WCTAssert(property != nil);
    WCTAssert(!propertyName.empty());

    SEL selector = nil;
    char *setter = property_copyAttributeValue(property, "S");
    if (setter != nullptr) {
        selector = sel_registerName(setter);
        free(setter);
    } else {
        std::ostringstream stream;
        stream << "set" << propertyName << ":";
        std::string defaultSetter = stream.str();
        defaultSetter[3] = (char) std::toupper(defaultSetter[3]);
        selector = sel_registerName(defaultSetter.data());
    }
    WCTAssert(selector != nil);
    return selector;
}

IMP WCTRuntimeBaseAccessor::getClassMethodImplementation(Class cls, SEL selector)
{
    WCTAssert(cls != nil);
    WCTAssert(selector != nil);
    return [cls methodForSelector:selector];
}

IMP WCTRuntimeBaseAccessor::getInstanceMethodImplementation(Class cls, SEL selector)
{
    WCTAssert(cls != nil);
    WCTAssert(selector != nil);
    return [cls instanceMethodForSelector:selector];
}

Class WCTRuntimeBaseAccessor::getPropertyClass(Class cls, const WCDB::UnsafeStringView &propertyName)
{
    WCTAssert(cls != nil);
    WCTAssert(!propertyName.empty());

    objc_property_t property = class_getProperty(cls, propertyName.data());
    NSString *attributes = [NSString stringWithUTF8String:property_getAttributes(property)];
    NSArray *splitAttributes = [attributes componentsSeparatedByString:@","];
    if (splitAttributes.count > 0) {
        NSString *encodeType = splitAttributes[0];
        NSArray *splitEncodeTypes = [encodeType componentsSeparatedByString:@"\""];
        WCTRemedialAssert(splitEncodeTypes.count > 1, WCDB::StringView::formatted("Failed to parse the type of [%s].", propertyName.data()), return nil;);
        NSString *className = splitEncodeTypes[1];
        return NSClassFromString(className);
    }
    return nil;
}
