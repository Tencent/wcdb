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

/// Convenient interface for table related operation
public protocol TableInterface: AnyObject {
    /// Create table, related indexes and constraints with specific type
    ///
    /// Note that it will create defined indexes automatically.
    /// The name of index is `"\(tableName)\(indexSuffixName)"` while `indexSuffixName` is defined by `IndexBinding`.
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
    func create(table name: String, with columnDefList: ColumnDef..., and constraintList: [TableConstraint]?) throws

    /// Get a wrapper from an existing table.
    ///
    /// - Parameters:
    ///   - name: The name of the table.
    ///   - type: A class conform to TableCodable protocol.
    /// - Returns: Table.
    /// - Throws: `Error`
    func getTable<Root: TableCodable>(
        named name: String,
        of type: Root.Type) -> Table<Root>

    /// Check the existence of the table.
    ///
    /// - Parameter table: The name of the table.
    /// - Returns: false for a non-existent table.
    /// - Throws: `Error`
    func isTableExists(_ table: String) throws -> Bool

    /// Create new column
    ///
    /// - Parameters:
    ///   - columnDef: WINQ column definition
    ///   - table: Table name
    /// - Throws: `Error`
    func addColumn(with columnDef: ColumnDef, forTable table: String) throws

    /// Drop table
    ///
    /// - Parameter name: Table name
    /// - Throws: `Erro`
    func drop(table name: String) throws

    /// Create index manually
    ///
    /// - Parameters:
    ///   - name: Index name
    ///   - indexedColumnConvertibleList: WINQ column index list
    ///   - table: Table name
    /// - Throws: `Error`
    func create(index name: String,
                with indexedColumnConvertibleList: [IndexedColumnConvertible],
                forTable table: String) throws

    /// Create index manually
    ///
    /// - Parameters:
    ///   - name: Index name
    ///   - indexedColumnConvertibleList: WINQ column index list
    ///   - table: Table name
    /// - Throws: `Error`
    func create(index name: String,
                with indexedColumnConvertibleList: IndexedColumnConvertible...,
                forTable table: String) throws

    /// Drop index
    ///
    /// - Parameter name: Index name
    /// - Throws: `Error`
    func drop(index name: String) throws
}

extension TableInterface where Self: HandleRepresentable {
    public func create<Root: TableDecodable>(
        table name: String,
        of rootType: Root.Type) throws {
            let handle = try getHandle(writeHint: true)
            let ret = WCDBBindingCreateTable(rootType.CodingKeys.objectRelationalMapping.innerBinding, name.cString, handle.cppHandle)
            if !ret {
                throw handle.getError()
            }
    }

    public func create<Root: TableDecodable>(virtualTable name: String, of rootType: Root.Type) throws {
        let handle = try getHandle(writeHint: true)
        let ret = WCDBBindingCreateVirtualTable(rootType.CodingKeys.objectRelationalMapping.innerBinding, name.cString, handle.cppHandle)
        if !ret {
            throw handle.getError()
        }
    }

    public func create(table name: String,
                       with columnDefList: ColumnDef...,
                       and constraintList: [TableConstraint]? = nil) throws {
        try create(table: name, with: columnDefList, and: constraintList)
    }

    public func create(table name: String,
                       with columnDefList: [ColumnDef],
                       and constraintList: [TableConstraint]? = nil) throws {
        try getHandle(writeHint: true).exec(StatementCreateTable().create(table: name).ifNotExists().with(columns: columnDefList).constraint(constraintList))
    }

    public func getTable<Root: TableCodable>(
        named name: String,
        of type: Root.Type = Root.self) -> Table<Root> {
        return Table<Root>(withDatabase: getDatabase(), named: name)
    }

    public func isTableExists(_ table: String) throws -> Bool {
        let handle = try getHandle(writeHint: false)
        let ret = WCDBHandleExistTable(handle.cppHandle, table)
        if !ret.hasValue {
            let error = WCDBHandleGetError(handle.cppHandle)
            throw ErrorBridge.getErrorFrom(cppError: error)
        }
        return ret.value
    }

    public func addColumn(with columnDef: ColumnDef, forTable table: String) throws {
        try getHandle(writeHint: true).exec(StatementAlterTable().alter(table: table).addColumn(with: columnDef))
    }

    public func drop(table name: String) throws {
        try getHandle(writeHint: true).exec(StatementDropTable().drop(table: name).ifExists())
    }

    public func create(index name: String,
                       with indexedColumnConvertibleList: IndexedColumnConvertible...,
                       forTable table: String) throws {
        try create(index: name, with: indexedColumnConvertibleList, forTable: table)
    }

    public func create(index name: String,
                       with indexedColumnConvertibleList: [IndexedColumnConvertible],
                       forTable table: String) throws {
        try getHandle(writeHint: true).exec(StatementCreateIndex().create(index: name).on(table: table).ifNotExists().indexesBy( indexedColumnConvertibleList))
    }

    public func drop(index name: String) throws {
        try getHandle(writeHint: true).exec(StatementDropIndex().drop(index: name).ifExists())
    }
}
