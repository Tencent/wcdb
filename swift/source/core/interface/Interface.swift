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

/// Convenient interface for inserting
public protocol InsertInterface: class {

    /// Execute inserting with `TableEncodable` object on specific(or all) properties
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insert<Object: TableEncodable>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws
    
    /// Execute inserting with `TableEncodable` object on specific(or all) properties
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insert<Object, T>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: T) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String

    /// Execute inserting with `TableEncodable` object on specific(or all) properties
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insert<Object: TableEncodable>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws

    /// Execute inserting with `TableEncodable` object on specific(or all) properties
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insert<Object, T>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: T) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String

    /// Execute inserting or replacing with `TableEncodable` object on specific(or all) properties.
    /// It will replace the original row while they have same primary key or row id.
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insertOrReplace<Object: TableEncodable>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws

    /// Execute inserting or replacing with `TableEncodable` object on specific(or all) properties.
    /// It will replace the original row while they have same primary key or row id.
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insertOrReplace<Object, T>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: T) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String

    /// Execute inserting or replacing with `TableEncodable` object on specific(or all) properties.
    /// It will replace the original row while they have same primary key or row id.
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insertOrReplace<Object: TableEncodable>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws

    /// Execute inserting or replacing with `TableEncodable` object on specific(or all) properties.
    /// It will replace the original row while they have same primary key or row id.
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insertOrReplace<Object, T>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: T) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String
    
}

extension InsertInterface where Self: Core {
    public func insert<Object, T>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: T) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String {
            return try insert(objects: objects, on: propertyConvertibleList, intoTable: table.rawValue)
    }

    public func insert<Object: TableEncodable>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        let insert = try Insert(with: self, named: table, on: propertyConvertibleList, isReplace: false)
        return try insert.execute(with: objects)
    }

    public func insertOrReplace<Object: TableEncodable>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        let insert = try Insert(with: self, named: table, on: propertyConvertibleList, isReplace: true)
        return try insert.execute(with: objects)
    }
    
    public func insertOrReplace<Object, T>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: T) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String {
            return try insertOrReplace(objects: objects, on: propertyConvertibleList, intoTable: table.rawValue)
    }

    public func insert<Object: TableEncodable>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        return try insert(objects: objects, on: propertyConvertibleList, intoTable: table)
    }

    public func insert<Object, T>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: T) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String {
            return try insert(objects: objects, on: propertyConvertibleList, intoTable: table.rawValue)
    }

    public func insertOrReplace<Object: TableEncodable>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        return try insertOrReplace(objects: objects, on: propertyConvertibleList, intoTable: table)
    }

    public func insertOrReplace<Object, T>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: T) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String {
        return try insertOrReplace(objects: objects, on: propertyConvertibleList, intoTable: table.rawValue)
    }
}

/// Convenient interface for updating
public protocol UpdateInterface: class {

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
    func update<Object, T>(
        table: T,
        on propertyConvertibleList: PropertyConvertible...,
        with object: Object,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String

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
    func update<Object, T>(
        table: T,
        on propertyConvertibleList: [PropertyConvertible],
        with object: Object,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String

    /// Execute updating with row on specific(or all) properties.
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
    func update(table: String,
                on propertyConvertibleList: PropertyConvertible...,
                with row: [ColumnEncodable],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws

    /// Execute updating with row on specific(or all) properties.
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
    func update<T>(table: T,
                   on propertyConvertibleList: PropertyConvertible...,
                   with row: [ColumnEncodableBase],
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   limit: Limit?,
                   offset: Offset?) throws
                   where T : RawRepresentable, T.RawValue == String

    /// Execute updating with row on specific(or all) properties.
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
    func update(table: String,
                on propertyConvertibleList: [PropertyConvertible],
                with row: [ColumnEncodable],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws

    /// Execute updating with row on specific(or all) properties.
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
    func update<T>(table: T,
                on propertyConvertibleList: [PropertyConvertible],
                with row: [ColumnEncodableBase],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
                where T : RawRepresentable, T.RawValue == String
}

extension UpdateInterface where Self: Core {
    public func update<Object: TableEncodable>(
        table: String,
        on propertyConvertibleList: [PropertyConvertible],
        with object: Object,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws {
        let update = try Update(with: self, on: propertyConvertibleList, andTable: table)
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

    public func update<Object, T>(
        table: T,
        on propertyConvertibleList: [PropertyConvertible],
        with object: Object,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String {
        return try update(table: table.rawValue, on: propertyConvertibleList, with: object, where: condition, orderBy: orderList, limit: limit, offset: offset)
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

    public func update<Object, T>(
        table: T,
        on propertyConvertibleList: PropertyConvertible...,
        with object: Object,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws
        where Object : TableEncodable, T : RawRepresentable, T.RawValue == String {
        return try update(table: table.rawValue,
                          on: propertyConvertibleList,
                          with: object,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    public func update(table: String,
                       on propertyConvertibleList: PropertyConvertible...,
                       with row: [ColumnEncodable],
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

    public func update<T>(table: T,
                          on propertyConvertibleList: PropertyConvertible...,
                          with row: [ColumnEncodableBase],
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          limit: Limit? = nil,
                          offset: Offset? = nil) throws
                          where T : RawRepresentable, T.RawValue == String {
        return try update(table: table.rawValue,
                          on: propertyConvertibleList,
                          with: row,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    public func update<T>(table: T,
                          on propertyConvertibleList: [PropertyConvertible],
                          with row: [ColumnEncodableBase],
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          limit: Limit? = nil,
                          offset: Offset? = nil) throws
                          where T : RawRepresentable, T.RawValue == String {
            return try update(table: table.rawValue, on: propertyConvertibleList, with: row, where: condition, orderBy: orderList, limit: limit, offset: offset)
        
    }
    public func update(table: String,
                       on propertyConvertibleList: [PropertyConvertible],
                       with row: [ColumnEncodable],
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        let update = try Update(with: self, on: propertyConvertibleList, andTable: table)
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

/// Convenient interface for deleting
public protocol DeleteInterface: class {

    /// Execute deleting
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    func delete(fromTable table: String,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws

    /// Execute deleting
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    func delete<T>(fromTable table: T,
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   limit: Limit?,
                   offset: Offset?) throws
                   where T : RawRepresentable, T.RawValue == String
}

extension DeleteInterface where Self: Core {
    public func delete<T>(fromTable table: T,
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          limit: Limit? = nil,
                          offset: Offset? = nil) throws
                          where T : RawRepresentable, T.RawValue == String {
            return try delete(fromTable: table.rawValue, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }

    public func delete(fromTable table: String,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        let delete = try Delete(with: self, andTableName: table)
        if condition != nil {
            delete.where(condition!)
        }
        if orderList != nil {
            delete.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                delete.limit(limit!, offset: offset!)
            } else {
                delete.limit(limit!)
            }
        }
        return try delete.execute()
    }
}

/// Convenient interface for row selecting
public protocol RowSelectInterface: class {

    /// Get rows by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRowXColumn`
    /// - Throws: `Error`
    func getRows(on columnResultConvertibleList: [ColumnResultConvertible],
                 fromTable table: String,
                 where condition: Condition?,
                 orderBy orderList: [OrderBy]?,
                 limit: Limit?,
                 offset: Offset?) throws -> FundamentalRowXColumn

    /// Get rows by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRowXColumn`
    /// - Throws: `Error`
    func getRows<T>(on columnResultConvertibleList: [ColumnResultConvertible],
                    fromTable table: T,
                    where condition: Condition?,
                    orderBy orderList: [OrderBy]?,
                    limit: Limit?,
                    offset: Offset?) throws -> FundamentalRowXColumn
                    where T : RawRepresentable, T.RawValue == String

    /// Get rows by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRowXColumn`
    /// - Throws: `Error`
    func getRows(on columnResultConvertibleList: ColumnResultConvertible...,
                 fromTable table: String,
                 where condition: Condition?,
                 orderBy orderList: [OrderBy]?,
                 limit: Limit?,
                 offset: Offset?) throws -> FundamentalRowXColumn

    /// Get rows by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRowXColumn`
    /// - Throws: `Error`
    func getRows<T>(on columnResultConvertibleList: ColumnResultConvertible...,
                    fromTable table: T,
                    where condition: Condition?,
                    orderBy orderList: [OrderBy]?,
                    limit: Limit?,
                    offset: Offset?) throws -> FundamentalRowXColumn
                    where T : RawRepresentable, T.RawValue == String

    /// Get row by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRow`
    /// - Throws: `Error`
    func getRow(on columnResultConvertibleList: ColumnResultConvertible...,
                fromTable table: String,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                offset: Offset?) throws -> FundamentalRow

    /// Get row by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRow`
    /// - Throws: `Error`
    func getRow<T>(on columnResultConvertibleList: ColumnResultConvertible...,
                   fromTable table: T,
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   offset: Offset?) throws -> FundamentalRow
                   where T : RawRepresentable, T.RawValue == String

    /// Get row by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRow`
    /// - Throws: `Error`
    func getRow(on columnResultConvertibleList: [ColumnResultConvertible],
                fromTable table: String,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                offset: Offset?) throws -> FundamentalRow

    /// Get row by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRow`
    /// - Throws: `Error`
    func getRow<T>(on columnResultConvertibleList: [ColumnResultConvertible],
                   fromTable table: T,
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   offset: Offset?) throws -> FundamentalRow
                   where T : RawRepresentable, T.RawValue == String

    /// Get column by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalColumn`
    /// - Throws: `Error`
    func getColumn(on columnResultConvertible: ColumnResultConvertible,
                   fromTable table: String,
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   limit: Limit?,
                   offset: Offset?) throws -> FundamentalColumn

    /// Get column by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalColumn`
    /// - Throws: `Error`
    func getColumn<T>(on result: ColumnResultConvertible,
                      fromTable table: T,
                      where condition: Condition?,
                      orderBy orderList: [OrderBy]?,
                      limit: Limit?,
                      offset: Offset?) throws -> FundamentalColumn
                      where T : RawRepresentable, T.RawValue == String

    /// Get distinct column by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalColumn`
    /// - Throws: `Error`
    func getDistinctColumn(on columnResultConvertible: ColumnResultConvertible,
                           fromTable table: String,
                           where condition: Condition?,
                           orderBy orderList: [OrderBy]?,
                           limit: Limit?,
                           offset: Offset?) throws -> FundamentalColumn
    
    /// Get distinct column by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalColumn`
    /// - Throws: `Error`
    func getDistinctColumn<T>(on result: ColumnResultConvertible,
                              fromTable table: T,
                              where condition: Condition?,
                              orderBy orderList: [OrderBy]?,
                              limit: Limit?,
                              offset: Offset?) throws -> FundamentalColumn
                              where T : RawRepresentable, T.RawValue == String
    /// Get value by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalValue`
    /// - Throws: `Error`
    func getValue(on columnResultConvertible: ColumnResultConvertible,
                  fromTable table: String,
                  where condition: Condition?,
                  orderBy orderList: [OrderBy]?,
                  limit: Limit?,
                  offset: Offset?) throws -> FundamentalValue

    /// Get value by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalValue`
    /// - Throws: `Error`
    func getValue<T>(on result: ColumnResultConvertible,
                     fromTable table: T,
                     where condition: Condition?,
                     orderBy orderList: [OrderBy]?,
                     limit: Limit?,
                     offset: Offset?) throws -> FundamentalValue
                     where T : RawRepresentable, T.RawValue == String

    /// Get distinct value by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalValue`
    /// - Throws: `Error`
    func getDistinctValue(on result: ColumnResultConvertible,
                          fromTable table: String,
                          where condition: Condition?,
                          orderBy orderList: [OrderBy]?,
                          limit: Limit?,
                          offset: Offset?) throws -> FundamentalValue

    /// Get distinct value by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertible: WINQ column result
    ///   - table: Table name
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalValue`
    /// - Throws: `Error`
    func getDistinctValue<T>(on result: ColumnResultConvertible,
                             fromTable table: T,
                             where condition: Condition?,
                             orderBy orderList: [OrderBy]?,
                             limit: Limit?,
                             offset: Offset?) throws -> FundamentalValue
                             where T : RawRepresentable, T.RawValue == String
}

extension RowSelectInterface where Self: Core {
    public func getRows<T>(on columnResultConvertibleList: [ColumnResultConvertible],
                           fromTable table: T,
                           where condition: Condition? = nil,
                           orderBy orderList: [OrderBy]? = nil,
                           limit: Limit? = nil,
                           offset: Offset? = nil) throws -> FundamentalRowXColumn
                           where T : RawRepresentable, T.RawValue == String {
        return try getRows(on: columnResultConvertibleList, fromTable: table.rawValue, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }

    public func getRows(on columnResultConvertibleList: [ColumnResultConvertible],
                        fromTable table: String,
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws -> FundamentalRowXColumn {
        let rowSelect = try RowSelect(with: self,
                                      results: columnResultConvertibleList,
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

    public func getRows(on columnResultConvertibleList: ColumnResultConvertible...,
                        fromTable table: String,
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws -> FundamentalRowXColumn {
        return try getRows(
            on: columnResultConvertibleList.isEmpty ? [Column.all] : columnResultConvertibleList,
            fromTable: table,
            where: condition,
            orderBy: orderList,
            limit: limit,
            offset: offset)
    }

    public func getRows<T>(on columnResultConvertibleList: ColumnResultConvertible...,
                           fromTable table: T,
                           where condition: Condition? = nil,
                           orderBy orderList: [OrderBy]? = nil,
                           limit: Limit? = nil,
                           offset: Offset? = nil) throws -> FundamentalRowXColumn
                           where T : RawRepresentable, T.RawValue == String {
        return try getRows(
            on: columnResultConvertibleList.isEmpty ? [Column.any] : columnResultConvertibleList,
            fromTable: table.rawValue,
            where: condition,
            orderBy: orderList,
            limit: limit,
            offset: offset)
    }

    public func getRow(on columnResultConvertibleList: ColumnResultConvertible...,
                       fromTable table: String,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       offset: Offset? = nil) throws -> FundamentalRow {
        return try getRow(
            on: columnResultConvertibleList.isEmpty ? [Column.all] : columnResultConvertibleList,
            fromTable: table,
            where: condition,
            orderBy: orderList,
            offset: offset)
    }

    public func getRow<T>(on columnResultConvertibleList: ColumnResultConvertible...,
                          fromTable table: T,
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          offset: Offset? = nil) throws -> FundamentalRow
                          where T : RawRepresentable, T.RawValue == String {
        return try getRow(
            on: columnResultConvertibleList.isEmpty ? [Column.any] : columnResultConvertibleList,
            fromTable: table.rawValue,
            where: condition,
            orderBy: orderList,
            offset: offset)
    }

    public func getRow(on columnResultConvertibleList: [ColumnResultConvertible],
                       fromTable table: String,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       offset: Offset? = nil) throws -> FundamentalRow {
        return try getRows(on: columnResultConvertibleList,
                           fromTable: table,
                           where: condition,
                           orderBy: orderList,
                           limit: 1,
                           offset: offset).first ?? []
    }

    public func getRow<T>(on columnResultConvertibleList: [ColumnResultConvertible],
                          fromTable table: T,
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          offset: Offset? = nil) throws -> FundamentalRow
                          where T : RawRepresentable, T.RawValue == String {
        return try getRow(on: columnResultConvertibleList, fromTable: table.rawValue, where: condition, orderBy: orderList, offset: offset)
    }

    public func getColumn<T>(on result: ColumnResultConvertible,
                             fromTable table: T,
                             where condition: Condition? = nil,
                             orderBy orderList: [OrderBy]? = nil,
                             limit: Limit? = nil,
                             offset: Offset? = nil) throws -> FundamentalColumn
                             where T : RawRepresentable, T.RawValue == String {
          return try getColumn(on: result, fromTable: table.rawValue, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }

    public func getColumn(on result: ColumnResultConvertible,
                          fromTable table: String,
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          limit: Limit? = nil,
                          offset: Offset? = nil) throws -> FundamentalColumn {
        let rowSelect = try RowSelect(with: self, results: [result], tables: [table], isDistinct: false)
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

    public func getDistinctColumn<T>(on result: ColumnResultConvertible,
                                     fromTable table: T,
                                     where condition: Condition? = nil,
                                     orderBy orderList: [OrderBy]? = nil,
                                     limit: Limit? = nil,
                                     offset: Offset? = nil) throws -> FundamentalColumn
                                     where T : RawRepresentable, T.RawValue == String {
        return try getDistinctColumn(on: result, fromTable: table.rawValue, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }

    public func getDistinctColumn(on result: ColumnResultConvertible,
                                  fromTable table: String,
                                  where condition: Condition? = nil,
                                  orderBy orderList: [OrderBy]? = nil,
                                  limit: Limit? = nil,
                                  offset: Offset? = nil) throws -> FundamentalColumn {
        let rowSelect = try RowSelect(with: self, results: [result], tables: [table], isDistinct: true)
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

    public func getValue<T>(on result: ColumnResultConvertible,
                            fromTable table: T,
                            where condition: Condition? = nil,
                            orderBy orderList: [OrderBy]? = nil,
                            limit: Limit? = nil,
                            offset: Offset? = nil) throws -> FundamentalValue
        where T : RawRepresentable, T.RawValue == String {
            return try getValue(on: result, fromTable: table.rawValue, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }

    public func getValue(on result: ColumnResultConvertible,
                         fromTable table: String,
                         where condition: Condition? = nil,
                         orderBy orderList: [OrderBy]? = nil,
                         limit: Limit? = nil,
                         offset: Offset? = nil) throws -> FundamentalValue {
        return (try getRows(on: result,
                            fromTable: table,
                            where: condition,
                            orderBy: orderList,
                            limit: 1,
                            offset: offset).first?.first) ?? FundamentalValue(nil)
    }

    public func getDistinctValue<T>(on result: ColumnResultConvertible,
                                    fromTable table: T,
                                    where condition: Condition? = nil,
                                    orderBy orderList: [OrderBy]? = nil,
                                    limit: Limit? = nil,
                                    offset: Offset? = nil) throws -> FundamentalValue
        where T : RawRepresentable, T.RawValue == String {
            return try getDistinctValue(on: result, fromTable: table.rawValue, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }

    public func getDistinctValue(on result: ColumnResultConvertible,
                                 fromTable table: String,
                                 where condition: Condition? = nil,
                                 orderBy orderList: [OrderBy]? = nil,
                                 limit: Limit? = nil,
                                 offset: Offset? = nil) throws -> FundamentalValue {
        return (try getDistinctColumn(on: result, fromTable: table).first) ?? FundamentalValue(nil)
    }
}

/// Convenient interface for selecting
public protocol SelectInterface: class {
    //TODO: Add generic property convertible to fit the type

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
    func getObjects<Object, T>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: T,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws -> [Object]
        where Object : TableDecodable, T : RawRepresentable, T.RawValue == String

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
    func getObjects<Object, T>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: T,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws -> [Object]
        where Object : TableDecodable, T : RawRepresentable, T.RawValue == String

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
    ///   - offset: Expression convertible
    /// - Returns: Table decodable objects
    /// - Throws: `Error`
    func getObject<Object, T>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: T,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        offset: Offset?) throws -> Object?
        where Object : TableDecodable, T : RawRepresentable, T.RawValue == String

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
    func getObject<Object, T>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: T,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        offset: Offset?) throws -> Object?
        where Object : TableDecodable, T : RawRepresentable, T.RawValue == String
}

extension SelectInterface where Self: Core {
    public func getObjects<Object, T>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: T,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws -> [Object]
        where Object : TableDecodable, T : RawRepresentable, T.RawValue == String {
            return try getObjects(on: propertyConvertibleList, fromTable: table.rawValue, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }

    public func getObjects<Object: TableDecodable>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: String,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws -> [Object] {
        let select = try Select(with: self, on: propertyConvertibleList, table: table, isDistinct: false)
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

    public func getObjects<Object, T>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: T,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws -> [Object]
        where Object : TableDecodable, T : RawRepresentable, T.RawValue == String {
        return try getObjects(on: propertyConvertibleList.isEmpty ? Object.Properties.all : propertyConvertibleList,
                              fromTable: table.rawValue,
                              where: condition,
                              orderBy: orderList,
                              limit: limit,
                              offset: offset)
    }

    public func getObject<Object, T>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: T,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        offset: Offset? = nil) throws -> Object?
        where Object : TableDecodable, T : RawRepresentable, T.RawValue == String {
            return try getObject(on: propertyConvertibleList, fromTable: table.rawValue, where: condition, orderBy: orderList, offset: offset)
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

    public func getObject<Object, T>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: T,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        offset: Offset? = nil) throws -> Object?
        where Object : TableDecodable, T : RawRepresentable, T.RawValue == String {
        return try getObject(on: propertyConvertibleList.isEmpty ? Object.Properties.all : propertyConvertibleList,
                             fromTable: table.rawValue,
                             where: condition,
                             orderBy: orderList,
                             offset: offset)
    }
}

/// Convenient interface for table related operation
public protocol TableInterface: class {
    /// Create table, related indexes and constraints with specific type
    ///
    /// Note that it will create defined indexes automatically.
    /// The name of index is `"\(tableName)\(indexSubfixName)"` while `indexSubfixName` is defined by `IndexBinding`.
    /// BUT, it will not drop the undefined indexes. You should drop it manually.
    ///
    /// Note that it will add the newly defined column automatically.
    /// AND, it will skip the undefined column. It can be very developer-friendly while upgrading your database column.
    ///
    /// Note that it will run embedded transaction
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - name: Table name.
    ///   - rootType: Type of table encodable object
    /// - Throws: `Error`
    func create<Root: TableDecodable>(table name: String, of rootType: Root.Type) throws
    
    /// Create table, related indexes and constraints with specific type
    ///
    /// Note that it will create defined indexes automatically.
    /// The name of index is `"\(tableName)\(indexSubfixName)"` while `indexSubfixName` is defined by `IndexBinding`.
    /// BUT, it will not drop the undefined indexes. You should drop it manually.
    ///
    /// Note that it will add the newly defined column automatically.
    /// AND, it will skip the undefined column. It can be very developer-friendly while upgrading your database column.
    ///
    /// Note that it will run embedded transaction
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - name: Table name.
    ///   - rootType: Type of table encodable object
    /// - Throws: `Error`
    func create<Root, T>(table name: T, of rootType: Root.Type) throws where Root : TableDecodable, T : RawRepresentable, T.RawValue == String

    /// Create virtual table and constraints with specific type
    ///
    /// Note that it will run embedded transaction
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - name: Table name.
    ///   - rootType: Type of table encodable object
    /// - Throws: `Error`
    func create<Root: TableDecodable>(virtualTable name: String, of rootType: Root.Type) throws

    /// Create virtual table and constraints with specific type
    ///
    /// Note that it will run embedded transaction
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - name: Table name.
    ///   - rootType: Type of table encodable object
    /// - Throws: `Error`
    func create<Root, T>(virtualTable name: T, of rootType: Root.Type) throws where Root : TableDecodable, T : RawRepresentable, T.RawValue == String

    /// Create table manually
    ///
    /// - Parameters:
    ///   - name: Table name
    ///   - columnDefList: WINQ column definition list
    ///   - constraintList: WINQ constraint list.
    /// - Throws: `Error`
    func create(table name: String, with columnDefList: [ColumnDef], and constraintList: [TableConstraint]?) throws
    
    /// Create table manually
    ///
    /// - Parameters:
    ///   - name: Table name
    ///   - columnDefList: WINQ column definition list
    ///   - constraintList: WINQ constraint list.
    /// - Throws: `Error`
    func create<T>(table name: T, with columnDefList: [ColumnDef], and constraintList: [TableConstraint]?) throws where T : RawRepresentable, T.RawValue == String

    /// Create table manually
    ///
    /// - Parameters:
    ///   - name: Table name
    ///   - columnDefList: WINQ column definition list
    ///   - constraintList: WINQ constraint list.
    /// - Throws: `Error`
    func create(table name: String, with columnDefList: ColumnDef..., and constraintList: [TableConstraint]?) throws

    /// Create table manually
    ///
    /// - Parameters:
    ///   - name: Table name
    ///   - columnDefList: WINQ column definition list
    ///   - constraintList: WINQ constraint list.
    /// - Throws: `Error`
    func create<T>(table name: T, with columnDefList: ColumnDef..., and constraintList: [TableConstraint]?) throws where T : RawRepresentable, T.RawValue == String

    /// Create new column
    ///
    /// - Parameters:
    ///   - columnDef: WINQ column definition
    ///   - table: Table name
    /// - Throws: `Error`
    func addColumn(with columnDef: ColumnDef, forTable table: String) throws

    /// Create new column
    ///
    /// - Parameters:
    ///   - columnDef: WINQ column definition
    ///   - table: Table name
    /// - Throws: `Error`
    func addColumn<T>(with columnDef: ColumnDef, forTable table: T) throws where T : RawRepresentable, T.RawValue == String

    /// Drop table
    ///
    /// - Parameter name: Table name
    /// - Throws: `Erro`
    func drop(table name: String) throws

    /// Drop table
    ///
    /// - Parameter name: Table name
    /// - Throws: `Erro`
    func drop<T>(table name: T) throws where T : RawRepresentable, T.RawValue == String

    /// Create index manually
    ///
    /// - Parameters:
    ///   - name: Index name
    ///   - columnIndexConvertibleList: WINQ column index list
    ///   - table: Table name
    /// - Throws: `Error`
    func create(index name: String,
                with columnIndexConvertibleList: [ColumnIndexConvertible],
                forTable table: String) throws
    
    /// Create index manually
    ///
    /// - Parameters:
    ///   - name: Index name
    ///   - columnIndexConvertibleList: WINQ column index list
    ///   - table: Table name
    /// - Throws: `Error`
    func create<T>(index name: String,
                   with columnIndexConvertibleList: [ColumnIndexConvertible],
                   forTable table: T) throws
                   where T : RawRepresentable, T.RawValue == String

    /// Create index manually
    ///
    /// - Parameters:
    ///   - name: Index name
    ///   - columnIndexConvertibleList: WINQ column index list
    ///   - table: Table name
    /// - Throws: `Error`
    func create(index name: String,
                with columnIndexConvertibleList: ColumnIndexConvertible...,
                forTable table: String) throws

    /// Create index manually
    ///
    /// - Parameters:
    ///   - name: Index name
    ///   - columnIndexConvertibleList: WINQ column index list
    ///   - table: Table name
    /// - Throws: `Error`
    func create<T>(index name: String,
                   with columnIndexConvertibleList: ColumnIndexConvertible...,
                   forTable table: T) throws
                   where T : RawRepresentable, T.RawValue == String

    /// Drop index
    ///
    /// - Parameter name: Index name
    /// - Throws: `Error`
    func drop(index name: String) throws
}

extension TableInterface where Self: Core {
    public func create<Root, T>(
        table name: T,
        of rootType: Root.Type) throws
        where Root : TableDecodable, T : RawRepresentable, T.RawValue == String {
            return try create(table: name.rawValue, of: rootType)
    }

    public func create<Root: TableDecodable>(
        table name: String,
        of rootType: Root.Type) throws {
        try run(embeddedTransaction: {
            let orm = Root.CodingKeys.objectRelationalMapping
            if try isTableExists(name) {
                var columnNames: [String] = []
                do {
                    let statementPragma = StatementPragma().pragma(.tableInfo, to: name)
                    let recyclableHandleStatement: RecyclableHandleStatement = try prepare(statementPragma)
                    let handleStatement = recyclableHandleStatement.raw
                    while try handleStatement.step() {
                        let columnName: String = handleStatement.columnValue(atIndex: 1)
                        columnNames.append(columnName)
                    }
                }
                var keys = orm.allKeys
                for columnName in columnNames {
                    if let index = keys.index(where: { (key) -> Bool in
                        return key.stringValue.caseInsensitiveCompare(columnName) == ComparisonResult.orderedSame
                    }) {
                        keys.remove(at: index)
                    } else {
                        Error.warning("Skip column named [\(columnName)] for table [\(name)]")
                    }
                }
                try keys.forEach {
                    try exec(StatementAlterTable().alter(table: name).addColumn(with: orm.generateColumnDef(with: $0)))
                }
            } else {
                try exec(orm.generateCreateTableStatement(named: name))
            }
            try orm.generateCreateIndexStatements(onTable: name)?.forEach { try exec($0) }
        })
    }

    public func create<Root, T>(virtualTable name: T, of rootType: Root.Type) throws
        where Root : TableDecodable, T : RawRepresentable, T.RawValue == String {
            return try create(virtualTable: name.rawValue, of: rootType)
    }

    public func create<Root: TableDecodable>(virtualTable name: String, of rootType: Root.Type) throws {
        try run(transaction: {
            try exec(rootType.CodingKeys.objectRelationalMapping.generateCreateVirtualTableStatement(named: name))
        })
    }

    public func create(table name: String,
                       with columnDefList: ColumnDef...,
                       and constraintList: [TableConstraint]? = nil) throws {
        try create(table: name, with: columnDefList, and: constraintList)
    }

    public func create<T>(table name: T,
                       with columnDefList: ColumnDef...,
        and constraintList: [TableConstraint]? = nil) throws
        where T : RawRepresentable, T.RawValue == String {
        try create(table: name.rawValue, with: columnDefList, and: constraintList)
    }

    public func create<T>(table name: T,
                          with columnDefList: [ColumnDef],
                          and constraintList: [TableConstraint]? = nil) throws
                        where T : RawRepresentable, T.RawValue == String {
            return try create(table: name.rawValue, with: columnDefList, and: constraintList)
    }

    public func create(table name: String,
                       with columnDefList: [ColumnDef],
                       and constraintList: [TableConstraint]? = nil) throws {
        try exec(StatementCreateTable().create(table: name, with: columnDefList, and: constraintList))
    }

    public func addColumn<T>(with columnDef: ColumnDef, forTable table: T) throws
        where T : RawRepresentable, T.RawValue == String {
        try addColumn(with: columnDef, forTable: table.rawValue)
    }

    public func addColumn(with columnDef: ColumnDef, forTable table: String) throws {
        try exec(StatementAlterTable().alter(table: table).addColumn(with: columnDef))
    }

    public func drop<T>(table name: T) throws
        where T : RawRepresentable, T.RawValue == String {
            try drop(table: name.rawValue)
    }

    public func drop(table name: String) throws {
        try exec(StatementDropTable().drop(table: name))
    }

    public func create<T>(index name: String,
                          with columnIndexConvertibleList: ColumnIndexConvertible...,
                          forTable table: T) throws
                          where T : RawRepresentable, T.RawValue == String {
            try create(index: name, with: columnIndexConvertibleList, forTable: table.rawValue)
    }

    public func create(index name: String,
                       with columnIndexConvertibleList: ColumnIndexConvertible...,
                       forTable table: String) throws {
        try create(index: name, with: columnIndexConvertibleList, forTable: table)
    }

    public func create(index name: String,
                       with columnIndexConvertibleList: [ColumnIndexConvertible],
                       forTable table: String) throws {
        try exec(StatementCreateIndex().create(index: name).on(table: table, indexesBy: columnIndexConvertibleList))
    }

    public func create<T>(index name: String,
                          with columnIndexConvertibleList: [ColumnIndexConvertible],
                          forTable table: T) throws
                          where T : RawRepresentable, T.RawValue == String {
        try create(index: name, with: columnIndexConvertibleList, forTable: table.rawValue)
    }

    public func drop(index name: String) throws {
        try exec(StatementDropIndex().drop(index: name))
    }
}
