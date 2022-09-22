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

    @discardableResult
    public func step() throws -> Bool {
        if !WCDBHandleStatementStep(stmt) {
            throw handle.getError()
        }
        return !WCDBHandleStatementIsDone(stmt)
    }

    public func reset() {
        WCDBHandleStatementReset(stmt)
    }

    public func finalize() {
        WCDBHandleStatementFinalize(stmt)
    }

    public var changes: Int {
        return Int(handle.changes)
    }

    public var isReadOnly: Bool {
        return WCDBHandleStatementIsReadOnly(stmt)
    }

    public func bind(_ value: FundamentalValue, toIndex index: Int) {
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
            bind(nil, toIndex: index)
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

    public func bind(_ _: Void?, toIndex index: Int) {
        WCDBHandleStatementBindNull(stmt, Int32(index))
    }

    public func columnValue(atIndex index: Int) -> FundamentalValue {
        switch columnType(atIndex: index) {
        case .integer32:
            return FundamentalValue(columnValue(atIndex: index, of: Int32.self))
        case .integer64:
            return FundamentalValue(columnValue(atIndex: index, of: Int64.self))
        case .float:
            return FundamentalValue(columnValue(atIndex: index, of: Double.self))
        case .text:
            return FundamentalValue(columnValue(atIndex: index, of: String.self))
        case .BLOB:
            return FundamentalValue(columnValue(atIndex: index, of: Data.self))
        case .null:
            return FundamentalValue(nil)
        }
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

    public func columnCount() -> Int {
        return Int(WCDBHandleStatementGetColumnCount(stmt))
    }

    public func columnName(atIndex index: Int) -> String {
        guard let cString = WCDBHandleStatementGetColumnName(stmt, Int32(index)) else {
            return ""
        }
        return String(cString: cString)
    }

    public func originalColumnName(atIndex index: Int) -> String {
        guard let cString = WCDBHandleStatementGetOriginalColumnName(stmt, Int32(index)) else {
            return ""
        }
        return String(cString: cString)
    }

    public func columnTableName(atIndex index: Int) -> String {
        guard let cString = WCDBHandleStatementGetColumnTableName(stmt, Int32(index)) else {
            return ""
        }
        return String(cString: cString)
    }

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

    public var lastInsertedRowID: Int64 {
        return handle.lasInsertedRowID
    }
}
