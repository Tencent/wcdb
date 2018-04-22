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

import Foundation

public protocol InsertTableInterface: class {
    associatedtype Object: TableEncodable

    func insert(objects: Object..., on propertyConvertibleList: [PropertyConvertible]?) throws
    func insert(objects: [Object], on propertyConvertibleList: [PropertyConvertible]?) throws
    func insertOrReplace(objects: Object..., on propertyConvertibleList: [PropertyConvertible]?) throws
    func insertOrReplace(objects: [Object], on propertyConvertibleList: [PropertyConvertible]?) throws
}

public protocol UpdateTableInterface: class {
    associatedtype Object: TableEncodable

    func update(on propertyConvertibleList: PropertyConvertible...,
                with object: Object,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
    func update(on propertyConvertibleList: [PropertyConvertible],
                with object: Object,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
    func update(on propertyConvertibleList: PropertyConvertible...,
                with row: [ColumnEncodableBase],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
    func update(on propertyConvertibleList: [PropertyConvertible],
                with row: [ColumnEncodableBase],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
}

public protocol DeleteTableInterface: class {
    func delete(where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws
}

public protocol RowSelectTableInterface: class {
    func getRows(on columnResultConvertibleList: [ColumnResultConvertible],
                 where condition: Condition?,
                 orderBy orderList: [OrderBy]?,
                 limit: Limit?,
                 offset: Offset?) throws -> FundamentalRowXColumn
    func getRows(on columnResultConvertibleList: ColumnResultConvertible...,
                 where condition: Condition?,
                 orderBy orderList: [OrderBy]?,
                 limit: Limit?,
                 offset: Offset?) throws -> FundamentalRowXColumn

    func getRow(on columnResultConvertibleList: ColumnResultConvertible...,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                offset: Offset?) throws -> FundamentalRow
    func getRow(on columnResultConvertibleList: [ColumnResultConvertible],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                offset: Offset?) throws -> FundamentalRow

    func getColumn(on result: ColumnResultConvertible,
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   limit: Limit?,
                   offset: Offset?) throws -> FundamentalColumn
    func getDistinctColumn(on result: ColumnResultConvertible,
                           where condition: Condition?,
                           orderBy orderList: [OrderBy]?,
                           limit: Limit?,
                           offset: Offset?) throws -> FundamentalColumn

    func getValue(on result: ColumnResultConvertible,
                  where condition: Condition?,
                  orderBy orderList: [OrderBy]?,
                  limit: Limit?,
                  offset: Offset?) throws -> FundamentalValue
    func getDistinctValue(on result: ColumnResultConvertible,
                          where condition: Condition?,
                          orderBy orderList: [OrderBy]?,
                          limit: Limit?,
                          offset: Offset?) throws -> FundamentalValue
}

public protocol SelectTableInterface: class {
    associatedtype Object: TableDecodable

    //TODO: Add generic property convertible to fit the type
    func getObjects(on propertyConvertibleList: [PropertyConvertible],
                    where condition: Condition?,
                    orderBy orderList: [OrderBy]?,
                    limit: Limit?,
                    offset: Offset?) throws -> [Object]
    func getObjects(on propertyConvertibleList: PropertyConvertible...,
                    where condition: Condition?,
                    orderBy orderList: [OrderBy]?,
                    limit: Limit?,
                    offset: Offset?) throws -> [Object]

    func getObject(on propertyConvertibleList: [PropertyConvertible],
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   offset: Offset?) throws -> Object?
    func getObject(on propertyConvertibleList: PropertyConvertible...,
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   offset: Offset?) throws -> Object?
}
