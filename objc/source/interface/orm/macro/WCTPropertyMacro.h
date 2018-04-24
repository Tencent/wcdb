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

#define __WCDB_PROPERTY_TYPE(className, propertyName)                          \
    decltype([className new].propertyName)

#define __WCDB_PROPERTY_IMP(propertyName) +(const WCTProperty &) propertyName;

#define __WCDB_PROPERTY_NAME(className, propertyName)                          \
    _s_##className##_##propertyName##_property

#define __WCDB_SYNTHESIZE_IMP(className, propertyName, columnName)             \
    static const WCTProperty &__WCDB_PROPERTY_NAME(className, propertyName) =  \
        [className objectRelationalMappingForWCDB]                             \
            ->addColumnBinding<__WCDB_PROPERTY_TYPE(className, propertyName)>( \
                WCDB_STRINGIFY(propertyName), columnName);                     \
    +(const WCTProperty &) propertyName                                        \
    {                                                                          \
        return __WCDB_PROPERTY_NAME(className, propertyName);                  \
    }

#define __WCDB_SYNTHESIZE_DEFAULT_IMP(className, propertyName, columnName,     \
                                      defaultValue)                            \
    __WCDB_SYNTHESIZE_IMP(className, propertyName, columnName)                 \
    static const auto WCDB_UNUSED_UNIQUE_NAME = []() {                         \
        [className objectRelationalMappingForWCDB]->addColumnConstraint(       \
            WCDB::ColumnConstraint().withDefaultValue(defaultValue),           \
            className.propertyName);                                           \
        return nullptr;                                                        \
    }();
