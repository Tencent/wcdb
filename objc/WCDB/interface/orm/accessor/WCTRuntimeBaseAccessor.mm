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

#import <WCDB/WCTRuntimeBaseAccessor.h>
#import <WCDB/error.hpp>
#import <objc/runtime.h>
#import <string>

#ifndef WCDB_COCOAPODS
#if __has_feature(objc_arc)
#error This file should be compiled without ARC to get better performance. Please use -fno-objc-arc flag on this file.
#endif
#endif

SEL WCTRuntimeBaseAccessor::GetGetterSelector(Class cls, const std::string &propertyName)
{
    objc_property_t objcProperty = class_getProperty(cls, propertyName.c_str());
    const char *getter = property_copyAttributeValue(objcProperty, "G");
    if (!getter) {
        getter = propertyName.c_str();
    }
    return sel_registerName(getter);
}

SEL WCTRuntimeBaseAccessor::GetSetterSelector(Class cls, const std::string &propertyName)
{
    objc_property_t objcProperty = class_getProperty(cls, propertyName.c_str());
    const char *setter = property_copyAttributeValue(objcProperty, "S");
    if (setter) {
        return sel_registerName(setter);
    }
    std::string defaultSetter = "set" + propertyName + ":";
    defaultSetter[3] = std::toupper(propertyName[0]);
    return sel_registerName(defaultSetter.c_str());
}

IMP WCTRuntimeBaseAccessor::GetClassMethodImplementation(Class cls, SEL selector)
{
    return [cls methodForSelector:selector];
}

IMP WCTRuntimeBaseAccessor::GetInstanceMethodImplementation(Class cls, SEL selector)
{
    return [cls instanceMethodForSelector:selector];
}

Class WCTRuntimeBaseAccessor::GetPropertyClass(Class cls, const std::string &propertyName)
{
    objc_property_t property = class_getProperty(cls, propertyName.c_str());
    NSString *attributes = [NSString stringWithUTF8String:property_getAttributes(property)];
    NSArray *splitAttributes = [attributes componentsSeparatedByString:@","];
    if (splitAttributes.count > 0) {
        NSString *encodeType = splitAttributes[0];
        NSArray *splitEncodeTypes = [encodeType componentsSeparatedByString:@"\""];
        if (splitEncodeTypes.count <= 1) {
            WCDB::Error::Abort(("Failed to parse the type of [" + propertyName + "]").c_str());
        }
        NSString *className = splitEncodeTypes[1];
        return NSClassFromString(className);
    }
    return nil;
}
