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

/// Convenient table interface
extension Table {

    convenience init(withDatabase database: Database, named name: String, of type: Root.Type = Root.self) where Root: WCTTableCoding {
        self.init(withDatabase: database, named: name)
    }
}

internal extension Table {

    func internalInsert(_ objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil, onConflict action: ConflictAction? = nil) throws where Object: WCTTableCoding {
        let insert = Insert(with: try self.database.getHandle(writeHint: true), named: self.name, on: propertyConvertibleList, onConflict: action)
        return try insert.execute(with: objects)
    }

    func internalUpdate(on propertyConvertibleList: [PropertyConvertible],
                        with object: Object,
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws where Object: WCTTableCoding {
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
                           offset: Offset? = nil) throws -> [Object] where Object: WCTTableCoding {
        let select = Select(with: try self.database.getHandle(writeHint: false),
                            on: propertyConvertibleList.isEmpty ? Object.allProperties() : propertyConvertibleList,
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

extension Table: InsertTableInterfaceForObjc where Root: WCTTableCoding {

    /// Execute inserting with `WCTTableCoding` object on specific(or all) properties
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Throws: `Error`
    public func insert(_ objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil) throws where Object: WCTTableCoding {
        try internalInsert(objects, on: propertyConvertibleList)
    }

    /// Execute inserting or replacing with `WCTTableCoding` object on specific(or all) properties.
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
    public func insertOrReplace(_ objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil) throws where Object: WCTTableCoding {
        try internalInsert(objects, on: propertyConvertibleList, onConflict: .Replace)
    }

    /// Execute inserting or ignoring with `WCTTableCoding` object on specific(or all) properties.
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
    public func insertOrIgnore(_ objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil) throws where Object: WCTTableCoding {
        try internalInsert(objects, on: propertyConvertibleList, onConflict: .Ignore)
    }

    /// Execute inserting with `WCTTableCoding` object on specific(or all) properties
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Throws: `Error`
    public func insert(_ objects: Object..., on propertyConvertibleList: [PropertyConvertible]? = nil) throws where Object: WCTTableCoding {
        try internalInsert(objects, on: propertyConvertibleList)
    }

    /// Execute inserting or replacing with `WCTTableCoding` object on specific(or all) properties.
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
    public func insertOrReplace(_ objects: Object..., on propertyConvertibleList: [PropertyConvertible]? = nil) throws where Object: WCTTableCoding {
        try internalInsert(objects, on: propertyConvertibleList, onConflict: .Replace)
    }

    /// Execute inserting or ignoring with `WCTTableCoding` object on specific(or all) properties.
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
    public func insertOrIgnore(_ objects: Object..., on propertyConvertibleList: [PropertyConvertible]? = nil) throws where Object: WCTTableCoding {
        try internalInsert(objects, on: propertyConvertibleList, onConflict: .Ignore)
    }
}

extension Table: UpdateTableInterfaceForObjc where Root: WCTTableCoding {

    /// Execute updating with `WCTTableCoding` object on specific(or all) properties.
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

    /// Execute updating with `WCTTableCoding` object on specific(or all) properties.
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
    ///   - object: Table encodable object
    ///   - condition: Expression convertible
    ///   - orderList: Order convertible list
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Throws: `Error`
    public func update(on propertyConvertibleList: PropertyConvertible...,
                       with row: [ColumnEncodable],
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
                       with row: [ColumnEncodable],
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

extension Table: SelectTableInterfaceForObjc where Root: WCTTableCoding {

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
        return try getObjects(on: propertyConvertibleList.isEmpty ? Object.allProperties() : propertyConvertibleList,
                              where: condition,
                              orderBy: orderList,
                              limit: 1,
                              offset: offset).first
    }
}

extension Table: TableInsertChainCallInterfaceForObjc {
    public func prepareInsert<Root: WCTTableCoding>(of cls: Root.Type) throws -> Insert {
        return Insert(with: try self.database.getHandle(writeHint: true), named: name, on: cls.allProperties())
    }

    public func prepareInsertOrReplace<Root: WCTTableCoding>(of cls: Root.Type) throws -> Insert {
            return Insert(with: try self.database.getHandle(writeHint: true), named: name, on: cls.allProperties(), onConflict: .Replace)
    }

    public func prepareInsertOrIgnore<Root: WCTTableCoding>(of cls: Root.Type) throws -> Insert {
            return Insert(with: try self.database.getHandle(writeHint: true), named: name, on: cls.allProperties(), onConflict: .Ignore)
    }
}

extension Table: TableSelectChainCallInterfaceForObjc {
    public func prepareSelect<Root: WCTTableCoding>(of cls: Root.Type,
                                                    isDistinct: Bool = false) throws -> Select {
        return Select(with: try self.database.getHandle(writeHint: false), on: cls.allProperties(), table: name, isDistinct: isDistinct)
    }
}

#endif
