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

Column::Column() = default;

Column::Column(const UnsafeStringView& name)
{
    syntax().wildcard = false;
    syntax().name = name;
}

Column::Column(const UnsafeStringView& name, const BaseBinding* binding)
{
    syntax().wildcard = false;
    syntax().name = name;
    syntax().tableBinding = binding;
}

Column& Column::schema(const Schema& schema)
{
    syntax().schema = schema;
    return *this;
}

Column& Column::table(const UnsafeStringView& table)
{
    syntax().table = table;
    return *this;
}

Column::~Column() = default;

Column Column::rowid()
{
    return Column("rowid");
}

ResultColumn Column::as(const UnsafeStringView& alias)
{
    return ResultColumn(*this).as(alias);
}

IndexedColumn Column::asIndex() const
{
    return IndexedColumn(*this);
}

IndexedColumn Column::asIndex(const Order& order) const
{
    return IndexedColumn(*this).order(order);
}

IndexedColumn Column::asAscIndex() const
{
    return IndexedColumn(*this).order(WCDB::Order::ASC);
}

IndexedColumn Column::asDescIndex() const
{
    return IndexedColumn(*this).order(WCDB::Order::DESC);
}

OrderingTerm Column::asOrder() const
{
    return OrderingTerm(*this);
}

OrderingTerm Column::asOrder(const Order& order) const
{
    return OrderingTerm(*this).order(order);
}

OrderingTerm Column::asAscOrder() const
{
    return OrderingTerm(*this).order(WCDB::Order::ASC);
}

OrderingTerm Column::asDescOrder() const
{
    return OrderingTerm(*this).order(WCDB::Order::DESC);
}

Column Column::all()
{
    Column result = Column();
    result.syntax().wildcard = true;
    return result;
}

Expression Column::asExpressionOperand() const
{
    return Expression(*this);
}

} // namespace WCDB
