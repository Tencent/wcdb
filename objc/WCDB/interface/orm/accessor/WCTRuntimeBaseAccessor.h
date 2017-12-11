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
#import <functional>

class WCTRuntimeBaseAccessor {
protected:
    using InstanceType = id;
    static SEL GetGetterSelector(Class cls, const std::string &propertyName);
    static SEL GetSetterSelector(Class cls, const std::string &propertyName);
    static IMP GetClassMethodImplementation(Class cls, SEL selector);
    static IMP GetInstanceMethodImplementation(Class cls, SEL selector);
    static Class GetPropertyClass(Class cls, const std::string &propertyName);
};

template <typename PropertyType>
class WCTRuntimeAccessor : public WCTRuntimeBaseAccessor {
public:
    using Setter = void (^)(InstanceType, PropertyType);
    using Getter = PropertyType (^)(InstanceType);

    WCTRuntimeAccessor(Class cls, const std::string &propertyName)
        : getProperty(GenerateGetter(cls, propertyName))
        , setProperty(GenerateSetter(cls, propertyName))
    {
    }

    const Setter setProperty;
    const Getter getProperty;

protected:
    Getter GenerateGetter(Class cls, const std::string &propertyName)
    {
        SEL selector = GetGetterSelector(cls, propertyName);
        IMP imp = GetInstanceMethodImplementation(cls, selector);
        auto block = ^(InstanceType instance) {
          using IMPGetter = PropertyType (*)(id, SEL);
          return ((IMPGetter) imp)(instance, selector);
        };
        return [block copy];
    }

    Setter GenerateSetter(Class cls, const std::string &propertyName)
    {
        SEL selector = GetSetterSelector(cls, propertyName);
        IMP imp = GetInstanceMethodImplementation(cls, selector);
        auto block = ^(InstanceType instance, PropertyType value) {
          using IMPSetter = void (*)(InstanceType, SEL, PropertyType);
          return ((IMPSetter) imp)(instance, selector, value);
        };
        return [block copy];
    }
};
