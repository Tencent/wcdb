//
// Created by sanhuazhang on 2019/05/02
//

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

#pragma once

#include "Statement.hpp"

namespace WCDB {

class TableOrSubquery;

class WCDB_API StatementSelect final : public SpecifiedSyntax<Syntax::SelectSTMT, Statement> {
public:
    using SpecifiedSyntax<Syntax::SelectSTMT, Statement>::SpecifiedSyntax;
    StatementSelect();
    ~StatementSelect() override;

    StatementSelect& with(const CommonTableExpressions& commonTableExpressions);
    StatementSelect& recursive();

    StatementSelect& select(const ResultColumns& resultColumns);
    StatementSelect& distinct();
    StatementSelect& from(const TablesOrSubqueries& tablesOrSubqueries);
    StatementSelect& from(const Join& join);

    StatementSelect& where(const Expression& condition);
    StatementSelect& group(const Expression& group);
    StatementSelect& groups(const Expressions& groups);
    StatementSelect& having(const Expression& having);

    StatementSelect& window(const UnsafeStringView& window);
    StatementSelect& as(const WindowDef& windowDef);

    TableOrSubquery as(const UnsafeStringView& alias);

    StatementSelect& value(const Expression& expression);
    StatementSelect& values(const Expressions& expressions);

    StatementSelect& union_();
    StatementSelect& unionAll();
    StatementSelect& intersect();
    StatementSelect& except();

    StatementSelect& order(const OrderingTerm& order);
    StatementSelect& orders(const OrderingTerms& orders);

    StatementSelect& limit(const Expression& from, const Expression& to);

    StatementSelect& limit(const Expression& limit);
    StatementSelect& offset(const Expression& offset);

private:
    Syntax::SelectCore& currentCore();
};

} // namespace WCDB
