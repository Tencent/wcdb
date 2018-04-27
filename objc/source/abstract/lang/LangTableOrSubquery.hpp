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

#include <WCDB/LangCommon.hpp>

namespace WCDB {

namespace Lang {

//TODO: remove it
class TableOrSubqueryBase : public Lang {
public:
    enum class Type : int {
        NotSet,
        Table,
        TableFunction,
        JoinClause,
        Select,
        List,
        TableOrSubquery,
    };
    virtual Type getType() const = 0;
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

    CopyOnWriteString SQL() const override;
    Type getType() const override;
};

class TableOrSubqueryTableFunction : public TableOrSubqueryBase {
public:
    CopyOnWriteString schemaName;
    CopyOnWriteString tableFunctionName;
    CopyOnWriteString tableAlias;
    CopyOnWriteLazyLangList<Expr> exprs;

    CopyOnWriteString SQL() const override;
    Type getType() const override;
};

class TableOrSubqueryJoinClause : public TableOrSubqueryBase {
public:
    CopyOnWriteLazyLang<JoinClause> joinClause;

    CopyOnWriteString SQL() const override;
    Type getType() const override;
};

class TableOrSubquerySelect : public TableOrSubqueryBase {
public:
    CopyOnWriteLazyLang<SelectSTMT> selectSTMT;
    CopyOnWriteString tableAlias;

    CopyOnWriteString SQL() const override;
    Type getType() const override;
};

class TableOrSubqueryList : public TableOrSubqueryBase {
public:
    CopyOnWriteLazyLangList<TableOrSubqueryBase> tableOrSubquerys;

    CopyOnWriteString SQL() const override;
    Type getType() const override;
};

class TableOrSubquery : public TableOrSubqueryBase {
public:
    TableOrSubquery();

    Type type;
    CopyOnWriteLazyLang<TableOrSubqueryTable> tableOrSubqueryTable;
    CopyOnWriteLazyLang<TableOrSubqueryTableFunction>
        tableOrSubqueryTableFunction;
    CopyOnWriteLazyLang<TableOrSubqueryJoinClause> tableOrSubqueryJoinClause;
    CopyOnWriteLazyLang<TableOrSubquerySelect> tableOrSubquerySelect;
    CopyOnWriteLazyLang<TableOrSubqueryList> tableOrSubqueryList;

    CopyOnWriteString SQL() const override;
    Type getType() const override;
};

} // namespace Lang

} // namespace WCDB

#endif /* LangTableOrSubquery_hpp */
