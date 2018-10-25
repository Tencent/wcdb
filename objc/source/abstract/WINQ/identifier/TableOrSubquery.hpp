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

#ifndef TableOrSubquery_hpp
#define TableOrSubquery_hpp

#include <WCDB/SQL.hpp>

namespace WCDB {

class TableOrSubquery : public SQLSyntax<Syntax::TableOrSubquery> {
public:
    using SQLSyntax<Syntax::TableOrSubquery>::SQLSyntax;

    template<typename T, typename Enable = typename std::enable_if<TableOrSubqueryConvertible<T>::value>::type>
    TableOrSubquery(const T& t)
    : TableOrSubquery(TableOrSubqueryConvertible<T>::asTableOrSubquery(t))
    {
    }

    TableOrSubquery(const char* table);
    TableOrSubquery& schema(const Schema& schema);

    TableOrSubquery& as(const SyntaxString& alias);

    TableOrSubquery& notIndexed();
    TableOrSubquery& indexed(const SyntaxString& index);

    static TableOrSubquery function(const SyntaxString& function);
    TableOrSubquery& invoke();
    TableOrSubquery& invoke(const Expressions& expressions);

    explicit TableOrSubquery(const TablesOrSubqueries& tableOrSubqueries);
    TableOrSubquery(const Join& join);

    TableOrSubquery(const StatementSelect& select);

    static TableOrSubquery master();
};

} // namespace WCDB

#endif /* TableOrSubquery_hpp */
