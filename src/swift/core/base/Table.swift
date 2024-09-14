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

/// Convenient table interface
public final class Table<Object: Any>: @unchecked Sendable {
    internal let database: Database
    public typealias Object = Object

    /// Table name
    public let name: String

    /// Table related type
    public var rootType: Object.Type {
        return Object.self
    }

    internal init(withDatabase database: Database, named name: String) {
        self.database = database
        self.name = name
    }

    convenience init(withDatabase database: Database, named name: String, of type: Object.Type = Object.self) where Object: TableCodable {
        self.init(withDatabase: database, named: name)
    }

    convenience init(withDatabase database: Database, named name: String, of type: Object.Type = Object.self) {
        fatalError("\(Object.self) must conform to TableCodable or WCTTableCoding")
    }
}

internal extension Table {

    func internalInsert(_ objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil, onConlict action: ConflictAction? = nil) throws where Object: TableEncodable {
        let insert = Insert(with: try self.database.getHandle(writeHint: true), named: self.name, on: propertyConvertibleList, onConflict: action)
        return try insert.execute(with: objects)
    }

    func internalUpdate(on propertyConvertibleList: [PropertyConvertible],
                        with object: Object,
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws where Object: TableEncodable {
        let update = Update(with: try self.database.getHandle(writeHint: true), on: propertyConvertibleList, andTable: self.name)
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

    func internalGetObjects(on propertyConvertibleList: [PropertyConvertible],
                           where condition: Condition? = nil,
                           orderBy orderList: [OrderBy]? = nil,
                           limit: Limit? = nil,
                           offset: Offset? = nil) throws -> [Object] where Object: TableDecodable {
        let select = Select(with: try self.database.getHandle(writeHint: false),
                            on: propertyConvertibleList.isEmpty ? Object.Properties.all : propertyConvertibleList,
                            table: self.name,
                            isDistinct: false)
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
}

extension Table: InsertTableInterface where Object: TableCodable {

    /// Execute inserting with `TableCodable` object on specific(or all) properties
    ///
    /// Note that it will run embedded transaction while objects.count>1.  
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Throws: `Error`
    public func insert(_ objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        try internalInsert(objects, on: propertyConvertibleList)
    }

    /// Execute inserting or replacing with `TableCodable` object on specific(or all) properties.  
    /// It will replace the original row while they have same primary key or row id.
    ///
    /// Note that it will run embedded transaction while objects.count>1.  
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Throws: `Error`
    public func insertOrReplace(_ objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        try internalInsert(objects, on: propertyConvertibleList, onConlict: .Replace)
    }

    /// Execute inserting or ignoring with `TableEncodable` object on specific(or all) properties.
    /// It will ignore the object while there already exists the same primary key or row id in current table.
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Throws: `Error`
    public func insertOrIgnore(_ objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        try internalInsert(objects, on: propertyConvertibleList, onConlict: .Ignore)
    }

    /// Execute inserting with `TableCodable` object on specific(or all) properties
    ///
    /// Note that it will run embedded transaction while objects.count>1.  
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Throws: `Error`
    public func insert(_ objects: Object..., on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        try internalInsert(objects, on: propertyConvertibleList)
    }

    /// Execute inserting or replacing with `TableCodable` object on specific(or all) properties.  
    /// It will replace the original row while they have same primary key or row id.
    ///
    /// Note that it will run embedded transaction while objects.count>1.  
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Throws: `Error`
    public func insertOrReplace(_ objects: Object..., on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        try internalInsert(objects, on: propertyConvertibleList, onConlict: .Replace)
    }

    /// Execute inserting or ignoring with `TableEncodable` object on specific(or all) properties.
    /// It will ignore the object while there already exists the same primary key or row id in current table.
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Throws: `Error`
    public func insertOrIgnore(_ objects: Object..., on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        try internalInsert(objects, on: propertyConvertibleList, onConlict: .Ignore)
    }
}

extension Table: UpdateTableInterface where Object: TableEncodable {

    /// Execute updating with `TableCodable` object on specific(or all) properties. 
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - object: Table encodable object
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    public func update(on propertyConvertibleList: [PropertyConvertible],
                       with object: Object,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        return try internalUpdate(on: propertyConvertibleList, with: object, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }

    /// Execute updating with `TableCodable` object on specific(or all) properties. 
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - object: Table encodable object
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    public func update(on propertyConvertibleList: PropertyConvertible...,
                       with object: Object,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        return try internalUpdate(on: propertyConvertibleList, with: object, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }

    /// Execute updating with row on specific(or all) properties. 
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - row: a row of ColumnEncodable value
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    public func update(on propertyConvertibleList: PropertyConvertible...,
                       with row: ColumnEncodable?...,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        return try update(on: propertyConvertibleList,
                          with: row,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    /// Execute updating with row on specific(or all) properties.
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - row: a row of ColumnEncodable value
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    public func update(on propertyConvertibleList: PropertyConvertible...,
                       with row: [ColumnEncodable?],
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        return try update(on: propertyConvertibleList,
                          with: row,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    /// Execute updating with row on specific(or all) properties.
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - row: a row of ColumnEncodable value
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    public func update(on propertyConvertibleList: [PropertyConvertible],
                       with row: ColumnEncodable?...,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        return try update(on: propertyConvertibleList,
                          with: row,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    /// Execute updating with row on specific(or all) properties. 
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - row: a row of ColumnEncodable value
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    public func update(on propertyConvertibleList: [PropertyConvertible],
                       with row: [ColumnEncodable?],
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        let update = Update(with: try self.database.getHandle(writeHint: true), on: propertyConvertibleList, andTable: self.name)
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

extension Table: DeleteTableInterface {

    /// Execute deleting 
    ///
    /// - Parameters:
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    public func delete(where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        let delete = Delete(with: try self.database.getHandle(writeHint: true), andTableName: self.name)
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

extension Table: SelectTableInterface where Object: TableDecodable {

    /// Get objects on specific(or all) properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: Table decodable objects
    /// - Throws: `Error`
    public func getObjects(on propertyConvertibleList: [PropertyConvertible],
                           where condition: Condition? = nil,
                           orderBy orderList: [OrderBy]? = nil,
                           limit: Limit? = nil,
                           offset: Offset? = nil) throws -> [Object] {
        return try internalGetObjects(on: propertyConvertibleList, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }

    /// Get objects on specific(or all) properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: Table decodable objects
    /// - Throws: `Error`
    public func getObjects(on propertyConvertibleList: PropertyConvertible...,
                           where condition: Condition? = nil,
                           orderBy orderList: [OrderBy]? = nil,
                           limit: Limit? = nil,
                           offset: Offset? = nil) throws -> [Object] {
        return try internalGetObjects(on: propertyConvertibleList, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }

    /// Get object on specific(or all) properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: Table decodable objects
    /// - Throws: `Error`
    public func getObject(on propertyConvertibleList: [PropertyConvertible],
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          offset: Offset? = nil) throws -> Object? {
        return try internalGetObjects(on: propertyConvertibleList,
                              where: condition,
                              orderBy: orderList,
                              limit: 1,
                              offset: offset).first
    }

    /// Get object on specific(or all) properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: Table decodable objects
    /// - Throws: `Error`
    public func getObject(on propertyConvertibleList: PropertyConvertible...,
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          offset: Offset? = nil) throws -> Object? {
        return try getObjects(on: propertyConvertibleList.isEmpty ? Object.Properties.all : propertyConvertibleList,
                              where: condition,
                              orderBy: orderList,
                              limit: 1,
                              offset: offset).first
    }
}

extension Table: RowSelectTableInterface {

    /// Get rows by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertibleList: WINQ column result list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `MultiRowsValue`
    /// - Throws: `Error`
    public func getRows(on resultColumnConvertibleList: [ResultColumnConvertible],
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws -> MultiRowsValue {
        let rowSelect = RowSelect(with: try self.database.getHandle(writeHint: false),
                                  results: resultColumnConvertibleList,
                                  tables: [self.name],
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

    /// Get rows by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertibleList: WINQ column result list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `MultiRowsValue`
    /// - Throws: `Error`
    public func getRows(on resultColumnConvertibleList: ResultColumnConvertible...,
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws -> MultiRowsValue {
        return try getRows(on: resultColumnConvertibleList.isEmpty ? [Column.all()] : resultColumnConvertibleList,
                           where: condition,
                           orderBy: orderList,
                           limit: limit,
                           offset: offset)
    }

    /// Get row by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertibleList: WINQ column result list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `MultiRowsValue`
    /// - Throws: `Error`
    public func getRow(on resultColumnConvertibleList: ResultColumnConvertible...,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       offset: Offset? = nil) throws -> OneRowValue {
        return try getRows(on: resultColumnConvertibleList.isEmpty ? [Column.all()] : resultColumnConvertibleList,
                           where: condition,
                           orderBy: orderList,
                           limit: 1,
                           offset: offset).first ?? []
    }

    /// Get row by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertibleList: WINQ column result list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `MultiRowsValue`
    /// - Throws: `Error`
    public func getRow(on resultColumnConvertibleList: [ResultColumnConvertible],
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       offset: Offset? = nil) throws -> OneRowValue {
        return try getRows(on: resultColumnConvertibleList,
                           where: condition,
                           orderBy: orderList,
                           limit: 1,
                           offset: offset).first ?? []
    }

    /// Get column by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertible: WINQ column result
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `OneColumnValue`
    /// - Throws: `Error`
    public func getColumn(on result: ResultColumnConvertible,
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          limit: Limit? = nil,
                          offset: Offset? = nil) throws -> OneColumnValue {
        let rowSelect = RowSelect(with: try self.database.getHandle(writeHint: false), results: [result], tables: [self.name], isDistinct: false)
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

    /// Get distinct column by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertible: WINQ column result
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `OneColumnValue`
    /// - Throws: `Error`
    public func getDistinctColumn(on result: ResultColumnConvertible,
                                  where condition: Condition? = nil,
                                  orderBy orderList: [OrderBy]? = nil,
                                  limit: Limit? = nil,
                                  offset: Offset? = nil) throws -> OneColumnValue {
        let rowSelect = RowSelect(with: try self.database.getHandle(writeHint: false), results: [result], tables: [self.name], isDistinct: true)
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

    /// Get value by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertible: WINQ column result
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `Value`
    /// - Throws: `Error`
    public func getValue(on result: ResultColumnConvertible,
                         where condition: Condition? = nil,
                         orderBy orderList: [OrderBy]? = nil,
                         limit: Limit? = nil,
                         offset: Offset? = nil) throws -> Value {
        return (try getRows(on: result,
                            where: condition,
                            orderBy: orderList,
                            limit: 1,
                            offset: offset).first?.first) ?? Value(nil)
    }

    /// Get distinct value by specific selecting
    ///
    /// - Parameters:
    ///   - resultColumnConvertible: WINQ column result
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `Value`
    /// - Throws: `Error`
    public func getDistinctValue(on result: ResultColumnConvertible,
                                 where condition: Condition? = nil,
                                 orderBy orderList: [OrderBy]? = nil,
                                 limit: Limit? = nil,
                                 offset: Offset? = nil) throws -> Value {
        return (try getDistinctColumn(on: result).first) ?? Value(nil)
    }
}

extension Table: TableInsertChainCallInterface {
    public func prepareInsert<Root: TableEncodable>(of cls: Root.Type) throws -> Insert {
        return Insert(with: try self.database.getHandle(writeHint: true), named: name, on: cls.Properties.all)
    }

    public func prepareInsertOrReplace<Root: TableEncodable>(
        of cls: Root.Type) throws -> Insert {
            return Insert(with: try self.database.getHandle(writeHint: true), named: name, on: cls.Properties.all, onConflict: .Replace)
    }

    public func prepareInsertOrIgnore<Root: TableEncodable>(
        of cls: Root.Type) throws -> Insert {
            return Insert(with: try self.database.getHandle(writeHint: true), named: name, on: cls.Properties.all, onConflict: .Ignore)
    }

    public func prepareInsert(on propertyConvertibleList: PropertyConvertible...) throws -> Insert {
        return try prepareInsert(on: propertyConvertibleList)
    }

    public func prepareInsertOrReplace(on propertyConvertibleList: PropertyConvertible...) throws -> Insert {
        return try prepareInsertOrReplace(on: propertyConvertibleList)
    }

    public func prepareInsertOrIgnore(on propertyConvertibleList: PropertyConvertible...) throws -> Insert {
        return try prepareInsertOrIgnore(on: propertyConvertibleList)
    }

    public func prepareInsert(on propertyConvertibleList: [PropertyConvertible]) throws -> Insert {
        return Insert(with: try self.database.getHandle(writeHint: true), named: name, on: propertyConvertibleList)
    }

    public func prepareInsertOrReplace(on propertyConvertibleList: [PropertyConvertible]) throws -> Insert {
        return Insert(with: try self.database.getHandle(writeHint: true), named: name, on: propertyConvertibleList, onConflict: .Replace)
    }

    public func prepareInsertOrIgnore(on propertyConvertibleList: [PropertyConvertible]) throws -> Insert {
        return Insert(with: try self.database.getHandle(writeHint: true), named: name, on: propertyConvertibleList, onConflict: .Ignore)
    }
}

extension Table: TableDeleteChainCallInterface {
    public func prepareDelete() throws -> Delete {
        return Delete(with: try self.database.getHandle(writeHint: true), andTableName: name)
    }
}

extension Table: TableUpdateChainCallInterface {
    public func prepareUpdate(on propertyConvertibleList: PropertyConvertible...) throws -> Update {
        return try prepareUpdate(on: propertyConvertibleList)
    }

    public func prepareUpdate(on propertyConvertibleList: [PropertyConvertible]) throws -> Update {
        return Update(with: try self.database.getHandle(writeHint: true), on: propertyConvertibleList, andTable: name)
    }
}

extension Table: TableRowSelectChainCallInterface {
    public func prepareRowSelect(on resultColumnConvertibleList: ResultColumnConvertible...,
                                 isDistinct: Bool = false) throws -> RowSelect {
        return try prepareRowSelect(on: resultColumnConvertibleList.isEmpty ?
                                    [Column.all()] : resultColumnConvertibleList,
                                    isDistinct: isDistinct)
    }

    public func prepareRowSelect(on resultColumnConvertibleList: [ResultColumnConvertible],
                                 isDistinct: Bool = false) throws -> RowSelect {
        return RowSelect(with: try self.database.getHandle(writeHint: false),
                         results: resultColumnConvertibleList,
                         tables: [name],
                         isDistinct: isDistinct)
    }
}

extension Table: TableSelectChainCallInterface {
    public func prepareSelect<Root: TableDecodable>(of cls: Root.Type,
                                                    isDistinct: Bool = false) throws -> Select {
        return Select(with: try self.database.getHandle(writeHint: false), on: cls.Properties.all, table: name, isDistinct: isDistinct)
    }

    public func prepareSelect(on propertyConvertibleList: PropertyConvertible...,
                              isDistinct: Bool = false) throws -> Select {
        return try prepareSelect(on: propertyConvertibleList,
                                 isDistinct: isDistinct)
    }

    public func prepareSelect(on propertyConvertibleList: [PropertyConvertible],
                              isDistinct: Bool = false) throws -> Select {
        return Select(with: try self.database.getHandle(writeHint: false), on: propertyConvertibleList, table: name, isDistinct: isDistinct)
    }
}
