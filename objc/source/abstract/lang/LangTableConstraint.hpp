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

#ifndef LangTableConstraint_hpp
#define LangTableConstraint_hpp

#include <WCDB/lang_common.h>

namespace WCDB {

namespace lang {

class TableConstraint : public Lang {
public:
    TableConstraint();

    copy_on_write_string name;
    enum class Type : int {
        NotSet,
        PrimaryKey,
        Unique,
        Check,
        ForeignKey,
    };
    Type type;

    copy_on_write_lazy_lang_list<IndexedColumn> indexedColumns;
    ConflictClause conflictClause;

    copy_on_write_lazy_lang<Expr> expr;
    copy_on_write_lazy_lang_list<Column> columns;
    copy_on_write_lazy_lang<ForeignKeyClause> foreignKeyClause;

    virtual copy_on_write_string SQL() const override;
};

} // namespace lang

} // namespace WCDB

#endif /* LangTableConstraint_hpp */
