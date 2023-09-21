//
// Created by qiuwenchen on 2022/9/13.
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

import Foundation
import WCDB_Private

/// Convenient interface for row selecting
public protocol RowSelectInterface: AnyObject {

    /// Get rows by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `MultiRowsValue`
    /// - Throws: `Error`
    func getRows(on resultColumnConvertibleList: [ResultColumnConvertible],
                 fromTable table: String,
                 where condition: Condition?,
                 orderBy orderList: [OrderBy]?,
                 limit: Limit?,
                 offset: Offset?) throws -> MultiRowsValue

    /// Get rows by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `MultiRowsValue`
    /// - Throws: `Error`
    func getRows(on resultColumnConvertibleList: ResultColumnConvertible...,
                 fromTable table: String,
                 where condition: Condition?,
                 orderBy orderList: [OrderBy]?,
                 limit: Limit?,
                 offset: Offset?) throws -> MultiRowsValue

    /// Get row by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `OneRowValue`
    /// - Throws: `Error`
    func getRow(on resultColumnConvertibleList: ResultColumnConvertible...,
                fromTable table: String,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                offset: Offset?) throws -> OneRowValue

    /// Get row by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `OneRowValue`
    /// - Throws: `Error`
    func getRow(on resultColumnConvertibleList: [ResultColumnConvertible],
                fromTable table: String,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                offset: Offset?) throws -> OneRowValue

    /// Get column by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `OneColumnValue`
    /// - Throws: `Error`
    func getColumn(on resultColumnConvertible: ResultColumnConvertible,
                   fromTable table: String,
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   limit: Limit?,
                   offset: Offset?) throws -> OneColumnValue

    /// Get distinct column by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `OneColumnValue`
    /// - Throws: `Error`
    func getDistinctColumn(on resultColumnConvertible: ResultColumnConvertible,
                           fromTable table: String,
                           where condition: Condition?,
                           orderBy orderList: [OrderBy]?,
                           limit: Limit?,
                           offset: Offset?) throws -> OneColumnValue

    /// Get value by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `Value`
    /// - Throws: `Error`
    func getValue(on resultColumnConvertible: ResultColumnConvertible,
                  fromTable table: String,
                  where condition: Condition?,
                  orderBy orderList: [OrderBy]?,
                  limit: Limit?,
                  offset: Offset?) throws -> Value

    /// Get distinct value by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `Value`
    /// - Throws: `Error`
    func getDistinctValue(on result: ResultColumnConvertible,
                          fromTable table: String,
                          where condition: Condition?,
                          orderBy orderList: [OrderBy]?,
                          limit: Limit?,
                          offset: Offset?) throws -> Value
}

extension RowSelectInterface where Self: HandleRepresentable {
    public func getRows(on resultColumnConvertibleList: [ResultColumnConvertible],
                        fromTable table: String,
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws -> MultiRowsValue {
        let rowSelect = RowSelect(with: try getHandle(writeHint: false),
                                  results: resultColumnConvertibleList,
                                  tables: [table],
                                  isDistinct: false)
        if condition != nil {
            rowSelect.where(condition!)
        }
        if orderList != nil {
            rowSelect.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                rowSelect.limit(limit!, offset: offset!)
            } else {
                rowSelect.limit(limit!)
            }
        }
        return try rowSelect.allRows()
    }

    public func getRows(on resultColumnConvertibleList: ResultColumnConvertible...,
                        fromTable table: String,
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws -> MultiRowsValue {
        return try getRows(
            on: resultColumnConvertibleList.isEmpty ? [Column.all()] : resultColumnConvertibleList,
            fromTable: table,
            where: condition,
            orderBy: orderList,
            limit: limit,
            offset: offset)
    }

    public func getRow(on resultColumnConvertibleList: ResultColumnConvertible...,
                       fromTable table: String,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       offset: Offset? = nil) throws -> OneRowValue {
        return try getRow(
            on: resultColumnConvertibleList.isEmpty ? [Column.all()] : resultColumnConvertibleList,
            fromTable: table,
            where: condition,
            orderBy: orderList,
            offset: offset)
    }

    public func getRow(on resultColumnConvertibleList: [ResultColumnConvertible],
                       fromTable table: String,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       offset: Offset? = nil) throws -> OneRowValue {
        return try getRows(on: resultColumnConvertibleList,
                           fromTable: table,
                           where: condition,
                           orderBy: orderList,
                           limit: 1,
                           offset: offset).first ?? []
    }

    public func getColumn(on result: ResultColumnConvertible,
                          fromTable table: String,
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          limit: Limit? = nil,
                          offset: Offset? = nil) throws -> OneColumnValue {
        let rowSelect = RowSelect(with: try getHandle(writeHint: false), results: [result], tables: [table], isDistinct: false)
        if condition != nil {
            rowSelect.where(condition!)
        }
        if orderList != nil {
            rowSelect.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                rowSelect.limit(limit!, offset: offset!)
            } else {
                rowSelect.limit(limit!)
            }
        }
        return try rowSelect.allValues()
    }

    public func getDistinctColumn(on result: ResultColumnConvertible,
                                  fromTable table: String,
                                  where condition: Condition? = nil,
                                  orderBy orderList: [OrderBy]? = nil,
                                  limit: Limit? = nil,
                                  offset: Offset? = nil) throws -> OneColumnValue {
        let rowSelect = RowSelect(with: try getHandle(writeHint: false), results: [result], tables: [table], isDistinct: true)
        if condition != nil {
            rowSelect.where(condition!)
        }
        if orderList != nil {
            rowSelect.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                rowSelect.limit(limit!, offset: offset!)
            } else {
                rowSelect.limit(limit!)
            }
        }
        return try rowSelect.allValues()
    }

    public func getValue(on result: ResultColumnConvertible,
                         fromTable table: String,
                         where condition: Condition? = nil,
                         orderBy orderList: [OrderBy]? = nil,
                         limit: Limit? = nil,
                         offset: Offset? = nil) throws -> Value {
        return (try getRows(on: result,
                            fromTable: table,
                            where: condition,
                            orderBy: orderList,
                            limit: 1,
                            offset: offset).first?.first) ?? Value(nil)
    }

    public func getDistinctValue(on result: ResultColumnConvertible,
                                 fromTable table: String,
                                 where condition: Condition? = nil,
                                 orderBy orderList: [OrderBy]? = nil,
                                 limit: Limit? = nil,
                                 offset: Offset? = nil) throws -> Value {
        return (try getDistinctColumn(on: result, fromTable: table).first) ?? Value(nil)
    }
}

/// Convenient interface for selecting
public protocol SelectInterface: AnyObject {
    // TODO: Add generic property convertible to fit the type

    /// Get objects on specific(or all) properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: Table decodable objects
    /// - Throws: `Error`
    func getObjects<Object: TableDecodable>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: String,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws -> [Object]

    /// Get objects on specific(or all) properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: Table decodable objects
    /// - Throws: `Error`
    func getObjects<Object: TableDecodable>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: String,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws -> [Object]

    /// Get object on specific(or all) properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: Table decodable objects
    /// - Throws: `Error`
    func getObject<Object: TableDecodable>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: String,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        offset: Offset?) throws -> Object?

    /// Get object on specific(or all) properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - condition: Expression convertible
    /// - Returns: Table decodable objects
    /// - Throws: `Error`
    func getObject<Object: TableDecodable>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: String,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        offset: Offset?) throws -> Object?
}

extension SelectInterface where Self: HandleRepresentable {
    public func getObjects<Object: TableDecodable>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: String,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws -> [Object] {
        let select = Select(with: try getHandle(writeHint: false), on: propertyConvertibleList, table: table, isDistinct: false)
        if condition != nil {
            select.where(condition!)
        }
        if orderList != nil {
            select.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                select.limit(limit!, offset: offset!)
            } else {
                select.limit(limit!)
            }
        }
        return try select.allObjects()
    }

    public func getObjects<Object: TableDecodable>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: String,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws -> [Object] {
        return try getObjects(on: propertyConvertibleList.isEmpty ? Object.Properties.all : propertyConvertibleList,
                              fromTable: table,
                              where: condition,
                              orderBy: orderList,
                              limit: limit,
                              offset: offset)
    }

    public func getObject<Object: TableDecodable>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: String,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        offset: Offset? = nil) throws -> Object? {
        return try getObjects(on: propertyConvertibleList,
                              fromTable: table,
                              where: condition,
                              orderBy: orderList,
                              limit: 1,
                              offset: offset).first
    }

    public func getObject<Object: TableDecodable>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: String,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        offset: Offset? = nil) throws -> Object? {
        return try getObject(on: propertyConvertibleList.isEmpty ? Object.Properties.all : propertyConvertibleList,
                             fromTable: table,
                             where: condition,
                             orderBy: orderList,
                             offset: offset)
    }
}
