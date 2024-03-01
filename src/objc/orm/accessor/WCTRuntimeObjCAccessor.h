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

#import "WCTObjCAccessor.h"
#import "WCTRuntimeBaseAccessor.h"
#import <objc/runtime.h>

class WCDB_API WCTRuntimeObjCAccessor final : public WCTRuntimeAccessor<id>,
                                              public WCTObjCAccessor {
protected:
    using InstanceType = WCTObjCAccessor::InstanceType;
    using PropertyType = NSObject *; //NSObject<WCTColumnCoding>*

public:
    WCTRuntimeObjCAccessor(Class instanceClass, const WCDB::UnsafeStringView &propertyName, Class propCls = nil) __attribute__((always_inline));

    ~WCTRuntimeObjCAccessor() override;

    void setObject(InstanceType instance, OCType value) override final;
    OCType getObject(InstanceType instance) override final;

protected:
    WCDB::ColumnType getColumnType() const override final;

    Class m_propertyClass;
    IMP m_impForArchiveSelector;
    IMP m_impForUnarchiveSelector;
    const WCDB::ColumnType m_columnType;

    static WCDB::ColumnType GetColumnType(Class propertyClass);
    static SEL archiveSelector();
    static SEL unarchiveSelector();
};
