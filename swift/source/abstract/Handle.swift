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

public typealias Tag = Int

public class Handle {
    private var handle: sqlite3? = nil

    public let path: String
    public internal(set) var tag: Tag? = nil
    
    typealias CommittedHook = (Handle, Int, Void?) -> Void
    private struct CommittedHookInfo {
        var onCommitted: CommittedHook
        weak var handle: Handle?
    }
    private var committedHookInfo: CommittedHookInfo? = nil 
    
    private lazy var tracer = Tracer(with: handle!)
    
    init(withPath path: String) {
        _ = Handle.once
        self.path = path
    }
    
    deinit {
        try? close()
    }
    
    private static let once: Void = {
        sqlite3_config_multithread()
        sqlite3_config_memstatus(Int32(truncating: false))
        sqlite3_config_log({ (_, code, message) in
            let msg = (message != nil) ? String(cString: message!) : ""
            Error.reportSQLiteGlobal(rc: Int(code), message: msg)
        }, nil)
    }()
    
    func open() throws {
        try File.createDirectoryWithIntermediateDirectories(atPath: URL(fileURLWithPath: path).deletingLastPathComponent().path)
        let rc = sqlite3_open(path, &handle)
        guard rc == SQLITE_OK else{
            throw Error.reportSQLite(tag: tag, path: path, operation: .Open, rc: rc, message: String(cString: sqlite3_errmsg(handle)))
        }
    }
    
    func close() throws {
        let rc = sqlite3_close(handle)
        guard rc == SQLITE_OK else {
            throw Error.reportSQLite(tag: tag, path: path, operation: .Close, rc: rc, message: String(cString: sqlite3_errmsg(handle)))
        }
        handle = nil
    }
    
    public func prepare(_ statement: Statement) throws -> HandleStatement {
        guard statement.statementType != .Transaction else{
            Error.abort("[prepare] a transaction is not allowed, use [exec] instead")
        }
        var stmt: OpaquePointer? = nil
        let rc = sqlite3_prepare(handle, statement.description, -1, &stmt, nil)
        guard rc==SQLITE_OK else {
            throw Error.reportSQLite(tag: tag, path: path, operation: .Prepare, extendedError: sqlite3_extended_errcode(handle), sql: statement.description, rc: rc, message: String(cString: sqlite3_errmsg(handle))) 
        }
        return HandleStatement(with: stmt!, and: self)
    }
    
    public func exec(_ statement: Statement) throws {
        let rc = sqlite3_exec(handle, statement.description, nil, nil, nil)
        guard rc == SQLITE_OK else {
            throw Error.reportSQLite(tag: tag, path: path, operation: .Exec, extendedError: sqlite3_extended_errcode(handle), sql: statement.description, rc: rc, message: String(cString: sqlite3_errmsg(handle)))
        }
    }
    
    public var lastInsertedRowID: Int64 {
        return sqlite3_last_insert_rowid(handle)
    }
    
    public var changes: Int {
        return Int(sqlite3_changes(handle))
    }
    
    public var isReadonly: Bool {
        return sqlite3_db_readonly(handle, nil)==1
    }
}

//Cipher
extension Handle {
    public func setCipher(key: Data) throws {
        #if WCDB_HAS_CODEC
            let rc = key.withUnsafeBytes ({ (bytes: UnsafePointer<Int8>) -> Int32 in
                return sqlite3_key(handle, bytes, Int32(key.count))
            })
            guard rc == SQLITE_OK else {
                throw Error.reportSQLite(tag: tag, path: path, operation: .SetCipherKey, extendedError: sqlite3_extended_errcode(handle), rc: rc, message: String(cString: sqlite3_errmsg(handle)))
            }
        #else
            Error.abort("[sqlite3_key] is not supported for current config")
        #endif
    }
}

//Repair
extension Handle {
    public static let backupSubfix = "-backup"
    
    public var backupPath: String {
        return path+Handle.backupSubfix
    }
    
    public func backup(withKey optionalKey: Data? = nil) throws {
        var rc = SQLITE_OK
        if let key = optionalKey {
            key.withUnsafeBytes { (bytes: UnsafePointer<Int8>) -> Void in
                rc = sqliterk_save_master(handle, path, bytes, Int32(key.count))
            }            
        }else {
            rc = sqliterk_save_master(handle, path, nil, 0)
        }
        guard rc == SQLITERK_OK else {
            throw Error.reportRepair(path: path, operation: .SaveMaster, code: Int(rc))
        }
    }
    
    public func recover(fromPath source: String, withPageSize pageSize: Int32, databaseKey optionalDatabaseKey: Data? = nil, backupKey optionalBackupKey: Data? = nil) throws {
        var rc = SQLITERK_OK
        
        let backupPath = source+Handle.backupSubfix
        
        let kdfSalt = UnsafeMutablePointer<UInt8>.allocate(capacity: 16)
        memset(kdfSalt, 0, 16)
        
        let backupBytes: UnsafeRawPointer? = optionalBackupKey?.withUnsafeBytes({ (bytes: UnsafePointer<UInt8>) -> UnsafeRawPointer in
            return UnsafeRawPointer(bytes)
        })
        let backupSize: Int32 = Int32(optionalBackupKey?.count ?? 0)
        
        var info: OpaquePointer? = nil
        
        rc = sqliterk_load_master(backupPath, backupBytes, backupSize, nil, 0, &info, kdfSalt) 
        guard rc == SQLITERK_OK else {
            throw Error.reportRepair(path: backupPath, operation: .Repair, code: Int(rc))
        }
        
        let databaseBytes: UnsafeRawPointer? = optionalDatabaseKey?.withUnsafeBytes({ (bytes: UnsafePointer<UInt8>) -> UnsafeRawPointer in
            return UnsafeRawPointer(bytes)
        })
        let databaseSize: Int32 = Int32(optionalDatabaseKey?.count ?? 0)

        var conf = sqliterk_cipher_conf()
        conf.key = databaseBytes
        conf.key_len = databaseSize
        conf.page_size = pageSize
        conf.kdf_salt = UnsafePointer(kdfSalt) 
        conf.use_hmac = 1
        
        typealias sqliterk = OpaquePointer
        var rk: sqliterk? = nil
        rc = sqliterk_open(source, &conf, &rk)
        guard rc == SQLITERK_OK else {
            throw Error.reportRepair(path: source, operation: .Repair, code: Int(rc))
        }
        
        rc = sqliterk_output(rk, handle, info, UInt32(SQLITERK_OUTPUT_ALL_TABLES))
        guard rc == SQLITERK_OK else {
            throw Error.reportRepair(path: source, operation: .Repair, code: Int(rc))
        }
    }
}

extension Handle {
    public static let subfixs: [String] = ["", "-wal", "-journal", "-shm", Handle.backupSubfix]
    
    public var paths: [String] {
        return Handle.subfixs.map({ (subfix) -> String in
            return path+subfix
        })
    }
}

extension Handle {
    public typealias SQLTracer = (String) -> Void
    
    func trace(sql sqlTracer: @escaping SQLTracer) {
        tracer.trace(sql: sqlTracer)
    }
    
    public typealias PerformanceTracer = (Tag?, [String:Int], Int64) -> Void // Tag?, (SQL, count), cost

    func trace(performance performanceTracer: @escaping PerformanceTracer) {
        tracer.track { performanceTracer(self.tag, $0 ,$1) }
    }
}

//Commit hook
extension Handle {
    func register(onCommitted optionalOnCommitted: CommittedHook?) {
        guard let onCommitted = optionalOnCommitted else {
            committedHookInfo = nil
            sqlite3_wal_hook(handle, nil, nil)
            return 
        }
        committedHookInfo = CommittedHookInfo(onCommitted: onCommitted, handle: self)
        sqlite3_wal_hook(handle, { (p, _, _, pages) -> Int32 in
            let committedHookInfo = p!.assumingMemoryBound(to: CommittedHookInfo.self).pointee
            committedHookInfo.onCommitted(committedHookInfo.handle!, Int(pages), nil)
            return SQLITE_OK
        }, &committedHookInfo)
    }
}
