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

#ifndef LangTableOrSubquery_hpp
#define LangTableOrSubquery_hpp

#include <WCDB/lang_common.h>

namespace WCDB {

namespace lang {

class TableOrSubqueryBase : public Lang {
};
class TableOrSubqueryTable;
class TableOrSubqueryTableFunction;
class TableOrSubqueryJoinClause;
class TableOrSubquerySelect;
class TableOrSubqueryList;

class TableOrSubqueryTable : public TableOrSubqueryBase {
public:
    TableOrSubqueryTable();

    copy_on_write_string schemaName;
    copy_on_write_string tableName;
    copy_on_write_string tableAlias;
    enum class IndexSwitch : int {
        NotSet,
        Indexed,
        NotIndexed,
    };
    IndexSwitch indexSwitcher;
    copy_on_write_string indexName;

    virtual copy_on_write_string SQL() const override;
};

class TableOrSubqueryTableFunction : public TableOrSubqueryBase {
public:
    copy_on_write_string schemaName;
    copy_on_write_string tableFunctionName;
    copy_on_write_string tableAlias;
    copy_on_write_lazy_lang_list<Expr> exprs;

    virtual copy_on_write_string SQL() const override;
};

class TableOrSubqueryJoinClause : public TableOrSubqueryBase {
public:
    copy_on_write_lazy_lang<JoinClause> joinClause;

    virtual copy_on_write_string SQL() const override;
};

class TableOrSubquerySelect : public TableOrSubqueryBase {
public:
    copy_on_write_lazy_lang<SelectSTMT> selectSTMT;
    copy_on_write_string tableAlias;

    virtual copy_on_write_string SQL() const override;
};

class TableOrSubqueryList : public TableOrSubqueryBase {
public:
    copy_on_write_lazy_lang_list<TableOrSubqueryBase> tableOrSubquerys;

    virtual copy_on_write_string SQL() const override;
};

class TableOrSubquery : public TableOrSubqueryBase {
public:
    TableOrSubquery();

    enum class Switch : int {
        NotSet,
        Table,
        TableFunction,
        JoinClause,
        Select,
        List,
    };
    Switch switcher;
    copy_on_write_lazy_lang<TableOrSubqueryTable> tableOrSubqueryTable;
    copy_on_write_lazy_lang<TableOrSubqueryTableFunction>
        tableOrSubqueryTableFunction;
    copy_on_write_lazy_lang<TableOrSubqueryJoinClause>
        tableOrSubqueryJoinClause;
    copy_on_write_lazy_lang<TableOrSubquerySelect> tableOrSubquerySelect;
    copy_on_write_lazy_lang<TableOrSubqueryList> tableOrSubqueryList;

    virtual copy_on_write_string SQL() const override;
};

} // namespace lang

} // namespace WCDB

#endif /* LangTableOrSubquery_hpp */
