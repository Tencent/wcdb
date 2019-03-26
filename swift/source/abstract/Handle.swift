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

public final class Handle {
    private var handle: SQLite3?

    public let path: String
    public internal(set) var tag: Tag? = nil {
        didSet {
            tracer?.userInfo = tag
        }
    }

    typealias CommittedHook = (Handle, Int, Void?) -> Void
    private struct CommittedHookInfo {
        var onCommitted: CommittedHook
        weak var handle: Handle?
    }
    private var committedHookInfo: CommittedHookInfo?

    private var tracer: Tracer?

    init(withPath path: String) {
        DispatchQueue.once(name: "com.tencent.wcdb.handle") {
            sqlite3_config_multithread()
            sqlite3_config_memstatus(Int32(truncating: false))
            sqlite3_config_log({ (_, code, message) in
                let msg = (message != nil) ? String(cString: message!) : ""
                Error.reportSQLiteGlobal(code: Int(code), message: msg)
            }, nil)
        }
        self.path = path
    }

    deinit {
        try? close()
    }

    func open() throws {
        let directory = URL(fileURLWithPath: path).deletingLastPathComponent().path
        try File.createDirectoryWithIntermediateDirectories(atPath: directory)
        let rc = sqlite3_open(path, &handle)
        guard rc == SQLITE_OK else {
            throw Error.reportSQLite(tag: tag,
                                     path: path,
                                     operation: .open,
                                     code: rc,
                                     message: String(cString: sqlite3_errmsg(handle))
            )
        }
    }

    func close() throws {
        let rc = sqlite3_close(handle)
        guard rc == SQLITE_OK else {
            throw Error.reportSQLite(tag: tag,
                                     path: path,
                                     operation: .close,
                                     code: rc,
                                     message: String(cString: sqlite3_errmsg(handle))
            )
        }
        handle = nil
    }

    public func prepare(_ statement: Statement) throws -> HandleStatement {
        assert(statement.statementType != .transaction, "[prepare] a transaction is not allowed, use [exec] instead")
        var stmt: OpaquePointer? = nil
        let rc = sqlite3_prepare_v2(handle, statement.description, -1, &stmt, nil)
        guard rc==SQLITE_OK else {
            throw Error.reportSQLite(tag: tag,
                                     path: path,
                                     operation: .prepare,
                                     extendedError: sqlite3_extended_errcode(handle),
                                     sql: statement.description,
                                     code: rc,
                                     message: String(cString: sqlite3_errmsg(handle))
            )
        }
        return HandleStatement(with: stmt!, and: self)
    }

    public func exec(_ statement: Statement) throws {
        let rc = sqlite3_exec(handle, statement.description, nil, nil, nil)
        let result = rc == SQLITE_OK
        if let tracer = self.tracer {
            if  statement.statementType == .transaction,
                let statementTransaction = statement as? StatementTransaction,
                let transactionType = statementTransaction.transactionType {
                switch transactionType {
                case .begin:
                    if result {
                        tracer.shouldAggregation = true
                    }
                case .commit:
                    if result {
                        tracer.shouldAggregation = false
                    }
                case .rollback:
                    tracer.shouldAggregation = false
                }
            }
        }
        guard result else {
            throw Error.reportSQLite(tag: tag,
                                     path: path,
                                     operation: .exec,
                                     extendedError: sqlite3_extended_errcode(handle),
                                     sql: statement.description,
                                     code: rc,
                                     message: String(cString: sqlite3_errmsg(handle))
            )
        }
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
        let rc = key.withUnsafeBytes ({ (bytes: UnsafePointer<Int8>) -> Int32 in
            return sqlite3_key(handle, bytes, Int32(key.count))
        })
        guard rc == SQLITE_OK else {
            throw Error.reportSQLite(tag: tag,
                                     path: path,
                                     operation: .setCipherKey,
                                     extendedError: sqlite3_extended_errcode(handle),
                                     code: rc,
                                     message: String(cString: sqlite3_errmsg(handle))
            )
        }
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
                rc = sqliterk_save_master(handle, backupPath, bytes, Int32(key.count))
            }
        } else {
            rc = sqliterk_save_master(handle, backupPath, nil, 0)
        }
        guard rc == SQLITERK_OK else {
            throw Error.reportRepair(path: path,
                                     operation: .saveMaster,
                                     code: Int(rc))
        }
    }

    public func recover(fromPath source: String,
                        withPageSize pageSize: Int32,
                        databaseKey optionalDatabaseKey: Data? = nil,
                        backupKey optionalBackupKey: Data? = nil) throws {
        var rc = SQLITERK_OK

        let backupPath = source+Handle.backupSubfix

        let kdfSalt = UnsafeMutablePointer<UInt8>.allocate(capacity: 16)
        memset(kdfSalt, 0, 16)

        var info: OpaquePointer? = nil
        let backupSize: Int32 = Int32(optionalBackupKey?.count ?? 0)
        if let backupKey = optionalBackupKey {
            backupKey.withUnsafeBytes({ (bytes: UnsafePointer<Int8>) -> Void in
                rc = sqliterk_load_master(backupPath, bytes, backupSize, nil, 0, &info, kdfSalt)
            })
        }else {
            rc = sqliterk_load_master(backupPath, nil, backupSize, nil, 0, &info, kdfSalt)
        }

        guard rc == SQLITERK_OK else {
            throw Error.reportRepair(path: backupPath,
                                     operation: .repair,
                                     code: Int(rc))
        }

        var conf = sqliterk_cipher_conf()
        conf.page_size = pageSize
        conf.kdf_salt = UnsafePointer(kdfSalt)
        conf.use_hmac = 1

        typealias RepairKit = OpaquePointer
        var rk: RepairKit? = nil
        
        let databaseSize: Int32 = Int32(optionalDatabaseKey?.count ?? 0)
        if let databaseKey = optionalDatabaseKey {
            databaseKey.withUnsafeBytes({ (bytes: UnsafePointer<Int8>) -> Void in
                sqliterk_cipher_conf_set_key(&conf, bytes, databaseSize)
                rc = sqliterk_open(source, &conf, &rk)
            })
        }else {
            sqliterk_cipher_conf_set_key(&conf, nil, 0)
            rc = sqliterk_open(source, &conf, &rk)
        }
        guard rc == SQLITERK_OK else {
            throw Error.reportRepair(path: source,
                                     operation: .repair,
                                     code: Int(rc))
        }

        rc = sqliterk_output(rk, handle, info, UInt32(SQLITERK_OUTPUT_ALL_TABLES))
        guard rc == SQLITERK_OK else {
            throw Error.reportRepair(path: source,
                                     operation: .repair,
                                     code: Int(rc))
        }
    }
}

extension Handle {
    public static let subfixs: [String] = ["", "-wal", "-journal", "-shm", Handle.backupSubfix]
}

extension Handle {
    public typealias SQLTracer = (String) -> Void

    func lazyTracer() -> Tracer? {
        if tracer == nil && handle != nil {
            tracer = Tracer(with: handle!)
        }
        return tracer
    }

    func trace(sql sqlTracer: @escaping SQLTracer) {
        lazyTracer()?.trace(sql: sqlTracer)
    }

    public typealias PerformanceTracer = (Tag?, [String: Int], Int64) -> Void // Tag?, (SQL, count), cost

    func trace(performance performanceTracer: @escaping PerformanceTracer) {
        lazyTracer()?.track(performance: { (sqls, cost, userInfo) in
            performanceTracer(userInfo as? Tag, sqls, cost)
        })
    }
}

//Commit hook
extension Handle {
    func register(onCommitted: @escaping CommittedHook) {
        committedHookInfo = CommittedHookInfo(onCommitted: onCommitted, handle: self)
        sqlite3_wal_hook(handle, { (pointer, _, _, pages) -> Int32 in
            let committedHookInfo = pointer!.assumingMemoryBound(to: CommittedHookInfo.self).pointee
            committedHookInfo.onCommitted(committedHookInfo.handle!, Int(pages), nil)
            return SQLITE_OK
        }, &committedHookInfo)
    }
}
