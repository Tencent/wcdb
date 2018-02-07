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

class LangTableOrSubqueryBase : public Lang {
};
class LangTableOrSubqueryTable;
class LangTableOrSubqueryTableFunction;
class LangTableOrSubqueryJoinClause;
class LangTableOrSubquerySelect;
class LangTableOrSubqueryList;

class LangTableOrSubqueryTable : public LangTableOrSubqueryBase {
public:
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

class LangTableOrSubqueryTableFunction : public LangTableOrSubqueryBase {
public:
    copy_on_write_string schemaName;
    copy_on_write_string tableFunctionName;
    copy_on_write_string tableAlias;
    copy_on_write_lazy_lang_list<LangExpr> exprs;

    virtual copy_on_write_string SQL() const override;
};

class LangTableOrSubqueryJoinClause : public LangTableOrSubqueryBase {
public:
    copy_on_write_lazy_lang<LangJoinClause> joinClause;

    virtual copy_on_write_string SQL() const override;
};

class LangTableOrSubquerySelect : public LangTableOrSubqueryBase {
public:
    copy_on_write_lazy_lang<LangSelectSTMT> selectSTMT;
    copy_on_write_string tableAlias;

    virtual copy_on_write_string SQL() const override;
};

class LangTableOrSubqueryList : public LangTableOrSubqueryBase {
public:
    copy_on_write_lazy_lang_list<LangTableOrSubqueryBase> tableOrSubquerys;

    virtual copy_on_write_string SQL() const override;
};

class LangTableOrSubquery : public LangTableOrSubqueryBase {
public:
    enum class Switch : int {
        Table,
        TableFunction,
        JoinClause,
        Select,
        List,
    };
    Switch switcher;
    copy_on_write_lazy_lang<LangTableOrSubqueryTable> tableOrSubqueryTable;
    copy_on_write_lazy_lang<LangTableOrSubqueryTableFunction>
        tableOrSubqueryTableFunction;
    copy_on_write_lazy_lang<LangTableOrSubqueryJoinClause>
        tableOrSubqueryJoinClause;
    copy_on_write_lazy_lang<LangTableOrSubquerySelect> tableOrSubquerySelect;
    copy_on_write_lazy_lang<LangTableOrSubqueryList> tableOrSubqueryList;

    virtual copy_on_write_string SQL() const override;
};

#endif /* LangTableOrSubquery_hpp */
