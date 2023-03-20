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

#include "WINQ.h"

namespace WCDB {

IndexedColumn::IndexedColumn() = default;

IndexedColumn::IndexedColumn(const Column& column)
{
    syntax().switcher = SyntaxType::Switch::Column;
    syntax().column = column;
}

IndexedColumn::IndexedColumn(const Expression& expression)
{
    syntax().switcher = SyntaxType::Switch::Expression;
    syntax().expression = expression;
}

IndexedColumn::~IndexedColumn() = default;

IndexedColumn& IndexedColumn::collate(const UnsafeStringView& collation)
{
    syntax().collation = collation;
    return *this;
}

IndexedColumn& IndexedColumn::order(const Order& order)
{
    syntax().order = order;
    return *this;
}

} // namespace WCDB
