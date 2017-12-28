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

/// Convenient table interface
public final class Table<Root: TableCodable> {
    private let database: Database

    /// Table name
    public let name: String

    /// Table related type
    public var rootType: Root.Type {
        return Root.self
    }

    init(withDatabase database: Database, named name: String, of type: Root.Type = Root.self) {
        self.database = database
        self.name = name
    }
}

extension Table: CoreRepresentable {
    public typealias Object = Root

    /// The tag of the related database. 
    public var tag: Tag? {
        return database.tag
    }

    /// The path of the related database. 
    public var path: String {
        return database.path
    }
}

extension Table: InsertTableInterface {

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
    public func insert(objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        let insert = try Insert(with: self.database, named: self.name, on: propertyConvertibleList, isReplace: false)
        return try insert.execute(with: objects)
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
    public func insertOrReplace(objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        let insert = try Insert(with: self.database, named: self.name, on: propertyConvertibleList, isReplace: true)
        return try insert.execute(with: objects)
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
    public func insert(objects: Object..., on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        return try insert(objects: objects, on: propertyConvertibleList)
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
    public func insertOrReplace(objects: Object..., on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        return try insertOrReplace(objects: objects, on: propertyConvertibleList)
    }
}

extension Table: UpdateTableInterface {

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
        let update = try Update(with: self.database, on: propertyConvertibleList, andTable: self.name)
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
        return try update(on: propertyConvertibleList,
                          with: object,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    /// Execute updating with row on specific(or all) properties. 
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
                       with row: [ColumnEncodableBase],
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
    ///   - object: Table encodable object
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    public func update(on propertyConvertibleList: [PropertyConvertible],
                       with row: [ColumnEncodableBase],
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        let update = try Update(with: self.database, on: propertyConvertibleList, andTable: self.name)
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
        let delete = try Delete(with: self.database, andTableName: self.name)
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

extension Table: SelectTableInterface {

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
        let select = try Select(with: self.database, on: propertyConvertibleList, table: self.name, isDistinct: false)
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
        return try getObjects(on: propertyConvertibleList.isEmpty ? Object.Properties.all : propertyConvertibleList,
                              where: condition,
                              orderBy: orderList,
                              limit: limit,
                              offset: offset)
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
        return try getObjects(on: propertyConvertibleList,
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
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRowXColumn`
    /// - Throws: `Error`
    public func getRows(on columnResultConvertibleList: [ColumnResultConvertible],
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws -> FundamentalRowXColumn {
        let rowSelect = try RowSelect(with: self.database,
                                      results: columnResultConvertibleList,
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
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRowXColumn`
    /// - Throws: `Error`
    public func getRows(on columnResultConvertibleList: ColumnResultConvertible...,
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws -> FundamentalRowXColumn {
        return try getRows(on: columnResultConvertibleList.isEmpty ? [Column.any] : columnResultConvertibleList,
                           where: condition,
                           orderBy: orderList,
                           limit: limit,
                           offset: offset)
    }

    /// Get row by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRowXColumn`
    /// - Throws: `Error`
    public func getRow(on columnResultConvertibleList: ColumnResultConvertible...,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       offset: Offset? = nil) throws -> FundamentalRow {
        return try getRows(on: columnResultConvertibleList.isEmpty ? [Column.any] : columnResultConvertibleList,
                           where: condition,
                           orderBy: orderList,
                           limit: 1,
                           offset: offset).first ?? []
    }

    /// Get row by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: WINQ column result list
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalRowXColumn`
    /// - Throws: `Error`
    public func getRow(on columnResultConvertibleList: [ColumnResultConvertible],
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       offset: Offset? = nil) throws -> FundamentalRow {
        return try getRows(on: columnResultConvertibleList,
                           where: condition,
                           orderBy: orderList,
                           limit: 1,
                           offset: offset).first ?? []
    }

    /// Get column by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertible: WINQ column result
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalColumn`
    /// - Throws: `Error`
    public func getColumn(on result: ColumnResultConvertible,
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          limit: Limit? = nil,
                          offset: Offset? = nil) throws -> FundamentalColumn {
        let rowSelect = try RowSelect(with: self.database, results: [result], tables: [self.name], isDistinct: false)
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
    ///   - columnResultConvertible: WINQ column result
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalColumn`
    /// - Throws: `Error`
    public func getDistinctColumn(on result: ColumnResultConvertible,
                                  where condition: Condition? = nil,
                                  orderBy orderList: [OrderBy]? = nil,
                                  limit: Limit? = nil,
                                  offset: Offset? = nil) throws -> FundamentalColumn {
        let rowSelect = try RowSelect(with: self.database, results: [result], tables: [self.name], isDistinct: true)
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
    ///   - columnResultConvertible: WINQ column result
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalValue`
    /// - Throws: `Error`
    public func getValue(on result: ColumnResultConvertible,
                         where condition: Condition? = nil,
                         orderBy orderList: [OrderBy]? = nil,
                         limit: Limit? = nil,
                         offset: Offset? = nil) throws -> FundamentalValue {
        return (try getRows(on: result,
                            where: condition,
                            orderBy: orderList,
                            limit: 1,
                            offset: offset).first?.first) ?? FundamentalValue(nil)
    }

    /// Get distinct value by specific selecting
    ///
    /// - Parameters:
    ///   - columnResultConvertible: WINQ column result
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - offset: Expression convertible
    /// - Returns: `FundamentalValue`
    /// - Throws: `Error`
    public func getDistinctValue(on result: ColumnResultConvertible,
                                 where condition: Condition? = nil,
                                 orderBy orderList: [OrderBy]? = nil,
                                 limit: Limit? = nil,
                                 offset: Offset? = nil) throws -> FundamentalValue {
        return (try getDistinctColumn(on: result).first) ?? FundamentalValue(nil)
    }
}
