//
// Created by qiuwenchen on 2022/9/2.
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
#include <WCDB/BaseOperation.hpp>
#include <WCDB/Value.hpp>

namespace WCDB {

class TableOperation : public BaseOperation {
#pragma mark - Insert
public:
    bool insertRows(const MultiRowsValue &rows, const Columns &columns);
    bool insertOrReplaceRows(const MultiRowsValue &rows, const Columns &columns);

#pragma mark - Update
public:
    bool updateRow(const OneRowValue &row,
                   const Columns &columns,
                   const Expression &where = Expression(),
                   const OrderingTerms &orders = OrderingTerms(),
                   const Expression &limit = Expression(),
                   const Expression &offset = Expression());

#pragma mark - delete
public:
    bool deleteValues(const Expression &where = Expression(),
                      const OrderingTerms &orders = OrderingTerms(),
                      const Expression &limit = Expression(),
                      const Expression &offset = Expression());

#pragma mark - Select
public:
    OptionalValue selectValue(const ResultColumn &column,
                              const Expression &where = Expression(),
                              const OrderingTerms &orders = OrderingTerms(),
                              const Expression &offset = Expression());

    OptionalOneColumn selectOneColumn(const ResultColumn &column,
                                      const Expression &where = Expression(),
                                      const OrderingTerms &orders = OrderingTerms(),
                                      const Expression &offset = Expression(),
                                      const Expression &limit = Expression());

    OptionalOneRow selectOneRow(const ResultColumns &columns,
                                const Expression &where = Expression(),
                                const OrderingTerms &orders = OrderingTerms(),
                                const Expression &offset = Expression());

    OptionalMultiRows selectAllRow(const ResultColumns &columns,
                                   const Expression &where = Expression(),
                                   const OrderingTerms &orders = OrderingTerms(),
                                   const Expression &limit = Expression(),
                                   const Expression &offset = Expression());

#pragma mark - Statement
public:
    OptionalValue getValueFromStatement(const Statement &statement, int index = 0);
    OptionalOneRow getOneColumnFromStatement(const Statement &statement, int index = 0);
    OptionalOneRow getOneRowFromStatement(const Statement &statement);
    OptionalMultiRows getAllRowsFromStatement(const Statement &statement);

protected:
    virtual ~TableOperation() override = 0;
    virtual const UnsafeStringView &getTableName() const = 0;
};

} //namespace WCDB
