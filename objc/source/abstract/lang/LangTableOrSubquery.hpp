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

#include <WCDB/LangCommon.h>

namespace WCDB {

namespace Lang {

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

    CopyOnWriteString schemaName;
    CopyOnWriteString tableName;
    CopyOnWriteString tableAlias;
    enum class IndexSwitch : int {
        NotSet,
        Indexed,
        NotIndexed,
    };
    IndexSwitch indexSwitcher;
    CopyOnWriteString indexName;

    virtual CopyOnWriteString SQL() const override;
};

class TableOrSubqueryTableFunction : public TableOrSubqueryBase {
public:
    CopyOnWriteString schemaName;
    CopyOnWriteString tableFunctionName;
    CopyOnWriteString tableAlias;
    CopyOnWriteLazyLangList<Expr> exprs;

    virtual CopyOnWriteString SQL() const override;
};

class TableOrSubqueryJoinClause : public TableOrSubqueryBase {
public:
    CopyOnWriteLazyLang<JoinClause> joinClause;

    virtual CopyOnWriteString SQL() const override;
};

class TableOrSubquerySelect : public TableOrSubqueryBase {
public:
    CopyOnWriteLazyLang<SelectSTMT> selectSTMT;
    CopyOnWriteString tableAlias;

    virtual CopyOnWriteString SQL() const override;
};

class TableOrSubqueryList : public TableOrSubqueryBase {
public:
    CopyOnWriteLazyLangList<TableOrSubqueryBase> tableOrSubquerys;

    virtual CopyOnWriteString SQL() const override;
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
    CopyOnWriteLazyLang<TableOrSubqueryTable> tableOrSubqueryTable;
    CopyOnWriteLazyLang<TableOrSubqueryTableFunction>
        tableOrSubqueryTableFunction;
    CopyOnWriteLazyLang<TableOrSubqueryJoinClause> tableOrSubqueryJoinClause;
    CopyOnWriteLazyLang<TableOrSubquerySelect> tableOrSubquerySelect;
    CopyOnWriteLazyLang<TableOrSubqueryList> tableOrSubqueryList;

    virtual CopyOnWriteString SQL() const override;
};

} // namespace Lang

} // namespace WCDB

#endif /* LangTableOrSubquery_hpp */
