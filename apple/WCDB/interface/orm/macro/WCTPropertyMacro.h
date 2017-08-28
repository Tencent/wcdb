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

#define __WCDB_SYNTHESIZE_IMP(className, propertyName, columnName)             \
    +(const WCTProperty &) propertyName                                        \
    {                                                                          \
        static const WCTProperty s_property(                                   \
            columnName, className.class,                                       \
            __WCDB_BINDING(className)                                          \
                .addColumnBinding<__WCDB_PROPERTY_TYPE(                        \
                    className, propertyName)>(WCDB_STRINGIFY(propertyName),    \
                                              columnName));                    \
        return s_property;                                                     \
    }                                                                          \
    static const auto UNUSED_UNIQUE_ID = [](WCTPropertyList &propertyList) {   \
        propertyList.push_back(className.propertyName);                        \
        return nullptr;                                                        \
    }(__WCDB_PROPERTIES(className));

#define __WCDB_SYNTHESIZE_DEFAULT_IMP(className, propertyName, columnName,     \
                                      defaultValue)                            \
    __WCDB_SYNTHESIZE_IMP(className, propertyName, columnName)                 \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding->getColumnBinding(className.propertyName)                      \
            ->makeDefault<__WCDB_PROPERTY_TYPE(className, propertyName)>(      \
                defaultValue);                                                 \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));
