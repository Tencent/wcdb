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
public final class HandleStatement {
    private var recyclableStmt: RecyclableCPPHandleStatement
    internal var stmt: CPPHandleStatement

    internal let handle: Handle

    internal init(with stmt: CPPHandleStatement, and handle: Handle) {
        self.recyclableStmt = ObjectBridge.createRecyclableCPPObject(stmt)
        self.stmt = stmt
        self.handle = handle
    }

    deinit {
        finalize()
    }

    /// The wrapper of `sqlite3_step`
    ///
    /// - Returns: True means you can continue stepping while false means the stepping has been completed.
    /// - Throws: `Error`
    @discardableResult
    public func step() throws -> Bool {
        if !WCDBHandleStatementStep(stmt) {
            throw handle.getError()
        }
        return !WCDBHandleStatementIsDone(stmt)
    }

    /// The wrapper of `sqlite3_reset`
    ///
    /// - Throws: `Error`
    public func reset() {
        WCDBHandleStatementReset(stmt)
    }

    /// The wrapper of `sqlite3_finalize`
    ///
    /// - Throws: `Error`
    public func finalize() {
        WCDBHandleStatementFinalize(stmt)
    }

    /// The number of changed rows in the most recent call.
    /// It should be called after executing successfully
    public var changes: Int {
        return Int(handle.changes)
    }

    /// The wrapper of `sqlite3_stmt_readonly`
    ///
    /// - Throws: `Error`
    public var isReadOnly: Bool {
        return WCDBHandleStatementIsReadOnly(stmt)
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
    public func bind<TableEncodableType: WCTTableCoding>(
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
            hashedKeys[args.0.codingTableKey!.stringValue.hashValue] = args.toIndex
        }
        let encoder = TableEncoder(hashedKeys, on: self)
        try object.encode(to: encoder)
    }
    public func bind<TableEncodableType: WCTTableCoding>(
        _ indexedPropertyConvertibleList: [(_: PropertyConvertible, toIndex: Int)],
        of object: TableEncodableType) throws {
        for args in indexedPropertyConvertibleList {
            assert(args.0.wctProperty != nil, "WCTProperty should not be failed. If you think it's a bug, please report an issue to us.")
            WCTAPIBridge.bindProperty(args.0.wctProperty!, ofObject: object, to: Int32(truncatingIfNeeded: args.toIndex), with: stmt)
        }
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
            hashedKeys[propertyConvertible.codingTableKey!.stringValue.hashValue] = index + 1
        }
        let encoder = TableEncoder(hashedKeys, on: self)
        try object.encode(to: encoder)
    }
    public func bind<TableEncodableType: WCTTableCoding>(
        _ propertyConvertibleList: [PropertyConvertible],
        of object: TableEncodableType) throws {
        WCTAPIBridge.bindProperties(propertyConvertibleList.asWCTBridgeProperties(), ofObject: object, with: stmt)
    }

    /// The wrapper of `sqlite3_bind_*` for binding column encodable object.
    ///
    /// - Parameters:
    ///   - value: Column encodable object
    ///   - index: Begin with 1
    public func bind(_ value: ColumnEncodable?, toIndex index: Int) {
        guard let bindingValue = value?.archivedValue() else {
            bindNull(toIndex: index)
            return
        }
        bind(bindingValue, toIndex: index)
    }

    public func bind(_ value: Value, toIndex index: Int) {
        switch value.type {
        case .integer32:
            bind(value.int32Value, toIndex: index)
        case .integer64:
            bind(value.int64Value, toIndex: index)
        case .float:
            bind(value.doubleValue, toIndex: index)
        case .text:
            bind(value.stringValue, toIndex: index)
        case .BLOB:
            bind(value.dataValue, toIndex: index)
        case .null:
            bindNull(toIndex: index)
        }
    }

    public func bind(_ value: Int32, toIndex index: Int) {
        WCDBHandleStatementBindInteger(stmt, Int32(index), Int64(value))
    }

    public func bind(_ value: Int64, toIndex index: Int) {
        WCDBHandleStatementBindInteger(stmt, Int32(index), value)
    }

    public func bind(_ value: Double, toIndex index: Int) {
        WCDBHandleStatementBindDouble(stmt, Int32(index), value)
    }

    public func bind(_ value: String, toIndex index: Int) {
        WCDBHandleStatementBindText(stmt, Int32(index), value)
    }

    public func bind(_ value: Data, toIndex index: Int) {
        value.withUnsafeBytes {buffer in
            WCDBHandleStatementBindBlob(stmt, Int32(index), buffer.bindMemory(to: UInt8.self).baseAddress!, UInt64(value.count))
        }
    }

    internal func bindNull(toIndex index: Int) {
        WCDBHandleStatementBindNull(stmt, Int32(index))
    }

    public func columnValue(atIndex index: Int) -> Value {
        switch columnType(atIndex: index) {
        case .integer32:
            return Value(columnValue(atIndex: index, of: Int32.self))
        case .integer64:
            return Value(columnValue(atIndex: index, of: Int64.self))
        case .float:
            return Value(columnValue(atIndex: index, of: Double.self))
        case .text:
            return Value(columnValue(atIndex: index, of: String.self))
        case .BLOB:
            return Value(columnValue(atIndex: index, of: Data.self))
        case .null:
            return Value(nil)
        }
    }

    /// The wrapper of `sqlite3_column_*` for getting column decodable value.
    ///
    /// - Parameters:
    ///   - index: Begin with 0
    ///   - type: Type of column codable object
    /// - Returns: Same as type
    public func value(atIndex index: Int, of type: ColumnDecodable.Type) -> ColumnDecodable? {
        guard columnType(atIndex: index) != .null else {
            return nil
        }
        return type.init(with: columnValue(atIndex: index))
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
        guard columnType(atIndex: index) != .null else {
            return nil
        }
        return type.init(with: columnValue(atIndex: index))
    }

    /// The wrapper of `sqlite3_column_*` for getting fundamentable value.
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: `Int32`, `Int64`, `Double`, `String`, `Data` or `nil` value.
    public func value(atIndex index: Int) -> Value {
        switch columnType(atIndex: index) {
        case .integer32:
            return Value(columnValue(atIndex: index, of: Int32.self))
        case .integer64:
            return Value(columnValue(atIndex: index, of: Int64.self))
        case .float:
            return Value(columnValue(atIndex: index, of: Double.self))
        case .text:
            return Value(columnValue(atIndex: index, of: String.self))
        case .BLOB:
            return Value(columnValue(atIndex: index, of: Data.self))
        case .null:
            return Value(nil)
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
        for index in 0..<columnCount() {
            if columnName(atIndex: index)==name {
                return index
            }
        }
        return nil
    }

    public func columnValue(atIndex index: Int, of type: Int32.Type = Int32.self) -> Int32 {
        return Int32(truncatingIfNeeded: WCDBHandleStatementGetInteger(stmt, Int32(index)))
    }

    public func columnValue(atIndex index: Int, of type: Int64.Type = Int64.self) -> Int64 {
        return WCDBHandleStatementGetInteger(stmt, Int32(index))
    }

    public func columnValue(atIndex index: Int, of type: Double.Type = Double.self) -> Double {
        return WCDBHandleStatementGetDouble(stmt, Int32(index))
    }

    public func columnValue(atIndex index: Int, of type: String.Type = String.self) -> String {
        guard let cString = WCDBHandleStatementGetText(stmt, Int32(index)) else {
            return ""
        }
        return String(cString: cString)
    }

    public func columnValue(atIndex index: Int, of type: Data.Type = Data.self) -> Data {
        guard let bytes = WCDBHandleStatementGetBlob(stmt, Int32(index)) else {
            return Data()
        }
        let count = Int(WCDBHandleStatementGetColumnSize(stmt, Int32(index)))
        return Data(bytes: bytes, count: count)
    }

    /// The wrapper of `sqlite3_column_count`.
    ///
    /// - Returns: Count of column result
    public func columnCount() -> Int {
        return Int(WCDBHandleStatementGetColumnCount(stmt))
    }

    /// The wrapper of `sqlite3_column_type`
    ///
    /// - Parameter name: Name of the column
    /// - Returns: Column type. For a non-exists column, `.null` will be returned.
    public func columnName(atIndex index: Int) -> String {
        guard let cString = WCDBHandleStatementGetColumnName(stmt, Int32(index)) else {
            return ""
        }
        return String(cString: cString)
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

    public func originalColumnName(atIndex index: Int) -> String {
        guard let cString = WCDBHandleStatementGetOriginalColumnName(stmt, Int32(index)) else {
            return ""
        }
        return String(cString: cString)
    }

    /// The wrapper of `sqlite3_column_table_name`.
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: The related table of column at index
    public func columnTableName(atIndex index: Int) -> String {
        guard let cString = WCDBHandleStatementGetColumnTableName(stmt, Int32(index)) else {
            return ""
        }
        return String(cString: cString)
    }

    /// The wrapper of `sqlite3_column_type`
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: Type of the column
    public func columnType(atIndex index: Int) -> ColumnType {
        switch WCDBHandleStatementGetColumnType(stmt, Int32(index)) {
        case WCDBColumnValueTypeInterger:
            return ColumnType.integer64
        case WCDBColumnValueTypeFloat:
            return ColumnType.float
        case WCDBColumnValueTypeBLOB:
            return ColumnType.BLOB
        case WCDBColumnValueTypeString:
            return ColumnType.text
        default:
            return ColumnType.null
        }
    }

    /// The row id of most recent insertion.
    public var lastInsertedRowID: Int64 {
        return handle.lasInsertedRowID
    }
}
