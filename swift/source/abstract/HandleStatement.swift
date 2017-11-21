
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
public class HandleStatement {
    private var stmt: sqlite3_stmt?
    
    private let handle: Handle
    
    init(with stmt: sqlite3_stmt, and handle: Handle) {
        self.stmt = stmt
        self.handle = handle
    }
    
    deinit {
        try! finalize()
    }
    
    @discardableResult
    public func step() throws -> Bool {
        let rc = sqlite3_step(stmt)
        guard rc==SQLITE_OK || rc==SQLITE_DONE || rc==SQLITE_ROW else {
            let dbHandle = sqlite3_db_handle(stmt)
            throw Error.reportSQLite(tag: handle.tag, path: handle.path, operation: .Step, extendedError: sqlite3_extended_errcode(dbHandle), sql: String(cString: sqlite3_sql(stmt)), rc: rc, message: String(cString: sqlite3_errmsg(dbHandle)))
        }
        return rc==SQLITE_ROW
    }
    
    public func reset() throws {
        let rc = sqlite3_reset(stmt)
        guard rc==SQLITE_OK else {
            let dbHandle = sqlite3_db_handle(stmt)
            throw Error.reportSQLite(tag: handle.tag, path: handle.path, operation: .Finalize, extendedError: sqlite3_extended_errcode(dbHandle), rc: rc, message: String(cString: sqlite3_errmsg(dbHandle)))
        }
    }
    
    public var changes: Int {
        return Int(sqlite3_changes(sqlite3_db_handle(stmt)))
    }
    
    public func bind(_ value: Int32, toIndex index: Int) {
        sqlite3_bind_int(stmt, Int32(index), value)
    }
    
    public func bind(_ value: Int64, toIndex index: Int) {
        sqlite3_bind_int64(stmt, Int32(index), value)
    }
    
    public func bind(_ value: Double, toIndex index: Int) {
        sqlite3_bind_double(stmt, Int32(index), value)
    }
    
    public func bind(_ value: String, toIndex index: Int) {
        sqlite3_bind_text_transient(stmt, Int32(index), value, -1)
    }
    
    public func bind(_ value: Data, toIndex index: Int) {
        value.withUnsafeBytes ({ (bytes: UnsafePointer<Int8>) -> Void in
            sqlite3_bind_blob_transient(stmt, Int32(index), bytes, Int32(value.count))
        })
    }
    
    public func bind(_ value: Void?, toIndex index: Int) {
        sqlite3_bind_null(stmt, Int32(index))
    }
    
    public func columnValue(atIndex index: Int) -> Int32 {
        return sqlite3_column_int(stmt, Int32(index))
    }
    
    public func columnValue(atIndex index: Int) -> Int64 {
        return sqlite3_column_int64(stmt, Int32(index))
    }
    
    public func columnValue(atIndex index: Int) -> Double {
        return sqlite3_column_double(stmt, Int32(index))
    }
    
    public func columnValue(atIndex index: Int) -> String {
        guard let cString = sqlite3_column_text(stmt, Int32(index)) else {
            return ""
        }
        return String(cString: cString)
    }
    
    public func columnValue(atIndex index: Int) -> Data {
        return Data(bytes: sqlite3_column_blob(stmt, Int32(index)), count: Int(sqlite3_column_bytes(stmt, Int32(index))))
    }
    
    public func columnCount() -> Int {
        return Int(sqlite3_column_count(stmt))
    }
    
    public func columnName(atIndex index: Int) -> String {
        return String(cString: sqlite3_column_name(stmt, Int32(index)))
    }
    
    public func columnTableName(atIndex index: Int) -> String {
        return String(cString: sqlite3_column_table_name(stmt, Int32(index)))
    }
    
    public func columnType(atIndex index: Int) -> ColumnType {
        switch sqlite3_column_type(stmt, Int32(index)) {
        case SQLITE_INTEGER:
            return ColumnType.Integer64
        case SQLITE_FLOAT:
            return ColumnType.Float
        case SQLITE_BLOB:
            return ColumnType.BLOB
        case SQLITE_TEXT:
            return ColumnType.Text
        default:
            return ColumnType.Null
        }
    }
    
    public var lastInsertedRowID: Int64 {
        return sqlite3_last_insert_rowid(stmt)
    }
    
    public func finalize() throws {
        if stmt != nil {
            let dbHandle = sqlite3_db_handle(stmt)
            let rc = sqlite3_finalize(stmt)
            stmt = nil
            guard rc==SQLITE_OK else {
                throw Error.reportSQLite(tag: handle.tag, path: handle.path, operation: .Finalize, extendedError: sqlite3_extended_errcode(dbHandle), rc: rc, message: String(cString: sqlite3_errmsg(dbHandle)))
            }
        }
    }
}
