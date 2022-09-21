//
// Created by 陈秋文 on 2022/9/12.
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

public protocol StatementInterface: AnyObject {

    /// The wrapper of `sqlite3_prepare`
    ///
    /// - Throws: `Error`
    func prepare(_ statement: Statement) throws

    /// Check if current statement is prepared
    var isPrepared: Bool { get }

    /// The wrapper of `sqlite3_step`
    ///
    /// - Returns: True means you can continue stepping while false means the stepping has been completed.
    /// - Throws: `Error`
    @discardableResult
    func step() throws -> Bool

    /// The wrapper of `sqlite3_reset`
    func reset() throws

    /// The wrapper of `sqlite3_finalize`
    func finalize()

    /// The wrapper of `sqlite3_getRawStatement()_readonly`
    var isReadOnly: Bool { get }

    /// The wrapper of `sqlite3_bind_*` for binding property of object to index.
    ///
    /// - Parameters:
    ///   - propertyConvertible: `Property` or `CodingTableKey`
    ///   - object: Table encodable object
    ///   - index: Begin with 1
    /// - Throws: `Error`
    func bind<TableEncodableType: TableEncodable>(
        _ propertyConvertible: PropertyConvertible,
        of object: TableEncodableType,
        toIndex index: Int) throws
    func bind<TableEncodableType: WCTTableCoding>(
        _ propertyConvertible: PropertyConvertible,
        of object: TableEncodableType,
        toIndex index: Int) throws

    /// The wrapper of `sqlite3_bind_*` for binding properties of object to indexes.
    ///
    /// - Parameters:
    ///   - indexedPropertyConvertibleList: Indexed `Property` or `CodingTableKey` list
    ///   - object: Table encodable object
    /// - Throws: Begin with 1
    func bind<TableEncodableType: TableEncodable>(
        _ indexedPropertyConvertibleList: [(_: PropertyConvertible, toIndex: Int)],
        of object: TableEncodableType) throws
    func bind<TableEncodableType: WCTTableCoding>(
        _ indexedPropertyConvertibleList: [(_: PropertyConvertible, toIndex: Int)],
        of object: TableEncodableType) throws

    /// The wrapper of `sqlite3_bind_*` for binding properties of object.
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - object: Table encodable object
    /// - Throws: Begin with 1
    func bind<TableEncodableType: TableEncodable>(
        _ propertyConvertibleList: [PropertyConvertible],
        of object: TableEncodableType) throws

    func bind<TableEncodableType: WCTTableCoding>(
        _ propertyConvertibleList: [PropertyConvertible],
        of object: TableEncodableType) throws

    /// The wrapper of `sqlite3_bind_*` for binding column encodable object.
    ///
    /// - Parameters:
    ///   - value: Column encodable object
    ///   - index: Begin with 1
    func bind(_ value: ColumnEncodable?, toIndex index: Int)

    func bind(_ value: Value, toIndex index: Int)
    func bind(_ value: Int32, toIndex index: Int)
    func bind(_ value: Int64, toIndex index: Int)
    func bind(_ value: Double, toIndex index: Int)
    func bind(_ value: String, toIndex index: Int)
    func bind(_ value: Data, toIndex index: Int)

    func columnValue(atIndex index: Int) -> Value

    /// The wrapper of `sqlite3_column_*` for getting column decodable value.
    ///
    /// - Parameters:
    ///   - index: Begin with 0
    ///   - type: Type of column codable object
    /// - Returns: Same as type
    func value(atIndex index: Int, of type: ColumnDecodable.Type) -> ColumnDecodable?

    /// The wrapper of `sqlite3_column_*` for getting column decodable value.
    ///
    /// - Parameters:
    ///   - name: Name of the column
    ///   - type: Type of column codable object
    /// - Returns: Same as type. Nil will be returned if no such a column.
    func value<ColumnDecodableType: ColumnDecodable>(
        byName name: String,
        of type: ColumnDecodableType.Type) -> ColumnDecodableType?

    /// The wrapper of `sqlite3_column_*` for getting fundamentable value.
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: `Int32`, `Int64`, `Double`, `String`, `Data` or `nil` value.
    func value(atIndex index: Int) -> Value

    /// Get index by column name.
    ///
    /// - Parameter name: Name of the column
    /// - Returns: Index of given column name. Nil will be returned if no such a column.
    func index(byName name: String) -> Int?

    func columnValue(atIndex index: Int) -> Int32
    func columnValue(atIndex index: Int, of type: Int32.Type) -> Int32
    func columnValue(atIndex index: Int) -> Int64
    func columnValue(atIndex index: Int, of type: Int64.Type) -> Int64
    func columnValue(atIndex index: Int) -> Double
    func columnValue(atIndex index: Int, of type: Double.Type) -> Double
    func columnValue(atIndex index: Int) -> String
    func columnValue(atIndex index: Int, of type: String.Type) -> String
    func columnValue(atIndex index: Int) -> Data
    func columnValue(atIndex index: Int, of type: Data.Type) -> Data

    /// The wrapper of `sqlite3_column_count`.
    ///
    /// - Returns: Count of column result
    func columnCount() -> Int

    /// The wrapper of `sqlite3_column_name`
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: Name of the column
    func columnName(atIndex index: Int) -> String

    /// The wrapper of `sqlite3_column_type`
    ///
    /// - Parameter name: Name of the column
    /// - Returns: Column type. For a non-exists column, `.null` will be returned.
    func columnType(byName name: String) -> ColumnType

    /// The wrapper of `sqlite3_column_type`
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: Type of the column
    func columnType(atIndex index: Int) -> ColumnType

    /// The wrapper of `sqlite3_column_table_name`.
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: Return the name of the table column from which a result column derives.
    func originalColumnName(atIndex index: Int) -> String

    /// The wrapper of `sqlite3_column_table_name`.
    ///
    /// - Parameter index: Begin with 0
    /// - Returns: Return the name of the table from which a result column derives.
    func columnTableName(atIndex index: Int) -> String
}

extension StatementInterface where Self: RawStatementmentRepresentable {
    public func prepare(_ statement: Statement) throws {
        let succeed = withExtendedLifetime(statement) {
            WCDBHandleStatementPrepare(getRawStatement(), $0.unmanagedCPPStatement)
        }
        if !succeed {
            let cppError = WCDBHandleStatementGetError(getRawStatement())
            throw ErrorBridge.getErrorFrom(cppError: cppError)
        }
    }

    public var isPrepared: Bool {
        WCDBHandleStatementCheckPrepared(getRawStatement())
    }

    @discardableResult
    public func step() throws -> Bool {
        if !WCDBHandleStatementStep(getRawStatement()) {
            let cppError = WCDBHandleStatementGetError(getRawStatement())
            throw ErrorBridge.getErrorFrom(cppError: cppError)
        }
        return !WCDBHandleStatementIsDone(getRawStatement())
    }

    public func reset() {
        WCDBHandleStatementReset(getRawStatement())
    }

    public func finalize() {
        WCDBHandleStatementFinalize(getRawStatement())
    }

    public var isReadOnly: Bool {
        return WCDBHandleStatementIsReadOnly(getRawStatement())
    }

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
            WCTAPIBridge.bindProperty(args.0.wctProperty!, ofObject: object, to: Int32(truncatingIfNeeded: args.toIndex), with: getRawStatement())
        }
    }

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
        WCTAPIBridge.bindProperties(propertyConvertibleList.asWCTBridgeProperties(), ofObject: object, with: getRawStatement())
    }

    public func bind(_ value: ColumnEncodable?, toIndex index: Int) {
        guard let bindingValue = value?.archivedValue() else {
            WCDBHandleStatementBindNull(getRawStatement(), Int32(index))
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
            WCDBHandleStatementBindNull(getRawStatement(), Int32(index))
        }
    }

    public func bind(_ value: Int32, toIndex index: Int) {
        WCDBHandleStatementBindInteger(getRawStatement(), Int32(index), Int64(value))
    }

    public func bind(_ value: Int64, toIndex index: Int) {
        WCDBHandleStatementBindInteger(getRawStatement(), Int32(index), value)
    }

    public func bind(_ value: Double, toIndex index: Int) {
        WCDBHandleStatementBindDouble(getRawStatement(), Int32(index), value)
    }

    public func bind(_ value: String, toIndex index: Int) {
        WCDBHandleStatementBindText(getRawStatement(), Int32(index), value)
    }

    public func bind(_ value: Data, toIndex index: Int) {
        value.withUnsafeBytes {buffer in
            WCDBHandleStatementBindBlob(getRawStatement(), Int32(index), buffer.bindMemory(to: UInt8.self).baseAddress!, UInt64(value.count))
        }
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

    public func value(atIndex index: Int, of type: ColumnDecodable.Type) -> ColumnDecodable? {
        guard columnType(atIndex: index) != .null else {
            return nil
        }
        return type.init(with: columnValue(atIndex: index))
    }

    public func value<ColumnDecodableType: ColumnDecodable>(
        atIndex index: Int,
        of type: ColumnDecodableType.Type = ColumnDecodableType.self) -> ColumnDecodableType? {
        guard columnType(atIndex: index) != .null else {
            return nil
        }
        return type.init(with: columnValue(atIndex: index))
    }

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

    public func value<ColumnDecodableType: ColumnDecodable>(
        byName name: String,
        of type: ColumnDecodableType.Type = ColumnDecodableType.self) -> ColumnDecodableType? {
        guard let index = index(byName: name) else {
            return nil
        }
        return value(atIndex: index)
    }

    public func index(byName name: String) -> Int? {
        for index in 0..<columnCount() {
            if columnName(atIndex: index)==name {
                return index
            }
        }
        return nil
    }

    public func columnValue(atIndex index: Int) -> Int32 {
        columnValue(atIndex: index, of: Int32.self)
    }

    public func columnValue(atIndex index: Int, of type: Int32.Type = Int32.self) -> Int32 {
        Int32(truncatingIfNeeded: WCDBHandleStatementGetInteger(getRawStatement(), Int32(index)))
    }

    public func columnValue(atIndex index: Int) -> Int64 {
        columnValue(atIndex: index, of: Int64.self)
    }

    public func columnValue(atIndex index: Int, of type: Int64.Type = Int64.self) -> Int64 {
        WCDBHandleStatementGetInteger(getRawStatement(), Int32(index))
    }

    public func columnValue(atIndex index: Int) -> Double {
        columnValue(atIndex: index, of: Double.self)
    }

    public func columnValue(atIndex index: Int, of type: Double.Type = Double.self) -> Double {
        WCDBHandleStatementGetDouble(getRawStatement(), Int32(index))
    }

    public func columnValue(atIndex index: Int) -> String {
        columnValue(atIndex: index, of: String.self)
    }

    public func columnValue(atIndex index: Int, of type: String.Type = String.self) -> String {
        guard let cString = WCDBHandleStatementGetText(getRawStatement(), Int32(index)) else {
            return ""
        }
        return String(cString: cString)
    }

    public func columnValue(atIndex index: Int) -> Data {
        columnValue(atIndex: index, of: Data.self)
    }

    public func columnValue(atIndex index: Int, of type: Data.Type = Data.self) -> Data {
        guard let bytes = WCDBHandleStatementGetBlob(getRawStatement(), Int32(index)) else {
            return Data()
        }
        let count = Int(WCDBHandleStatementGetColumnSize(getRawStatement(), Int32(index)))
        return Data(bytes: bytes, count: count)
    }

    public func columnCount() -> Int {
        return Int(WCDBHandleStatementGetColumnCount(getRawStatement()))
    }

    public func columnName(atIndex index: Int) -> String {
        guard let cString = WCDBHandleStatementGetColumnName(getRawStatement(), Int32(index)) else {
            return ""
        }
        return String(cString: cString)
    }

    public func columnType(byName name: String) -> ColumnType {
        guard let index = index(byName: name) else {
            return .null
        }
        return columnType(atIndex: index)
    }

    public func columnType(atIndex index: Int) -> ColumnType {
        switch WCDBHandleStatementGetColumnType(getRawStatement(), Int32(index)) {
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

    public func originalColumnName(atIndex index: Int) -> String {
        guard let cString = WCDBHandleStatementGetOriginalColumnName(getRawStatement(), Int32(index)) else {
            return ""
        }
        return String(cString: cString)
    }

    public func columnTableName(atIndex index: Int) -> String {
        guard let cString = WCDBHandleStatementGetColumnTableName(getRawStatement(), Int32(index)) else {
            return ""
        }
        return String(cString: cString)
    }
}
