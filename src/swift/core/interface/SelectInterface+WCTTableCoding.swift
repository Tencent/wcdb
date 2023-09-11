//
// Created by qiuwenchen on 2022/11/13.
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

#if WCDB_SWIFT_BRIDGE_OBJC

import Foundation
import WCDB_Private

/// Convenient interface for selecting
public protocol SelectInterfaceForObjc: AnyObject {
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
    func getObjects<Object: WCTTableCoding>(
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
    func getObjects<Object: WCTTableCoding>(
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
    func getObject<Object: WCTTableCoding>(
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
    func getObject<Object: WCTTableCoding>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: String,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        offset: Offset?) throws -> Object?
}

extension SelectInterfaceForObjc where Self: HandleRepresentable {
    public func getObjects<Object: WCTTableCoding>(
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

    public func getObjects<Object: WCTTableCoding>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: String,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws -> [Object] {
        return try getObjects(on: propertyConvertibleList.isEmpty ? Object.allProperties() : propertyConvertibleList,
                              fromTable: table,
                              where: condition,
                              orderBy: orderList,
                              limit: limit,
                              offset: offset)
    }

    public func getObject<Object: WCTTableCoding>(
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

    public func getObject<Object: WCTTableCoding>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: String,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        offset: Offset? = nil) throws -> Object? {
        return try getObject(on: propertyConvertibleList.isEmpty ? Object.allProperties() : propertyConvertibleList,
                             fromTable: table,
                             where: condition,
                             orderBy: orderList,
                             offset: offset)
    }
}

#endif
