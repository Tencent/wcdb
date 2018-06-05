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

/// ChainCall interface for inserting
public protocol InsertChainCallInterface: class {

    /// Prepare chain call for inserting of `TableEncodable` object
    ///
    /// - Parameters:
    ///   - cls: Type of table codable object
    ///   - table: Table name
    /// - Returns: `Insert`
    /// - Throws: `Error`
    func prepareInsert<Root: TableEncodable>(of cls: Root.Type, intoTable table: String) throws -> Insert

    /// Prepare chain call for inserting or replacing of `TableEncodable` object
    ///
    /// - Parameters:
    ///   - cls: Type of table codable object
    ///   - table: Table name
    /// - Returns: `Insert`
    /// - Throws: `Error`
    func prepareInsertOrReplace<Root: TableEncodable>(of cls: Root.Type, intoTable table: String) throws -> Insert

    /// Prepare chain call for inserting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list 
    ///   - table: Table name
    /// - Returns: `Insert`
    /// - Throws: `Error`
    func prepareInsert(on propertyConvertibleList: PropertyConvertible..., intoTable table: String) throws -> Insert

    /// Prepare chain call for inserting or replacing on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Returns: `Insert`
    /// - Throws: `Error`
    func prepareInsertOrReplace(on propertyConvertibleList: PropertyConvertible...,
                                intoTable table: String) throws -> Insert

    /// Prepare chain call for inserting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Returns: `Insert`
    /// - Throws: `Error`
    func prepareInsert(on propertyConvertibleList: [PropertyConvertible],
                       intoTable table: String) throws -> Insert

    /// Prepare chain call for inserting or replacing on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Returns: `Insert`
    /// - Throws: `Error`
    func prepareInsertOrReplace(on propertyConvertibleList: [PropertyConvertible],
                                intoTable table: String) throws -> Insert
}

extension InsertChainCallInterface where Self: Core {
    public func prepareInsert<Root: TableEncodable>(of cls: Root.Type, intoTable table: String) throws -> Insert {
        return try Insert(with: self, named: table, on: cls.Properties.all, isReplace: false)
    }

    public func prepareInsertOrReplace<Root: TableEncodable>(
        of cls: Root.Type,
        intoTable table: String) throws -> Insert {
        return try Insert(with: self, named: table, on: cls.Properties.all, isReplace: true)
    }

    public func prepareInsert(on propertyConvertibleList: PropertyConvertible...,
                              intoTable table: String) throws -> Insert {
        return try prepareInsert(on: propertyConvertibleList, intoTable: table)
    }

    public func prepareInsertOrReplace(on propertyConvertibleList: PropertyConvertible...,
                                       intoTable table: String) throws -> Insert {
        return try prepareInsertOrReplace(on: propertyConvertibleList, intoTable: table)
    }

    public func prepareInsert(on propertyConvertibleList: [PropertyConvertible],
                              intoTable table: String) throws -> Insert {
        return try Insert(with: self, named: table, on: propertyConvertibleList, isReplace: false)
    }

    public func prepareInsertOrReplace(on propertyConvertibleList: [PropertyConvertible],
                                       intoTable table: String) throws -> Insert {
        return try Insert(with: self, named: table, on: propertyConvertibleList, isReplace: true)
    }
}

/// ChainCall interface for deleting
public protocol DeleteChainCallInterface: class {

    /// Prepare chain call for deleting on specific properties
    ///
    /// - Parameter table: Table name
    /// - Returns: `Delete`
    /// - Throws: `Error`
    func prepareDelete(fromTable table: String) throws -> Delete
}

extension DeleteChainCallInterface where Self: Core {
    public func prepareDelete(fromTable table: String) throws -> Delete {
        return try Delete(with: self, andTableName: table)
    }
}

/// ChainCall interface for updating
public protocol UpdateChainCallInterface: class {

    /// Prepare chain call for updating on specific properties
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Update`
    /// - Throws: `Error`
    func prepareUpdate(table: String, on propertyConvertibleList: PropertyConvertible...) throws -> Update

    /// Prepare chain call for updating on specific properties
    ///
    /// - Parameters:
    ///   - table: Table name
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Update`
    /// - Throws: `Error`
    func prepareUpdate(table: String, on propertyConvertibleList: [PropertyConvertible]) throws -> Update
}

extension UpdateChainCallInterface where Self: Core {
    public func prepareUpdate(table: String, on propertyConvertibleList: PropertyConvertible...) throws -> Update {
        return try prepareUpdate(table: table, on: propertyConvertibleList)
    }

    public func prepareUpdate(table: String, on propertyConvertibleList: [PropertyConvertible]) throws -> Update {
        return try Update(with: self, on: propertyConvertibleList, andTable: table)
    }
}

/// ChainCall interface for row-selecting
public protocol RowSelectChainCallInterface: class {

    /// Prepare chain call for row-selecting on specific column results
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: `ColumnResult` list
    ///   - tables: Table name list
    ///   - isDistinct: Is distinct or not
    /// - Returns: `RowSelect`
    /// - Throws: `Error`
    func prepareRowSelect(on columnResultConvertibleList: ColumnResultConvertible...,
                          fromTables tables: [String],
                          isDistinct: Bool) throws -> RowSelect

    /// Prepare chain call for row-selecting on specific column results
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: `ColumnResult` list
    ///   - tables: Table name list
    ///   - isDistinct: Is distinct or not
    /// - Returns: `RowSelect`
    /// - Throws: `Error`
    func prepareRowSelect(on columnResultConvertibleList: [ColumnResultConvertible],
                          fromTables tables: [String],
                          isDistinct: Bool) throws -> RowSelect

    /// Prepare chain call for row-selecting on specific column results
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: `ColumnResult` list
    ///   - tables: Table name
    ///   - isDistinct: Is distinct or not
    /// - Returns: `RowSelect`
    /// - Throws: `Error`
    func prepareRowSelect(on columnResultConvertibleList: ColumnResultConvertible...,
                          fromTable table: String,
                          isDistinct: Bool) throws -> RowSelect

    /// Prepare chain call for row-selecting on specific column results
    ///
    /// - Parameters:
    ///   - columnResultConvertibleList: `ColumnResult` list
    ///   - tables: Table name
    ///   - isDistinct: Is distinct or not
    /// - Returns: `RowSelect`
    /// - Throws: `Error`
    func prepareRowSelect(on columnResultConvertibleList: [ColumnResultConvertible],
                          fromTable table: String,
                          isDistinct: Bool) throws -> RowSelect
}

extension RowSelectChainCallInterface where Self: Core {
    public func prepareRowSelect(on columnResultConvertibleList: ColumnResultConvertible...,
                                 fromTables tables: [String],
                                 isDistinct: Bool = false) throws -> RowSelect {
        return try prepareRowSelect(on: columnResultConvertibleList.isEmpty ?
            [Column.all] : columnResultConvertibleList,
                                    fromTables: tables,
                                    isDistinct: isDistinct)
    }

    public func prepareRowSelect(on columnResultConvertibleList: [ColumnResultConvertible],
                                 fromTables tables: [String],
                                 isDistinct: Bool = false) throws -> RowSelect {
        return try RowSelect(with: self, results: columnResultConvertibleList, tables: tables, isDistinct: isDistinct)
    }

    public func prepareRowSelect(on columnResultConvertibleList: ColumnResultConvertible...,
                                 fromTable table: String,
                                 isDistinct: Bool = false) throws -> RowSelect {
        return try prepareRowSelect(on: columnResultConvertibleList.isEmpty ?
            [Column.all] : columnResultConvertibleList,
                                    fromTable: table,
                                    isDistinct: isDistinct)
    }

    public func prepareRowSelect(on columnResultConvertibleList: [ColumnResultConvertible],
                                 fromTable table: String,
                                 isDistinct: Bool = false) throws -> RowSelect {
        return try RowSelect(with: self,
                             results: columnResultConvertibleList,
                             tables: [table],
                             isDistinct: isDistinct)
    }
}

/// ChainCall interface for selecting
public protocol SelectChainCallInterface: class {

    /// Prepare chain call for selecting of `TableDecodable` object
    ///
    /// - Parameters:
    ///   - cls: Type of table decodable object
    ///   - table: Table name
    ///   - isDistinct: Is distinct or not
    /// - Returns: `Select`
    /// - Throws: `Error`
    func prepareSelect<Root: TableDecodable>(of cls: Root.Type,
                                             fromTable table: String,
                                             isDistinct: Bool) throws -> Select

    /// Prepare chain call for selecting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    ///   - isDistinct: Is distinct or not
    /// - Returns: `Select`
    /// - Throws: `Error`
    func prepareSelect(on propertyConvertibleList: PropertyConvertible...,
                       fromTable table: String,
                       isDistinct: Bool) throws -> Select

    /// Prepare chain call for selecting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    ///   - isDistinct: Is distinct or not
    /// - Returns: `Select`
    /// - Throws: `Error`
    func prepareSelect(on propertyConvertibleList: [PropertyConvertible],
                       fromTable table: String,
                       isDistinct: Bool) throws -> Select
}

extension SelectChainCallInterface where Self: Core {
    public func prepareSelect<Root: TableDecodable>(of cls: Root.Type,
                                                    fromTable table: String,
                                                    isDistinct: Bool = false) throws -> Select {
        return try Select(with: self, on: cls.Properties.all, table: table, isDistinct: isDistinct)
    }

    public func prepareSelect(on propertyConvertibleList: PropertyConvertible...,
                              fromTable table: String,
                              isDistinct: Bool = false) throws -> Select {
        return try prepareSelect(on: propertyConvertibleList,
                                 fromTable: table,
                                 isDistinct: isDistinct)
    }

    public func prepareSelect(on propertyConvertibleList: [PropertyConvertible],
                              fromTable table: String,
                              isDistinct: Bool = false) throws -> Select {
        return try Select(with: self, on: propertyConvertibleList, table: table, isDistinct: isDistinct)
    }
}

/// ChainCall interface for multi-selecting
public protocol MultiSelectChainCallInterface: class {

    /// Prepare chain call for multi-selecting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - tables: Table name list
    /// - Returns: `MultiSelect`
    /// - Throws: `Error`
    func prepareMultiSelect(on propertyConvertibleList: PropertyConvertible...,
                            fromTables tables: [String]) throws -> MultiSelect

    /// Prepare chain call for multi-selecting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - tables: Table name list
    /// - Returns: `MultiSelect`
    /// - Throws: `Error`
    func prepareMultiSelect(on propertyConvertibleList: [PropertyConvertible],
                            fromTables tables: [String]) throws -> MultiSelect
}

extension MultiSelectChainCallInterface where Self: Core {
    public func prepareMultiSelect(on propertyConvertibleList: PropertyConvertible...,
                                   fromTables tables: [String]) throws -> MultiSelect {
        return try prepareMultiSelect(on: propertyConvertibleList, fromTables: tables)
    }

    public func prepareMultiSelect(on propertyConvertibleList: [PropertyConvertible],
                                   fromTables tables: [String]) throws -> MultiSelect {
        return try MultiSelect(with: self, on: propertyConvertibleList, tables: tables)
    }
}
