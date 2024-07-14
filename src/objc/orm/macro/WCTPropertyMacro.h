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

#include "Macro.h"

// Property
#define WCDB_ORM_TYPER __wcdbtyper

#define __WCDB_PROPERTY_TYPE(className, propertyName) \
    decltype([className WCDB_ORM_TYPER].propertyName)

#ifdef __cplusplus
#define __WCDB_PROPERTY_IMP(propertyName) +(const WCTProperty &) propertyName;
#elif WCDB_SWIFT
#define __WCDB_PROPERTY_IMP(propertyName) +(WCTBridgeProperty *) swift_##propertyName NS_SWIFT_NAME(propertyName());
#else
#define __WCDB_PROPERTY_IMP(propertyName)
#endif

// Synthesize
#define WCDB_ORM_TYPE_SYNTHESIZE synthesize

// __wcdb_className_synthesize_uniqueID
#define __WCDB_SYNTHESIZE_IMP(WINQName, propertyName, columnName)                                                                                                                                               \
    +(const WCTProperty &) WINQName                                                                                                                                                                             \
    {                                                                                                                                                                                                           \
        static const char *name = columnName;                                                                                                                                                                   \
        static const WCTProperty *s_property = new WCTProperty(WCDB::StringView::makeConstant(name), WCTColumnBinding::generate<__WCDB_PROPERTY_TYPE(self, propertyName)>(self, WCDB_STRINGIFY(propertyName))); \
        WCTBinding::assertNoInheritance(*s_property, self);                                                                                                                                                     \
        return *s_property;                                                                                                                                                                                     \
    }                                                                                                                                                                                                           \
    +(const WCTProperty &) WCDB_ORM_UNIQUE(WCDB_ORM_TYPE_SYNTHESIZE)                                                                                                                                            \
    {                                                                                                                                                                                                           \
        WCDB_UNUSED(binding);                                                                                                                                                                                   \
        return self.WINQName;                                                                                                                                                                                   \
    }                                                                                                                                                                                                           \
    +(WCTBridgeProperty *) swift_##WINQName                                                                                                                                                                     \
    {                                                                                                                                                                                                           \
        return [WCTBridgeProperty creatBridgeProperty:[self WINQName]];                                                                                                                                         \
    }
