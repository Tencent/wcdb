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
#import <WCDB/WCTObjCAccessor.h>
#import <WCDB/WCTRuntimeBaseAccessor.h>
#import <objc/runtime.h>

class WCTRuntimeObjCAccessor : public WCTRuntimeAccessor<id>,
                               public WCTObjCAccessor {
protected:
    using InstanceType = WCTObjCAccessor::InstanceType;
    using PropertyType = NSObject *; //NSObject<WCTColumnCoding>*
    using PropertyGetter = WCTRuntimeAccessor<id>::Getter;
    using PropertySetter = WCTRuntimeAccessor<id>::Setter;
    using ValueGetter = WCTObjCAccessor::Getter;
    using ValueSetter = WCTObjCAccessor::Setter;

public:
    WCTRuntimeObjCAccessor(Class instanceClass,
                           const std::string &propertyName);

protected:
    ValueGetter generateValueGetter(Class instanceClass,
                                    const std::string &propertyName);
    ValueSetter generateValueSetter(Class instanceClass,
                                    const std::string &propertyName);

    virtual WCTColumnType getColumnType() const override;

    WCTColumnType GetColumnType(Class instanceClass,
                                const std::string &propertyName);

    const WCTColumnType m_columnType;
};
