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

OrderingTerm::OrderingTerm(const Expression &expression)
{
    setupWithExpression(expression);
}

OrderingTerm &OrderingTerm::withCollate(const std::string &collateName)
{
    lang::OrderingTerm &lang = getMutableLang();
    lang.collationName.assign(collateName);
    return *this;
}

OrderingTerm &OrderingTerm::withOrder(const Order &order)
{
    lang::OrderingTerm &lang = getMutableLang();
    lang.order = order;
    return *this;
}

void OrderingTerm::setupWithExpression(const Expression &expression)
{
    lang::OrderingTerm &lang = getMutableLang();
    lang.expr.assign(expression.getLang());
}

} // namespace WCDB
