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

#include <WINQ/WINQ.h>

namespace WCDB {

IndexedColumn::IndexedColumn(const Expression &expression)
{
    lang::IndexedColumn &lang = getMutableLang();
    lang.switcher = lang::IndexedColumn::Switch::Expr;
    lang.expr.assign(expression.getLang());
}

IndexedColumn::IndexedColumn(const std::string &columnName)
{
    lang::IndexedColumn &lang = getMutableLang();
    lang.switcher = lang::IndexedColumn::Switch::ColumnName;
    lang.columnName.assign(columnName);
}

IndexedColumn &IndexedColumn::withCollate(const std::string &collationName)
{
    lang::IndexedColumn &lang = getMutableLang();
    lang.collationName.assign(collationName);
    return *this;
}

IndexedColumn &IndexedColumn::withOrder(const Order &order)
{
    lang::IndexedColumn &lang = getMutableLang();
    lang.order = order;
    return *this;
}

} // namespace WCDB
