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

#ifndef LangModuleArgument_hpp
#define LangModuleArgument_hpp

#include <WCDB/lang_common.h>

// What's the true syntax of LangModuleArgument?
class LangModuleArgument : public Lang {
public:
    enum class Type : int {
        ColumnDef,
        TableConstraint,
        LeftRight,
    };
    Type type;
    copy_on_write_lazy_lang<LangColumnDef> columnDef;
    copy_on_write_lazy_lang<LangTableConstraint> tableConstraint;
    copy_on_write_string left;
    copy_on_write_string right;

    virtual copy_on_write_string SQL() const override;
};

#endif /* LangModuleArgument_hpp */
