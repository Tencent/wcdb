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

/// CoreStatement for preparing or executing WINQ sql.
public final class CoreStatement {
    private let core: Core
    private let recyclableHandleStatement: RecyclableHandleStatement

    init(with core: Core, and recyclableHandleStatement: RecyclableHandleStatement) {
        self.core = core
        self.recyclableHandleStatement = recyclableHandleStatement
    }

    private var handleStatement: HandleStatement {
        return recyclableHandleStatement.raw
    }

    /// The wrapper of `sqlite3_bind_*` for binding property of object to index.
    ///
    /// - Parameters:
    ///   - propertyConvertible: `Property` or `CodingTableKey`
    ///   - object: Table encodable object
    ///   - index: Begin with 1
    /// - Throws: `Error`
    public func bind<TableEncodableType: TableEncodable>(
        _ propertyConvertible: PropertyConvertible,
        of object: TableEncodableType,
        toIndex index: Int = 1) throws {
        try bind([(propertyConvertible, toIndex: index)], of: object)
    }

    /// The wrapper of `sqlite3_bind_*` for binding properties of object to indexes.
    ///
    /// - Parameters:
    ///   - indexedPropertyConvertibleList: Indexed `Property` or `CodingTableKey` list
    ///   - object: Table encodable object
    /// - Throws: Begin with 1
    public func bind<TableEncodableType: TableEncodable>(
        _ indexedPropertyConvertibleList: [(_: PropertyConvertible, toIndex: Int)],
        of object: TableEncodableType) throws {
        var hashedKeys: TableEncoder.HashedKey = [:]
        for args in indexedPropertyConvertibleList {
            hashedKeys[args.0.codingTableKey.stringValue.hashValue] = args.toIndex
        }
        let encoder = TableEncoder(hashedKeys, on: recyclableHandleStatement)
        try object.encode(to: encoder)
    }

    /// The wrapper of `sqlite3_bind_*` for binding properties of object.
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - object: Table encodable object
    /// - Throws: Begin with 1
    public func bind<TableEncodableType: TableEncodable>(
        _ propertyConvertibleList: [PropertyConvertible],
        of object: TableEncodableType) throws {
        var hashedKeys: TableEncoder.HashedKey = [:]
        for (index, propertyConvertible) in propertyConvertibleList.enumerated() {
            hashedKeys[propertyConvertible.codingTableKey.stringValue.hashValue] = index
        }
        let encoder = TableEncoder(hashedKeys, on: recyclableHandleStatement)
        try object.encode(to: encoder)
    }

    /// The wrapper of `sqlite3_bind_*` for binding column encodable object.
    ///
    /// - Parameters:
    ///   - value: Column encodable object
    ///   - index: Begin with 1
    public func bind(_ value: ColumnEncodable?, toIndex index: Int) {
        guard let bindingValue = value?.archivedValue() else {
            handleStatement.bind(nil, toIndex: index)
            return
        }
        handleStatement.bind(bindingValue, toIndex: index)
    }

    /// The wrapper of `sqlite3_column_*` for getting column decodable value.
    ///
    /// - Parameters:
    ///   - index: Begin with 0
    ///   - type: Type of column codable object
    /// - Returns: Same as type
    public func value(atIndex index: Int, of type: ColumnDecodable.Type) -> ColumnDecodable? {
        guard handleStatement.columnType(atIndex: index) != .null else {
            return nil
        }
        return type.init(with: handleStatement.columnValue(atIndex: index))
    }

    /// The wrapper of `sqlite3_column_*` for getting column decodable value.
    ///
    /// - Parameters:
    ///   - index: Begin with 0
    ///   - type: Type of column codable object
    /// - Returns: Same as type
    public func value<ColumnDecodableType: ColumnDecodable>(
        atIndex index: Int,
        of type: ColumnDecodableType.Type = ColumnDecodableType.self) -> ColumnDecodableType? {
        guard handleStatement.columnType(atIndex: index) != .null else {
            return nil
        }
        return type.init(with: handleStatement.columnValue(atIndex: index))
    }

    /// The wrapper of `sqlite3_column_*` for getting fundamentable value.
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: `Int32`, `Int64`, `Double`, `String`, `Data` or `nil` value.
    public func value(atIndex index: Int) -> FundamentalValue {
        switch handleStatement.columnType(atIndex: index) {
        case .integer32:
            return FundamentalValue(handleStatement.columnValue(atIndex: index, of: Int32.self))
        case .integer64:
            return FundamentalValue(handleStatement.columnValue(atIndex: index, of: Int64.self))
        case .float:
            return FundamentalValue(handleStatement.columnValue(atIndex: index, of: Double.self))
        case .text:
            return FundamentalValue(handleStatement.columnValue(atIndex: index, of: String.self))
        case .BLOB:
            return FundamentalValue(handleStatement.columnValue(atIndex: index, of: Data.self))
        case .null:
            return FundamentalValue(nil)
        }
    }

    /// The wrapper of `sqlite3_column_*` for getting column decodable value.
    ///
    /// - Parameters:
    ///   - name: Name of the column
    ///   - type: Type of column codable object
    /// - Returns: Same as type. Nil will be returned if no such a column.
    public func value<ColumnDecodableType: ColumnDecodable>(
        byName name: String,
        of type: ColumnDecodableType.Type = ColumnDecodableType.self) -> ColumnDecodableType? {
        guard let index = index(byName: name) else {
            return nil
        }
        return value(atIndex: index)
    }

    /// Get index by column name.
    ///
    /// - Parameter name: Name of the column
    /// - Returns: Index of given column name. Nil will be returned if no such a column.
    public func index(byName name: String) -> Int? {
        for index in 0..<handleStatement.columnCount() {
            if handleStatement.columnName(atIndex: index)==name {
                return index
            }
        }
        return nil
    }

    /// The wrapper of `sqlite3_step`
    ///
    /// - Returns: True means you can continue stepping while false means the stepping has been completed. 
    /// - Throws: `Error`
    @discardableResult
    public func step() throws -> Bool {
        return try handleStatement.step()
    }

    /// The wrapper of `sqlite3_reset`
    ///
    /// - Throws: `Error`
    public func reset() throws {
        try handleStatement.reset()
    }

    /// The wrapper of `sqlite3_column_type`
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: Type of the column
    public func columnType(atIndex index: Int) -> ColumnType {
        return handleStatement.columnType(atIndex: index)
    }

    /// The wrapper of `sqlite3_column_type`
    ///
    /// - Parameter name: Name of the column
    /// - Returns: Column type. For a non-exists column, `.null` will be returned.
    public func columnType(byName name: String) -> ColumnType {
        guard let index = index(byName: name) else {
            return .null
        }
        return columnType(atIndex: index)
    }

    /// The wrapper of `sqlite3_column_count`.
    ///
    /// - Returns: Count of column result
    public func columnCount() -> Int {
        return handleStatement.columnCount()
    }

    /// The wrapper of `sqlite3_column_name`.
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: Column name
    public func columnName(atIndex index: Int) -> String {
        return handleStatement.columnName(atIndex: index)
    }

    /// The wrapper of `sqlite3_column_table_name`.
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: The related table of column at index
    public func columnTableName(atIndex index: Int) -> String {
        return handleStatement.columnTableName(atIndex: index)
    }

    /// The wrapper of `sqlite3_finalize`
    ///
    /// - Throws: `Error`
    public func finalize() throws {
        try handleStatement.finalize()
    }

    /// The number of changed rows in the most recent call.  
    /// It should be called after executing successfully
    public var changes: Int {
        return handleStatement.changes
    }

    /// The row id of most recent insertion.
    public var lastInsertedRowID: Int64 {
        return handleStatement.lastInsertedRowID
    }
}

extension CoreStatement {

    /// The tag of the related database. 
    public var tag: Tag? {
        return core.tag
    }

    /// The path of the related database. 
    public var path: String {
        return core.path
    }
}
