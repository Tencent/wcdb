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

#define __WCDB_INDEX_IMP(className, indexSubfixName, propertyName, order,      \
                         unique)                                               \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding->getOrCreateIndexBinding(indexSubfixName)                      \
            ->addIndex(className.propertyName.index(order));                   \
        WCDB_IF(unique, binding->getOrCreateIndexBinding(indexSubfixName)      \
                            ->setUnique(true);)                                \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));

#define __WCDB_VIRTUAL_TABLE_ARGUMENT_IMP(className, left, right)              \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding->addVirtualTableArgument(left, right);                         \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));

#define __WCDB_VIRTUAL_TABLE_MODULE_IMP(className, moduleName)                 \
    static const auto UNUSED_UNIQUE_ID = [](WCTBinding *binding) {             \
        binding->setVirtualTableModule(moduleName);                            \
        return nullptr;                                                        \
    }(&__WCDB_BINDING(className));
