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

#ifndef LangIndexedColumn_hpp
#define LangIndexedColumn_hpp

#include <WCDB/lang_common.h>

class LangIndexedColumn : public Lang {
public:
    enum class Switch : int {
        ColumnName,
        Expr,
    };
    Switch switcher;
    copy_on_write_string columnName;
    copy_on_write_lazy_lang<LangExpr> expr;
    copy_on_write_string collationName;
    LangOrder order;

    virtual copy_on_write_string SQL() const;
};

#endif /* LangIndexedColumn_hpp */
