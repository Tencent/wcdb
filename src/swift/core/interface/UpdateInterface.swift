//
// Created by qiuwenchen on 2022/9/12.
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

/// Convenient interface for updating
public protocol UpdateInterface: AnyObject {

    /// Execute updating with `TableEncodable` object on specific(or all) properties.
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - object: Table encodable object
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    func update<Object: TableEncodable>(
        table: String,
        on propertyConvertibleList: PropertyConvertible...,
        with object: Object,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws

    /// Execute updating with `TableEncodable` object on specific(or all) properties.
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - object: Table encodable object
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    func update<Object: TableEncodable>(
        table: String,
        on propertyConvertibleList: [PropertyConvertible],
        with object: Object,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws

    /// Execute updating with row on specific(or all) properties.
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - row: a row of ColumnEncodable value
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    func update(table: String,
                on propertyConvertibleList: PropertyConvertible...,
                with row: [ColumnEncodable?],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws

    /// Execute updating with row on specific(or all) properties.
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - row: a row of ColumnEncodable value
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    func update(table: String,
                on propertyConvertibleList: PropertyConvertible...,
                with row: ColumnEncodable?...,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws

    /// Execute updating with row on specific(or all) properties.
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - row: a row of ColumnEncodable value
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    func update(table: String,
                on propertyConvertibleList: [PropertyConvertible],
                with row: ColumnEncodable?...,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws

    /// Execute updating with row on specific(or all) properties.
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - row: a row of ColumnEncodable value
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    func update(table: String,
                on propertyConvertibleList: [PropertyConvertible],
                with row: [ColumnEncodable?],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
}

extension UpdateInterface where Self: HandleRepresentable {
    public func update<Object: TableEncodable>(
        table: String,
        on propertyConvertibleList: [PropertyConvertible],
        with object: Object,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws {
        let update = Update(with: try getHandle(writeHint: true), on: propertyConvertibleList, andTable: table)
        if condition != nil {
            update.where(condition!)
        }
        if orderList != nil {
            update.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                update.limit(limit!, offset: offset!)
            } else {
                update.limit(limit!)
            }
        }
        return try update.execute(with: object)
    }

    public func update<Object: TableEncodable>(
        table: String,
        on propertyConvertibleList: PropertyConvertible...,
        with object: Object,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws {
        return try update(table: table,
                          on: propertyConvertibleList,
                          with: object,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    public func update(table: String,
                       on propertyConvertibleList: PropertyConvertible...,
                       with row: [ColumnEncodable?],
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        return try update(table: table,
                          on: propertyConvertibleList,
                          with: row,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    public func update(table: String,
                       on propertyConvertibleList: PropertyConvertible...,
                       with row: ColumnEncodable?...,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        return try update(table: table,
                          on: propertyConvertibleList,
                          with: row,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    public func update(table: String,
                       on propertyConvertibleList: [PropertyConvertible],
                       with row: ColumnEncodable?...,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        return try update(table: table,
                          on: propertyConvertibleList,
                          with: row,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    public func update(table: String,
                       on propertyConvertibleList: [PropertyConvertible],
                       with row: [ColumnEncodable?],
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        let update = Update(with: try getHandle(writeHint: true), on: propertyConvertibleList, andTable: table)
        if condition != nil {
            update.where(condition!)
        }
        if orderList != nil {
            update.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                update.limit(limit!, offset: offset!)
            } else {
                update.limit(limit!)
            }
        }
        return try update.execute(with: row)
    }
}
