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

#import "WCTCommon.h"
#import <functional>
#import <objc/runtime.h>

class WCDB_API WCTRuntimeBaseAccessor {
public:
    WCTRuntimeBaseAccessor(Class cls, const WCDB::UnsafeStringView &propertyName);

    virtual ~WCTRuntimeBaseAccessor();

protected:
    SEL m_selForGetter;
    SEL m_selForSetter;
    IMP m_impForGetter;
    IMP m_impForSetter;

    using InstanceType = id;

    static Class getPropertyClass(Class cls, const WCDB::UnsafeStringView &propertyName);

    static SEL getGetterSelector(objc_property_t property,
                                 const WCDB::UnsafeStringView &propertyName);
    static SEL getSetterSelector(objc_property_t property,
                                 const WCDB::UnsafeStringView &propertyName);
    static IMP getClassMethodImplementation(Class cls, SEL selector);
    static IMP getInstanceMethodImplementation(Class cls, SEL selector);

private:
    WCTRuntimeBaseAccessor(Class cls,
                           const WCDB::UnsafeStringView &propertyName,
                           objc_property_t property);
};

template<typename PropertyType>
class WCTRuntimeAccessor : public WCTRuntimeBaseAccessor {
public:
    using WCTRuntimeBaseAccessor::WCTRuntimeBaseAccessor;

    virtual ~WCTRuntimeAccessor() override = default;

    void setProperty(InstanceType instance, PropertyType value)
    {
        using IMPSetter = void (*)(InstanceType, SEL, PropertyType);
        return ((IMPSetter) m_impForSetter)(instance, m_selForSetter, value);
    }

    PropertyType getProperty(InstanceType instance)
    {
        using IMPGetter = PropertyType (*)(id, SEL);
        return ((IMPGetter) m_impForGetter)(instance, m_selForGetter);
    }
};
