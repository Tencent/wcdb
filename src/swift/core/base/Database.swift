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

/// Thread-safe Database object
public class Database {
    internal final let recyclableDatabase: RecyclableCPPDatabase
    internal final var database: CPPDatabase {
        return recyclableDatabase.raw
    }

    /// Init a database from path.  
    /// Note that all database objects with same path share the same core.
    /// So you can create multiple database objects. WCDB will manage them automatically.  
    /// Note that WCDB will not generate a sqlite handle until the first operation, 
    /// which is also called as lazy initialization.
    ///
    /// - Parameter path: Path to your database
    public convenience init(at path: String) {
        self.init(at: URL(fileURLWithPath: path))
    }

    /// Init a database from file url.  
    /// Note that all database objects with same path share the same core. 
    /// So you can create multiple database objects. WCDB will manage them automatically.  
    /// Note that WCDB will not generate a sqlite handle until the first operation, 
    /// which is also called as lazy initialization.
    ///
    /// - Parameter url: File url to your database
    public convenience init(at url: URL) {
        #if swift(>=5)
        #else
            WCDBError.fatalError("Swift 5 is required.")
        #endif
        let database = WCDBCoreCreateDatabase(url.standardizedFileURL.path)
        self.init(with: database)
    }

    internal init(with cppDatabase: CPPDatabase) {
        DispatchQueue.once(name: "Once.ObjectBridge", {
            ObjectBridge.initializeCPPAPI()
        })
        self.recyclableDatabase = ObjectBridge.createRecyclableCPPObject(cppDatabase)
    }

    /// The tag of the database. Default to nil.
    /// Note that core objects with same path share this tag, even they are not the same object.
    ///
    ///     let database1 = Database(withPath: path)
    ///     let database2 = Database(withPath: path)
    ///     database1.tag = 1
    ///     print("Tag: \(database2.tag!)") // print 1
    ///
    public var tag: Tag? {
        get {
            let tag = WCDBDatabaseGetTag(database)
            if tag != 0 {
                return tag
            }
            return nil
        }
        set {
            if let tag = newValue {
                WCDBDatabaseSetTag(database, tag)
            } else {
                WCDBDatabaseSetTag(database, 0)
            }
        }
    }

    /// The path of the related database.
    public final var path: String {
        if let path = WCDBDatabaseGetPath(database) {
            return String(cString: path)
        }
        return ""
    }

    /// Since WCDB is using lazy initialization, 
    /// `init(withPath:)`, `init(withFileURL:)` never failed even the database can't open. 
    /// So you can call this to check whether the database can be opened.  
    /// Return false if an error occurs during sqlite handle initialization.
    public var canOpen: Bool {
        return WCDBDatabaseCanOpen(database)
    }

    /// Check database is already opened.
    public var isOpened: Bool {
        return WCDBDatabaseIsOpened(database)
    }

    public typealias OnClosed = () throws -> Void

    /// Close the database.  
    ///     Since Multi-threaded operation is supported in WCDB, 
    ///     other operations in different thread can open the closed database. 
    ///     So this method can make sure database is closed in the `onClosed` block. 
    ///     All other operations will be blocked until this method returns.
    ///
    /// A close operation consists of 4 steps:  
    ///     1. `blockade`, which blocks all other operations.  
    ///     2. `close`, which waits until all sqlite handles return and closes them.  
    ///     3. `onClosed`, which trigger the callback.  
    ///     4. `unblokade`, which unblocks all other opreations.  
    ///
    /// You can simply call `close:` to do all steps above or call these separately.  
    /// Since this method will wait until all sqlite handles return, it may lead to deadlock in some bad practice. 
    ///     The key to avoid deadlock is to make sure all WCDB objects in current thread is dealloced. In detail:  
    ///     1. You should not keep WCDB objects, including `Insert`, `Delete`, `Update`, `Select`, `RowSelect`, 
    ///        `MultiSelect`, `Handle`, `PreparedStatement`. These objects should not be kept.
    ///        You should get them, use them, then release them right away.  
    ///     2. WCDB objects may not be out of its' scope.  
    ///     The best practice is to call `close:` in sub-thread and display a loading animation in main thread.  
    ///
    ///     //close directly
    ///     database.close(onClosed: { () throws -> Void in
    ///         //do something on this closed database
    ///     })
    ///
    ///     //close separately
    ///     database.blockade()
    ///     database.close()
    ///     //do something on this closed database
    ///     database.unblockade()
    ///
    /// - Parameter onClosed: Trigger on database closed.
    /// - Throws: Rethrows your error.
    public func close(onClosed: @escaping OnClosed) throws {
        var callbackError: Swift.Error?
        let closeCallback: () -> Void = {
            do {
                try onClosed()
            } catch {
                callbackError = error
            }
        }

        let cppCallback: @convention(c) (UnsafeMutableRawPointer) -> Void = {
            cppContext in
            let callbackWrap: ValueWrap<() -> Void>? = ObjectBridge.extractTypedSwiftObject(cppContext)
            guard let callbackWrap = callbackWrap else {
                return
            }
            callbackWrap.value()
        }
        let callbackWrap = ValueWrap(closeCallback)
        let callbackWrapPointer = ObjectBridge.getUntypeSwiftObject(callbackWrap)
        WCDBDatabaseClose(database, callbackWrapPointer, cppCallback)
        ObjectBridge.objectDestructor(callbackWrapPointer)
        if callbackError != nil {
            throw callbackError!
        }
    }

    /// Close the database.
    public func close() {
        WCDBDatabaseClose(database, nil, nil)
    }

    /// Blockade the database.
    public func blockade() {
        WCDBDatabaseBlockade(database)
    }

    /// Unblockade the database.
    public func unblockade() {
        WCDBDatabaseUnblockade(database)
    }

    /// Check whether database is blockaded.
    public var isBlockaded: Bool {
        return WCDBDatabaseIsBlockaded(database)
    }

    /// Purge all unused memory of this database.  
    /// WCDB will cache and reuse some sqlite handles to improve performance.   
    /// The max count of free sqlite handles is same
    /// as the number of concurrent threads supported by the hardware implementation.  
    /// You can call it to save some memory.
    public func purge() {
        WCDBDatabasePurge(database)
    }

    /// Purge all unused memory of all databases.  
    /// Note that WCDB will call this interface automatically while it receives memory warning on iOS.
    public static func purge() {
        WCDBCorePurgeAllDatabase()
    }

    internal func getError() -> WCDBError {
        let cppError = WCDBDatabaseGetError(database)
        return ErrorBridge.getErrorFrom(cppError: cppError)
    }

    /// Create a `Handle` for current database.
    /// `Handle` is a wrapper for sqlite db handle of type `sqlite3*`,
    /// and the sqlite db handle is lazy initialized and will not be actually generated until the first operation on current handle takes place.
    /// Note that all `Handle` created by the current database in the current thread will share the same sqlite db handle internally,
    /// so it can avoid the deadlock between different sqlite db handles in some extreme cases.
    /// - Parameter writeHint: A hint as to whether the handle will be used to update content in the database. It doesn't need to be precise.
    /// - Returns: A new `Handle`.
    /// - Throws: `Error`
    public func getHandle(writeHint: Bool = false) throws -> Handle {
        let cppHandle = WCDBDatabaseGetHandle(database, writeHint)
        let handle = Handle(withCPPHandle: cppHandle, database: self)
        if !WCDBHandleCheckValid(cppHandle) {
            throw getError()
        }
        return handle
    }

    public func getDatabase() -> Database {
        return self
    }

    /// Exec a specific sql.  
    /// Note that you can use this interface to execute a SQL that is not contained in the WCDB interface layer. 
    ///
    /// - Parameter statement: WINQ statement
    /// - Throws: `Error`
    public func exec(_ statement: Statement) throws {
        let hint = withExtendedLifetime(statement) {
            WCDBStatementNeedToWrite($0.rawCPPObj)
        }
        return try getHandle(writeHint: hint).exec(statement)
    }
}

// Config
public extension Database {

    enum CipherVersion: Int32 {
        case defaultVerion = 0
        case version1
        case version2
        case version3
        case version4
    }

    /// Set cipher key for a database.   
    /// For an encrypted database, you must call it before all other operation.  
    /// The cipher page size defaults to 4096 in WCDB, but it defaults to 1024 in other databases. 
    /// So for an existing database created by other database framework, you should set it to 1024. 
    /// Otherwise, you'd better to use cipher page size with 4096 
    /// or simply call setCipherKey: interface to get better performance.
    /// If your database is created with the default configuration of WCDB 1.0.x, please set cipherVersion to `CipherVersion.version3`.
    ///
    /// - Parameters:
    ///   - optionalKey: Cipher key. Nil for no cipher.
    ///   - pageSize: Cipher page size.
    ///   - cipherVersion: Use the default configuration of a specific version of sqlcipher
    func setCipher(key optionalKey: Data?, pageSize: Int = 4096, cipherVersion: CipherVersion = .defaultVerion) {
        if let key = optionalKey {
            key.withUnsafeBytes { (buffer: UnsafeRawBufferPointer) in
                WCDBDatabaseConfigCipher(database, buffer.bindMemory(to: UInt8.self).baseAddress, Int32(buffer.count), Int32(pageSize), cipherVersion.rawValue)
            }
        } else {
            WCDBDatabaseConfigCipher(database, nil, 0, 0, 0)
        }
    }

    /// Force SQLCipher to operate with the default settings consistent with that major version number as the default.
    /// It works the same as `PRAGMA cipher_default_compatibility`.
    ///
    /// - Parameter version: The specified sqlcipher major version.
    static func setDefaultCipherConfiguration(_ version: CipherVersion) {
        WCDBCoreSetDefaultCipherConfig(version.rawValue)
    }

    enum ConfigPriority: Int32 {
        case highest = -2147483648 // Only For cipher config
        case high = -100
        case `default` = 0
        case low = 100
    }

    typealias Config = (Handle) throws -> Void
    /// Set config for this database.
    ///
    /// Since WCDB is a multi-handle database, an executing handle will not apply this config immediately.  
    /// Instead, all handles will run this config before its next operation.
    ///
    /// If you want to add cipher config, please use `ConfigPriority.highest`.
    ///
    ///     database.setConfig(named: "demo", withInvocation: { (handle: Handle) throws in
    ///         try handle.exec(StatementPragma().pragma(.secureDelete).to(true))
    ///     }, withUninvocation: { (handle: Handle) throws in
    ///         try handle.exec(StatementPragma().pragma(.secureDelete).to(false))
    ///     }, withPriority: .high)
    ///
    /// - Parameters:
    ///   - name: The Identifier for this config
    ///   - callback: config
    ///   - order: The smaller number is called first
    func setConfig(named name: String,
                   withInvocation invocation: @escaping Config,
                   withUninvocation uninvocation: Config? = nil,
                   withPriority priority: ConfigPriority = ConfigPriority.default) {
        let cppInvocation: @convention(c) (UnsafeMutableRawPointer, CPPHandle) -> Bool = {
            cppContext, cppHandle in
            let invocationWrap: ValueWrap<(CPPHandle) -> Bool>? = ObjectBridge.extractTypedSwiftObject(cppContext)
            guard let invocationWrap = invocationWrap else {
                return false
            }
            return invocationWrap.value(cppHandle)
        }
        let invocationBlock: (CPPHandle) -> Bool = {
            cppHandle in
            let handle = Handle(withCPPHandle: cppHandle, database: self)
            var ret = true
            do {
                try invocation(handle)
            } catch {
                ret = false
            }
            return ret
        }
        let invocationWrap = ValueWrap(invocationBlock)
        let invocationWrapPointer = ObjectBridge.getUntypeSwiftObject(invocationWrap)

        var uninvocationWrapPointer: UnsafeMutableRawPointer?
        if let uninvocation = uninvocation {
            let uninvocationBlock: (CPPHandle) -> Bool = {
                cppHandle in
                let handle = Handle(withCPPHandle: cppHandle, database: self)
                var ret = true
                do {
                    try uninvocation(handle)
                } catch {
                    ret = false
                }
                return ret
            }
            let uninvocationWrap = ValueWrap(uninvocationBlock)
            uninvocationWrapPointer = ObjectBridge.getUntypeSwiftObject(uninvocationWrap)
        }
        WCDBDatabaseConfig(database,
                           name.cString,
                           cppInvocation,
                           invocationWrapPointer,
                           uninvocationWrapPointer != nil ? cppInvocation : nil,
                           uninvocationWrapPointer,
                           priority.rawValue,
                           ObjectBridge.objectDestructor)
    }

    /// Set the default directory for temporary database files.
    /// If not set, an existing directory will be selected as the temporary database files directory in the following order:
    ///     1. TMPDIR environment value;
    ///     2. /Var/tmp;
    ///     3. /usr/tmp;
    ///     4. /tmp;
    ///     5. The current working directory (".")
    /// Please see: https://www.sqlite.org/tempfiles.html
    /// - Parameter directory a global temporary directory.
    /// - Returns: true if directory exists or create directory success.
    static func setDefaultTemporaryDirectory(_ directory: String) -> Bool {
        return WCDBCoreSetDefaultTemporaryDirectory(directory.cString)
    }

    struct PerformanceInfo {
        public let tablePageReadCount: Int
        public let tablePageWriteCount: Int
        public let indexPageReadCount: Int
        public let indexPageWriteCount: Int
        public let overflowPageReadCount: Int
        public let overflowPageWriteCount: Int
        public let costInNanoseconds: Int64
        init(_ cppInfo: UnsafePointer<CPPPerformanceInfo>) {
            tablePageReadCount = Int(cppInfo.pointee.tablePageReadCount)
            tablePageWriteCount = Int(cppInfo.pointee.tablePageWriteCount)
            indexPageReadCount = Int(cppInfo.pointee.indexPageReadCount)
            indexPageWriteCount = Int(cppInfo.pointee.indexPageWriteCount)
            overflowPageReadCount = Int(cppInfo.pointee.overflowPageReadCount)
            overflowPageWriteCount = Int(cppInfo.pointee.overflowPageWriteCount)
            costInNanoseconds = cppInfo.pointee.costInNanoseconds
        }
    }

    typealias PerformanceTracer = (Tag, String, UInt64, String, PerformanceInfo) -> Void // Tag, Path, handleIdentifier, SQL, Info

    /// You can register a tracer to monitor the performance of all SQLs.
    /// It returns
    /// 1. Every SQL executed by the database.
    /// 2. Time consuming in nanoseconds.
    /// 3. Number of reads and writes on different types of db pages.
    /// 4. Tag of database.
    /// 5. Path of database.
    /// 6. The id of the handle executing this SQL.
    ///
    /// Note that:
    /// 1. You should register trace before all db operations.
    /// 2. Global tracer will be recovered by db tracer.
    ///
    ///     Database.globalTracePerformance{ (tag, path, handleId, sql, info) in
    ///         print("Tag: \(tag)")
    ///         print("Path: \(path)")
    ///         print("The handle with id \(handleId) took \(info.costInNanoseconds) nanoseconds to execute \(sql)")
    ///     }
    ///
    /// Tracer may cause WCDB performance degradation, according to your needs to choose whether to open.
    ///
    /// - Parameter trace: trace. Nil to disable global preformance trace.
    ///
    static func globalTracePerformance(_ trace: @escaping PerformanceTracer) {
        let traceWrap = ValueWrap(trace)
        let tracePointer = ObjectBridge.getUntypeSwiftObject(traceWrap)
        let cppTrace: @convention(c) (UnsafeMutableRawPointer?, Int, UnsafePointer<CChar>, UInt64, UnsafePointer<CChar>, UnsafePointer<CPPPerformanceInfo>) -> Void = {
            cppTrace, tag, path, handleId, sql, cppInfo in
            let traceWrap: ValueWrap<PerformanceTracer>? = ObjectBridge.extractTypedSwiftObject(cppTrace)
            guard let traceWrap = traceWrap else {
                return
            }
            let info = PerformanceInfo(cppInfo)
            traceWrap.value(tag, String(cString: path), handleId, String(cString: sql), info)
        }
        WCDBDatabaseGlobalTracePerformance(cppTrace, tracePointer, ObjectBridge.objectDestructor)
    }
    static func globalTracePerformance(_ trace: Void?) {
        WCDBDatabaseGlobalTracePerformance(nil, nil, nil)
    }

    /// You can register a reporter to monitor all errors of current database.
    /// Tracer may cause WCDB performance degradation, according to your needs to choose whether to open.
    ///
    /// - Parameter trace: trace. Nil to disable preformance trace.
    func trace(ofPerformance trace: @escaping PerformanceTracer) {
        let traceWrap = ValueWrap(trace)
        let traceWrapPointer = ObjectBridge.getUntypeSwiftObject(traceWrap)
        let cppTrace: @convention(c) (UnsafeMutableRawPointer?, Int, UnsafePointer<CChar>, UInt64, UnsafePointer<CChar>, UnsafePointer<CPPPerformanceInfo>) -> Void = {
            cppTrace, tag, path, handleId, sql, cppInfo in
            let traceWrap: ValueWrap<PerformanceTracer>? = ObjectBridge.extractTypedSwiftObject(cppTrace)
            guard let traceWrap = traceWrap else {
                return
            }
            let info = PerformanceInfo(cppInfo)
            traceWrap.value(tag, String(cString: path), handleId, String(cString: sql), info)
        }
        WCDBDatabaseTracePerformance(database, cppTrace, traceWrapPointer, ObjectBridge.objectDestructor)
    }
    func trace(ofPerformance: Void?) {
        WCDBDatabaseTracePerformance(database, nil, nil, nil)
    }

    typealias SQLTracer = (Tag, String, UInt64, String, String) -> Void // Tag, Path, handleIdentifier, SQL, info

    /// You can register a tracer to monitor the execution of all SQLs.
    /// It returns
    /// 1. Every SQL executed by the database.
    /// 2. Tag of database.
    /// 3. Path of database.
    /// 4. The id of the handle executing this SQL.
    /// 5. Detailed execution information of SQL. It is valid only when full sql trace is enable.
    /// Note that you should register trace before all db operations.
    ///
    ///     Database.globalTraceSQL{ (tag, path, handleId, sql, info) in
    ///         print("Tag: \(tag)")
    ///         print("Path: \(path)")
    ///         print("The handle with id \(handleId) executed \(sql)")
    ///         print("Execution info \(info)")
    ///     }
    ///
    /// Tracer may cause WCDB performance degradation, according to your needs to choose whether to open.
    ///
    /// - Parameter trace: trace. Nil to disable global sql trace.
    static func globalTraceSQL(_ trace: @escaping SQLTracer) {
        let cppCallback: @convention(c) (UnsafeMutableRawPointer?, Int, UnsafePointer<CChar>, UInt64, UnsafePointer<CChar>, UnsafePointer<CChar>) -> Void = {
            (cppContext, tag, path, handleId, sql, info) in
            let traceWrap: ValueWrap<SQLTracer>? = ObjectBridge.extractTypedSwiftObject(cppContext)
            guard let traceWrap = traceWrap else {
                return
            }
            traceWrap.value(tag, String(cString: path), handleId, String(cString: sql), String(cString: info))
        }
        let traceWrap = ValueWrap(trace)
        let traceWrapPointer = ObjectBridge.getUntypeSwiftObject(traceWrap)
        WCDBDatabaseGlobalTraceSQL(cppCallback, traceWrapPointer, ObjectBridge.objectDestructor)
    }
    static func globalTraceSQL(_ trace: Void?) {
        WCDBDatabaseGlobalTraceSQL(nil, nil, nil)
    }

    /// You can register a tracer to monitor the execution of all SQLs executed in the current database.
    /// Tracer may cause WCDB performance degradation, according to your needs to choose whether to open.
    ///
    /// - Parameter trace: trace. Nil to disable sql trace.
    func traceSQL(_ trace: @escaping SQLTracer) {
        let cppCallback: @convention(c) (UnsafeMutableRawPointer?, Int, UnsafePointer<CChar>, UInt64, UnsafePointer<CChar>, UnsafePointer<CChar>) -> Void = {
            (cppContext, tag, path, handleId, sql, info) in
            let traceWrap: ValueWrap<SQLTracer>? = ObjectBridge.extractTypedSwiftObject(cppContext)
            guard let traceWrap = traceWrap else {
                return
            }
            traceWrap.value(tag, String(cString: path), handleId, String(cString: sql), String(cString: info))
        }
        let traceWrap = ValueWrap(trace)
        let traceWrapPointer = ObjectBridge.getUntypeSwiftObject(traceWrap)
        WCDBDatabaseTraceSQL(database, cppCallback, traceWrapPointer, ObjectBridge.objectDestructor)
    }
    func traceSQL(_ trace: Void?) {
        WCDBDatabaseTraceSQL(database, nil, nil, nil)
    }

    /// Enable to collect more SQL execution information in SQL tracer.
    ///
    /// The detailed execution information of sql will include all bind parameters,
    /// step counts of `SELECT` statement, last inserted rowid of `INSERT` statement,
    /// changes of `UPDATE` and `DELETE` statements.
    /// These infomation will be returned in the last parameter of `SQLTracer`.
    ///
    /// Note That collecting these infomation will significantly reduce the performance of WCDB,
    /// please enable it only when necessary, and disable it when unnecessary.
    ///
    /// - Parameter enable: enable or not.
    func setFullSQLTrace(enable: Bool) {
        WCDBDatabaseSetFullSQLTraceEnable(database, enable)
    }

    /// You can register a reporter to monitor all errors.
    ///
    ///     Database.globalTraceError{ (error) in
    ///         print("[WCDB] \(error.description)")
    ///     }
    ///
    /// - Parameter errorReporter: report. Nil to disable error trace.
    static func globalTraceError(_ errorReporter: @escaping (WCDBError) -> Void) {
        let cppCallback: @convention(c) (UnsafeMutableRawPointer?, CPPError) -> Void = {
            cppContext, cppError in
            let errorReporterWrap: ValueWrap<(WCDBError) -> Void>? = ObjectBridge.extractTypedSwiftObject(cppContext)
            guard let errorReporterWrap = errorReporterWrap else {
                return
            }
            let error = ErrorBridge.getErrorFrom(cppError: cppError)
            errorReporterWrap.value(error)
        }
        let errorReporterWrap = ValueWrap(errorReporter)
        let errorReporterWrapPointer = ObjectBridge.getUntypeSwiftObject(errorReporterWrap)
        WCDBDatabaseGlobalTraceError(cppCallback, errorReporterWrapPointer, ObjectBridge.objectDestructor)
    }
    static func globalTraceError(_ trace: Void?) {
        WCDBDatabaseGlobalTraceError(nil, nil, nil)
    }

    /// You can register a reporter to monitor all errors of current database.
    ///
    /// - Parameter errorReporter: report. Nil to disable error trace.
    func traceError(_ errorReporter: @escaping (WCDBError) -> Void) {
        let cppCallback: @convention(c) (UnsafeMutableRawPointer?, CPPError) -> Void = {
            cppContext, cppError in
            let errorReporterWrap: ValueWrap<(WCDBError) -> Void>? = ObjectBridge.extractTypedSwiftObject(cppContext)
            guard let errorReporterWrap = errorReporterWrap else {
                return
            }
            let error = ErrorBridge.getErrorFrom(cppError: cppError)
            errorReporterWrap.value(error)
        }
        let errorReporterWrap = ValueWrap(errorReporter)
        let errorReporterWrapPointer = ObjectBridge.getUntypeSwiftObject(errorReporterWrap)
        WCDBDatabaseTraceError(database, cppCallback, errorReporterWrapPointer, ObjectBridge.objectDestructor)
    }
    func traceError(_ trace: Void?) {
        WCDBDatabaseTraceError(database, nil, nil, nil)
    }

    enum Operation: Int {
        case Create = 0
        case SetTag = 1
        case OpenHandle = 2
    }

    /// The following are the keys in the infos from the callback of database operation monitoring.
    static let OperationInfoKeyHandleCount = String(cString: WCDBDatabaseOperationTracerInfoKeyHandleCount)
    static let OperationInfoKeyHandleOpenTime = String(cString: WCDBDatabaseOperationTracerInfoKeyHandleOpenTime)
    static let OperationInfoKeyHandleOpenCPUTime = String(cString: WCDBDatabaseOperationTracerInfoKeyHandleOpenCPUTime)
    static let OperationInfoKeySchemaUsage = String(cString: WCDBDatabaseOperationTracerInfoKeySchemaUsage)
    static let OperationInfoKeyTableCount = String(cString: WCDBDatabaseOperationTracerInfoKeyTableCount)
    static let OperationInfoKeyIndexCount = String(cString: WCDBDatabaseOperationTracerInfoKeyIndexCount)
    static let OperationInfoKeyTriggerCount = String(cString: WCDBDatabaseOperationTracerInfoKeyTriggerCount)

    typealias OperationTracer = (Database, /* database */
                                 Database.Operation, /* type of operation*/
                                 [String: Value]/* infos about current operation */
    ) -> Void

    /// You can register a tracer to these database events:
    /// 1. creating a database object for the first time;
    /// 2. setting a tag on the database;
    /// 3. opening a new database handle.
    ///
    /// - Parameter trace: trace. Nil to disable error trace.
    static func globalTraceDatabaseOperation(_ trace: @escaping OperationTracer) {
        let tracerWrap = ValueWrap(trace)
        let tracerWrapPointer = ObjectBridge.getUntypeSwiftObject(tracerWrap)
        let callback: @convention(c) (UnsafeMutableRawPointer?, CPPDatabase, Int, UnsafeRawPointer) -> Void = {
            ctx, cppDatabase, type, info in
            let tracerWrap: ValueWrap<OperationTracer>? = ObjectBridge.extractTypedSwiftObject(ctx)
            guard let tracerWrap = tracerWrap else {
                return
            }
            let database = Database(with: cppDatabase)
            let operation = Operation(rawValue: type) ?? .Create
            let wrapInfo: ValueWrap<[String: Value]> = ValueWrap([:])
            let wrapInfoPointer = ObjectBridge.getUntypeSwiftObject(wrapInfo)
            let enumerator: @convention(c) (UnsafeMutableRawPointer, UnsafePointer<CChar>, CPPCommonValue) -> Void = {
                valueWrapPointer, key, value in
                let valueWrap: ValueWrap<[String: Value]>? = ObjectBridge.extractTypedSwiftObject(valueWrapPointer)
                guard let valueWrap = valueWrap else {
                    return
                }
                switch value.type {
                case WCDBBridgedType_Int:
                    valueWrap.value[String(cString: key)] = Value(value.intValue)
                case WCDBBridgedType_Double:
                    valueWrap.value[String(cString: key)] = Value(value.doubleValue)
                case WCDBBridgedType_String:
                    valueWrap.value[String(cString: key)] = Value(String(cString: unsafeBitCast(value.intValue, to: UnsafePointer<CChar>.self)))
                default:
                    return
                }
            }
            WCDBEnumerateStringViewMap(info, wrapInfoPointer, enumerator)
            tracerWrap.value(database, operation, wrapInfo.value)
            ObjectBridge.releaseSwiftObject(wrapInfoPointer)
        }
        WCDBDatabaseGlobalTraceOperation(callback, tracerWrapPointer, ObjectBridge.objectDestructor)
    }

    static func globalTraceDatabaseOperation(_ trace: Void?) {
        WCDBDatabaseGlobalTraceOperation(nil, nil, nil)
    }

    // Triggered when the database operation is blocked by other threads.
    typealias BusyTracer = (Tag /* tag */,
                            String /* path */,
                            UInt64 /* id of the thread being waited on */,
                            String /* sql executing in the thread being waited on */
    ) -> Void

    /// You can register a tracer to database busy events.
    /// It returns:
    /// 1. Tag of database being busy.
    /// 2. Path of database being busy.
    /// 3. ID of the thread being waited on.
    /// 4. SQL executing in the thread being waited on.
    ///
    /// Note that the tracer will be called back synchronously
    /// when the database operation is blocked and times out,
    /// so you can neither directly access the busy database
    /// nor perform heavy operation in the tracer.
    ///
    /// - Parameter trace: tracer. Nil to disable busy trace.
    /// - Parameter timeOut: timeout in seconds for blocking database operation.
    static func globalTraceBusy(_ trace: @escaping BusyTracer, timeOut: Double) {
        let callback: @convention(c) (UnsafeMutableRawPointer?, Int, UnsafePointer<CChar>, UInt64, UnsafePointer<CChar>) -> Void = {
            cppTrace, tag, path, tid, sql in
            let traceWrap: ValueWrap<BusyTracer>? = ObjectBridge.extractTypedSwiftObject(cppTrace)
            guard let traceWrap = traceWrap else {
                return
            }
            traceWrap.value(tag, String(cString: path), tid, String(cString: sql))
        }
        let traceWrap = ValueWrap(trace)
        let traceWrapPointer = ObjectBridge.getUntypeSwiftObject(traceWrap)
        WCDBCoreGlobalTraceBusy(callback, timeOut, traceWrapPointer, ObjectBridge.objectDestructor)
    }

    static func globalTraceBusy(_ trace: Void?, timeOut: Double) {
        WCDBCoreGlobalTraceBusy(nil, 0, nil, nil)
    }
}

// File
public extension Database {

    /// URLs to all database-related files.
    var urls: [URL] {
        return paths.map({ (path) -> URL in
            return URL(fileURLWithPath: path)
        })
    }

    /// Paths to all database-related files.
    var paths: [String] {
        var paths: [String] = []
        let enumerator: (UnsafePointer<CChar>) -> Void = {
            path in
            paths.append(String(cString: path))
        }
        let cppEnumerater: @convention(c) (UnsafeMutableRawPointer?, UnsafePointer<CChar>) -> Void = {
            cppContext, string in
            let enumeratorWrap: ValueWrap<(UnsafePointer<CChar>) -> Void>? = ObjectBridge.extractTypedSwiftObject(cppContext)
            guard let enumeratorWrap = enumeratorWrap else {
                return
            }
            enumeratorWrap.value(string)
        }
        let enumeratorWrap = ValueWrap(enumerator)
        let enumeratorWrapPointer = ObjectBridge.getUntypeSwiftObject(enumeratorWrap)
        WCDBDatabaseGetPaths(database, enumeratorWrapPointer, cppEnumerater)
        ObjectBridge.objectDestructor(enumeratorWrapPointer)
        return paths
    }

    /// Remove all database-related files.  
    /// You should call it on a closed database. Otherwise you will get a warning.
    ///
    /// - Throws: `Error`
    func removeFiles() throws {
        if !WCDBDatabaseRemoveFile(database) {
            throw getError()
        }
    }

    /// Move all database-related files and some extra files to directory safely.
    ///
    /// - Parameters:
    ///   - directory: destination
    /// - Throws: `Error`
    func moveFiles(toDirectory directory: String) throws {
        if !WCDBDatabaseMoveFile(database, directory) {
            throw getError()
        }
    }

    /// Get the space used by the database files.  
    /// You should call it on a closed database. Otherwise you will get a warning.
    ///
    /// - Returns: The sum of files size in bytes.
    /// - Throws: `Error`
    func getFilesSize() throws -> UInt64 {
        let ret = WCDBDatabaseGetFileSize(database)
        if !ret.hasValue {
            throw getError()
        }
        return ret.value
    }
}

// Repair
public extension Database {
    typealias OnCorrupted = (_ corruptedDatabase: Database) -> Void
    /// You can register a notification callback for database corruption.
    /// If the current database reports an error of `SQLITE_CORRUPT` or `SQLITE_NOTADB` during operation,
    /// WCDB will asynchronously use `PRAGMA integrity_check` to check whether this database is truly corrupted.
    /// Once confirmed, WCDB will notify you through the callback registered by this method.
    /// In the callback, you can delete the corrupted database or try to repair the database.
    ///
    /// - Parameter callback: The callback of notification.
    func setNotification(whenCorrupted callback: OnCorrupted?) {
        if let callback = callback {
            let cppCallback: @convention(c) (UnsafeMutableRawPointer?, CPPDatabase) -> Void = {
                cppContext, database in
                let callbackWrap: ValueWrap<OnCorrupted>? = ObjectBridge.extractTypedSwiftObject(cppContext)
                guard let callbackWrap = callbackWrap else {
                    return
                }
                callbackWrap.value(Database(with: database))
            }
            let callbackWrap = ValueWrap(callback)
            let callbackWrapPointer = ObjectBridge.getUntypeSwiftObject(callbackWrap)
            WCDBDatabaseSetNotificationWhenCorrupted(database, cppCallback, callbackWrapPointer, ObjectBridge.objectDestructor)
        } else {
            WCDBDatabaseSetNotificationWhenCorrupted(database, nil, nil, nil)
        }
    }

    /// Check if the database is corrupted.
    /// Since this method will check all the contents of the database until it finds something corrupted, it is time consuming and you'd better not call this method on the main thread.
    func checkIfCorrupted() -> Bool {
        return WCDBDatabaseCheckIfCorrupted(database)
    }

    /// Check if the database has been marked as corrupted
    func isAlreadyCorrupted() -> Bool {
        return WCDBDatabaseCheckIsAlreadyCorrupted(database)
    }

    /// Enable database to automatically backup itself after there are updates.
    ///
    /// The backup content mainly includes the SQL statements related to table creation,
    /// and all leaf page numbers of each table in database.
    /// The backup content will be saved in the same directory of the database.
    /// And the backup file name is the database file name plus "-first.material" and "-last.material" suffixes.
    ///
    /// Auto-backup do not degrade IO performance of the database.
    func setAutoBackup(enable flag: Bool) {
        WCDBDatabaseEnableAutoBackup(database, flag)
    }

    /// Backup the database manually.
    ///
    /// The backup content will be saved in the same directory of the database.
    /// And the backup file name is the database file name plus "-first.material" and "-last.material" suffixes.
    func backup() throws {
        if !WCDBDatabaseBackup(database) {
            throw getError()
        }
    }

    typealias BackupFiilter = (_ tableName: String) -> Bool

    /// Set a filter to tell which table should be backed up.
    ///
    /// The filter will be called for every table in database during the initialization of backup logic.
    /// If there are some tables that do not need to be backed up, return false when these table names are passed into the filter.
    ///
    /// - Parameter filter: The table filter. A table name will be pass to filter when it's called.
    func filterBackup(tableShouldBeBackedUp filter: BackupFiilter?) {
        if let filter = filter {
            let cppFilter: @convention(c) (UnsafeMutableRawPointer?, UnsafePointer<CChar>) -> Bool = {
                cppContext, cppTable in
                let filterWrap: ValueWrap<BackupFiilter>? = ObjectBridge.extractTypedSwiftObject(cppContext)
                guard let filterWrap = filterWrap else {
                    return false
                }
                return filterWrap.value(String(cString: cppTable))
            }
            let filterWrap = ValueWrap(filter)
            let filterWrapPointer = ObjectBridge.getUntypeSwiftObject(filterWrap)
            WCDBDatabaseFilterBackup(database, cppFilter, filterWrapPointer, ObjectBridge.objectDestructor)
        } else {
            WCDBDatabaseFilterBackup(database, nil, nil, nil)
        }
    }

    typealias ProgressUpdate = (_ percentage: Double, _ increment: Double) -> Bool /* Continue or not */

    /// Recover data from a corruped db.
    ///
    /// If there is a valid backup of this database, most of the uncorrupted data can be recovered,
    /// otherwise WCDB will try to read all the data that can still be accessed, starting from the first page of the database.
    /// In the extreme case where this database is not backed up and the first page is corrupted, no data can be recovered.
    ///
    /// Since repairing a database requires reading all uncorrupted data and writing them to a new database, it may take a lot of time to finish the job.
    /// During this period, you'd better display a loading view on the screen and present the processing percentage.
    ///
    /// - Parameter progress: A closure that receives the repair progress.
    /// - Returns: Percentage of repaired data. 0 or less then 0 means data recovery failed. 1 means data is fully recovered.
    func retrieve(with progress: ProgressUpdate?) throws -> Double {
        var score = 0.0
        if let progress = progress {
            let cppProgress: @convention(c) (UnsafeMutableRawPointer?, Double, Double) -> Bool = {
                cppContext, percentage, increment in
                let progressWrap: ValueWrap<ProgressUpdate>? = ObjectBridge.extractTypedSwiftObject(cppContext)
                guard let progressWrap = progressWrap else {
                    return false
                }
                return progressWrap.value(percentage, increment)
            }
            let progressWrap = ValueWrap(progress)
            let progressWrapPointer = ObjectBridge.getUntypeSwiftObject(progressWrap)
            score = WCDBDatabaseRetrieve(database, cppProgress, progressWrapPointer, ObjectBridge.objectDestructor)
        } else {
            score = WCDBDatabaseRetrieve(database, nil, nil, nil)
        }
        if score < 0 {
            throw getError()
        }
        return score
    }

    /// Deposit database.
    /// This method is designed for conditions where the database is corrupted and cannot be repaired temporarily. It can deposit the corrupted database to a new location and create a temporary database for the user. The contents of the deposited database will be restored when you recover the database.
    /// This method can be called multiple times without loss of data.
    /// - Throws: `Error`
    func deposit() throws {
        if !WCDBDatabaseDeposit(database) {
            throw getError()
        }
    }

    /// Check whether there is deposited data.
    func containDepositedFiles() -> Bool {
        return WCDBDatabaseContainDepositedFiles(database)
    }

    /// Remove deposited data.
    func removeDepositedFiles() throws {
        if !WCDBDatabaseRemoveDepositedFiles(database) {
            throw getError()
        }
    }

    /// Vacuum current database.
    /// It can be used to vacuum a database of any size with limited memory usage.
    func vacuum(with progress: ProgressUpdate?) throws {
        if let progress = progress {
            let cppProgress: @convention(c) (UnsafeMutableRawPointer?, Double, Double) -> Bool = {
                cppContext, percentage, increment in
                let progressWrap: ValueWrap<ProgressUpdate>? = ObjectBridge.extractTypedSwiftObject(cppContext)
                guard let progressWrap = progressWrap else {
                    return false
                }
                return progressWrap.value(percentage, increment)
            }
            let progressWrap = ValueWrap(progress)
            let progressWrapPointer = ObjectBridge.getUntypeSwiftObject(progressWrap)
            if !WCDBDatabaseVacuum(database, cppProgress, progressWrapPointer, ObjectBridge.objectDestructor) {
                throw getError()
            }
        } else {
            if !WCDBDatabaseVacuum(database, nil, nil, nil) {
                throw getError()
            }
        }
    }
}

// Migration
public extension Database {
    struct MigrationInfo {
        public var table: String = ""           // Target table of migration
        public var sourceTable: String?         // Source table of migration
        public var filterCondition: Expression? // Filter condition of source table
    }

    /// Triggered at any time when WCDB needs to know whether a table in the current database needs to migrate data,
    /// mainly including creating a new table, reading and writing a table, and starting to migrate a new table.
    /// If the current table does not need to migrate data, you need to set the sourceTable in `MigrationInfo` to nil.
    typealias TableFilter = (_ info: inout MigrationInfo) -> Void

    /// Configure which tables in the current database need to migrate data, and the source table they need to migrate data from.
    ///
    /// Once configured, you can treat the target table as if it already has all the data of the source table, and can read and write these data through the target table.
    /// WCDB will internally convert your CRUD operations on the target table into the CRUD operations on both the target table and the source table appropriately.
    /// You neither need to be aware of the existence of the source table, nor care about the progress of data migration.
    ///
    /// The column definition of the target table must be exactly the same as the column definition of the source table.
    /// The database does not record the state of the migration to disk, so if you have data to migrate, you need to use this function to configure the migration before executing any statements on current database.
    ///
    /// If the source table is not in the current database, the database containing the source table will be attached to the current database before the migration is complete.
    /// After migration, source tables will be dropped.
    ///
    /// - Parameter sourcePath: path of source database.
    /// - Parameter sourceCipher: cipher of source database. It is optional.
    /// - Parameter filter: see `TableFilter`.
    func addMigration(sourcePath: String?, sourceCipher: Data? = nil, _ filter: TableFilter?) {
        if let filter = filter {
            let filterWrap = ValueWrap(filter)
            let filterPointer = ObjectBridge.getUntypeSwiftObject(filterWrap)
            let cppFilter: @convention(c) (UnsafeMutableRawPointer?, UnsafePointer<CChar>, UnsafeMutableRawPointer, @convention(c) (UnsafeMutableRawPointer, UnsafePointer<CChar>?, CPPExpression) -> Void) -> Void  = {
                filter, table, cppInfo, setter in
                let filterWrap: ValueWrap<TableFilter>? = ObjectBridge.extractTypedSwiftObject(filter)
                if let filterWrap = filterWrap {
                    var info = MigrationInfo(table: String(cString: table))
                    filterWrap.value(&info)
                    if let sourceTable = info.sourceTable {
                        if let filtercCondition = info.filterCondition {
                            withExtendedLifetime(filtercCondition) {
                                setter(cppInfo, sourceTable.cString, $0.cppObj)
                            }
                        } else {
                            setter(cppInfo, sourceTable.cString, CPPExpression())
                        }
                    }
                }
            }
            if let sourceCipher = sourceCipher {
                sourceCipher.withUnsafeBytes { (buffer: UnsafeRawBufferPointer) in
                    WCDBDatabaseAddMigration(database, sourcePath?.cString, buffer.bindMemory(to: UInt8.self).baseAddress, Int32(buffer.count), cppFilter, filterPointer, ObjectBridge.objectDestructor)
                }
            } else {
                WCDBDatabaseAddMigration(database, sourcePath?.cString, nil, 0, cppFilter, filterPointer, ObjectBridge.objectDestructor)
            }
        } else {
            WCDBDatabaseAddMigration(database, sourcePath?.cString, nil, 0, nil, nil, nil)
        }
    }

    ///  Manually spend about 0.01 sec. to migrate data.
    ///  You can call this function periodically until all data is migrated.
    func stepMigration() throws {
        if !WCDBDatabaseStepMigration(database) {
            throw getError()
        }
    }

    /// Configure the database to automatically step migration every two seconds.
    func setAutoMigration(enable flag: Bool) {
        WCDBDatabaseEnableAutoMigration(database, flag)
    }

    /// Check if all tables in the database has finished migration.
    /// It only check an internal flag of database.
    func isMigrated() -> Bool {
        return WCDBDatabaseIsMigrated(database)
    }

    /// Triggered when a table or a database is migrated completely.
    /// When a table is migrated successfully, tableInfo will carry the information of the table.
    /// When a database is migrated, tableInfo has no value.
    typealias MigratedCallback = (_ database: Database, _ tableInfo: MigrationInfo?) -> Void

    /// Register a callback for migration notification.
    /// The callback will be called when each table completes the migration.
    ///
    /// - Parameter callback: see `MigratedCallback`.
    func setNotificationWhenMigrated(_ callback: MigratedCallback?) {
        if let callback = callback {
            let cppCallback: @convention(c) (UnsafeMutableRawPointer?, CPPDatabase, UnsafePointer<CChar>?, UnsafePointer<CChar>?) -> Void = {
                cppContext, cppDatabase, tableName, sourceTableName in
                let callbackWrap: ValueWrap<MigratedCallback>? = ObjectBridge.extractTypedSwiftObject(cppContext)
                guard let callbackWrap = callbackWrap else {
                    return
                }
                let database = Database(with: cppDatabase)
                var info: MigrationInfo?
                if let tableName = tableName,
                   let sourceTableName = sourceTableName {
                    info = MigrationInfo(table: String(cString: tableName),
                                         sourceTable: String(cString: sourceTableName))
                }
                callbackWrap.value(database, info)
            }
            let callbackWrap = ValueWrap(callback)
            let callbackWrapPointer = ObjectBridge.getUntypeSwiftObject(callbackWrap)
            WCDBDatabaseSetNotificationWhenMigrated(database, cppCallback, callbackWrapPointer, ObjectBridge.objectDestructor)
        } else {
            WCDBDatabaseSetNotificationWhenMigrated(database, nil, nil, nil)
        }
    }
}

// Compression
public extension Database {
    static let DictDefaultMatchValue = INT64_MAX
    typealias DictId = UInt8

    /// Train a zstd formalized dict with a set of sample strings.
    ///
    /// Note that the total size of all samples cannot exceed 4G.
    ///
    /// - Parameter strings: samples.
    /// - Parameter dictId: spercified id of the result dict. It can not be zero.
    /// - Returns: a dict of 100KB if succeed.
    /// - Throws: `Error`
    static func trainDict(with strings: [String], and dictId: DictId) throws -> Data {
        try withExtendedLifetime(strings) {
            var index = 0
            let dataEnumerator: () -> CPPData = {
                var ret = CPPData(buffer: nil, size: 0)
                if index >= strings.count {
                    return ret
                }
                let cstring = strings[index].cString
                guard let cstring = cstring else {
                    index += 1
                    return ret
                }
                ret.size = UInt64(strings[index].lengthOfBytes(using: .utf8))
                cstring.withMemoryRebound(to: UInt8.self, capacity: Int(ret.size)) { pointer in
                    ret.buffer = UnsafeMutablePointer(mutating: pointer)
                }
                index += 1
                return ret
            }
            let cppDataEnumerator: @convention(c) (UnsafeMutableRawPointer) -> CPPData = {
                cppContext in
                let enumeratorWrap: ValueWrap<() -> CPPData>? = ObjectBridge.extractTypedSwiftObject(cppContext)
                guard let enumeratorWrap = enumeratorWrap else {
                    return CPPData(buffer: nil, size: 0)
                }
                return enumeratorWrap.value()
            }
            let enumeratorWrap = ValueWrap(dataEnumerator)
            let enumeratorWrapPointer = ObjectBridge.getUntypeSwiftObject(enumeratorWrap)
            let dictData = WCDBDatabaseTrainDict(dictId, cppDataEnumerator, enumeratorWrapPointer)
            ObjectBridge.objectDestructor(enumeratorWrapPointer)
            if dictData.size > 0, let buffer = dictData.buffer {
                let dict = Data(bytes: buffer, count: Int(dictData.size))
                free(buffer)
                return dict
            }
            let cppError = WCDBCoreGetThreadedError()
            throw ErrorBridge.getErrorFrom(cppError: cppError)
        }
    }

    /// Train a zstd formalized dict with a set of sample datas.
    ///
    /// Note that the total size of all samples cannot exceed 4G.
    ///
    /// - Parameter datas: samples.
    /// - Parameter dictId: spercified id of the result dict. It can not be zero.
    /// - Returns: a dict of 100KB if succeed.
    /// - Throws: `Error`
    static func trainDict(with datas: [Data], and dictId: DictId) throws -> Data {
        try withExtendedLifetime(datas) {
            var index = 0
            let dataEnumerator: () -> CPPData = {
                var ret = CPPData(buffer: nil, size: 0)
                if index >= datas.count {
                    return ret
                }
                ret.size = UInt64(datas[index].count)
                datas[index].withUnsafeBytes { pointer in
                    ret.buffer = UnsafeMutablePointer(mutating: pointer.bindMemory(to: UInt8.self).baseAddress)
                }
                index += 1
                return ret
            }
            let cppDataEnumerator: @convention(c) (UnsafeMutableRawPointer) -> CPPData = {
                cppContext in
                let enumeratorWrap: ValueWrap<() -> CPPData>? = ObjectBridge.extractTypedSwiftObject(cppContext)
                guard let enumeratorWrap = enumeratorWrap else {
                    return CPPData(buffer: nil, size: 0)
                }
                return enumeratorWrap.value()
            }
            let enumeratorWrap = ValueWrap(dataEnumerator)
            let enumeratorWrapPointer = ObjectBridge.getUntypeSwiftObject(enumeratorWrap)
            let dictData = WCDBDatabaseTrainDict(dictId, cppDataEnumerator, enumeratorWrapPointer)
            ObjectBridge.objectDestructor(enumeratorWrapPointer)
            if dictData.size > 0, let buffer = dictData.buffer {
                let dict = Data(bytes: buffer, count: Int(dictData.size))
                free(buffer)
                return dict
            }
            let cppError = WCDBCoreGetThreadedError()
            throw ErrorBridge.getErrorFrom(cppError: cppError)
        }
    }

    /// Register a zstd dict in to WCDB.
    /// You must register a dict before using it.
    ///
    /// - Parameter dict: dict data.
    /// - Parameter dictId: id of the dict. It can not be zero.
    /// - Throws: `Error`
    static func register(dict: Data, with dictId: DictId) throws {
        try dict.withUnsafeBytes { (bytes: UnsafeRawBufferPointer) -> Void in
            if !WCDBDatabaseRegisterDict(bytes.bindMemory(to: UInt8.self).baseAddress, dict.count, dictId) {
                let cppError = WCDBCoreGetThreadedError()
                throw ErrorBridge.getErrorFrom(cppError: cppError)
            }
        }
    }

    class CompressionInfo {
        // The table to be compressed
        public let tableName: String

        // Configure to compress all data in the specified column with the default zstd compression algorithm.
        public func addZSTDNormalCompress(to property: PropertyConvertible) {
            let column = property.asColumn()
            withExtendedLifetime(column) {
                WCDBDatabaseSetZSTDNormalCompress(cppInfo, $0.cppObj)
            }
        }

        // Configure to compress all data in the specified column with a registered zstd dict.
        public func addZSTDDictCompress(to property: PropertyConvertible, withDict id: DictId) {
            let column = property.asColumn()
            withExtendedLifetime(column) {
                WCDBDatabaseSetZSTDDictCompress(cppInfo, $0.cppObj, id)
            }
        }

        /// Configure to compress all data in the specified column with multi registered zstd dict.
        /// Which dict to use when compressing is based on the value of the specified matching column.
        /// You can use `DictDefaultMatchValue` to specify a default dict.
        ///
        /// Note that the matching column must be an integer column.
        public func addZSTDMultiDictCompress(to property: PropertyConvertible,
                                             withMatchProperty matchProperty: PropertyConvertible,
                                             andDicts dicts: [Int64/* Value of match column */: DictId]) {
            let column = property.asColumn()
            let matchColumn = matchProperty.asColumn()
            ObjectBridge.extendLifetime(column, matchColumn) {
                let keyBuffer: UnsafeMutableBufferPointer<Int64> = .allocate(capacity: dicts.count)
                let valueBuffer: UnsafeMutableBufferPointer<UInt8> = .allocate(capacity: dicts.count)
                var index = 0
                for (key, value) in dicts {
                    keyBuffer[index] = key
                    valueBuffer[index] = value
                    index += 1
                }
                WCDBDatabaseSetZSTDMultiDictCompress(self.cppInfo, column.cppObj, matchColumn.cppObj, keyBuffer.baseAddress, valueBuffer.baseAddress, Int32(dicts.count))
                keyBuffer.deallocate()
                valueBuffer.deallocate()
            }
        }

        internal init(with cppInfo: UnsafeMutableRawPointer, table name: String) {
            self.cppInfo = cppInfo
            self.tableName = name
        }
        private let cppInfo: UnsafeMutableRawPointer
    }

    /// Triggered at any time when WCDB needs to know whether a table in the current database needs to compress data,
    /// mainly including creating a new table, reading and writing a table,and starting to compress a new table.
    /// If the current table does not need to compress data, you don't need to config `CompressionInfo`.
    typealias CompressionFilter = (_ info: inout CompressionInfo) -> Void

    /// Configure which tables in the current database need to compress data.
    /// Once configured, newly written data will be compressed immediately and synchronously,
    /// and you can use `Database.stepCompression()` and `Database.enableAutoCompression()` to compress existing data.
    ///
    /// - Parameter filter: a filter to configure which tables need to be compressed and how to compress them.
    func setCompression(with filter: CompressionFilter?) {
        if let filter = filter {
            let filterWrap = ValueWrap(filter)
            let filterPointer = ObjectBridge.getUntypeSwiftObject(filterWrap)
            let cppFilter: @convention(c) (UnsafeMutableRawPointer, UnsafePointer<CChar>, UnsafeMutableRawPointer) -> Void  = {
                cppContext, table, cppInfo in
                let filterWrap: ValueWrap<CompressionFilter>? = ObjectBridge.extractTypedSwiftObject(cppContext)
                guard let filterWrap = filterWrap else {
                    return
                }
                var info = CompressionInfo(with: cppInfo, table: String(cString: table))
                filterWrap.value(&info)
            }
            WCDBDatabaseSetCompression(database, cppFilter, filterPointer, ObjectBridge.objectDestructor)
        } else {
            WCDBDatabaseSetCompression(database, nil, nil, nil)
        }
    }

    /// Configure not to compress new data written to the current database.
    /// This configuration is mainly used to deal with some emergency scenarios.
    /// It allows already compressed data to be read normally, but new data is no longer compressed.
    /// - Parameter disable: disable compression or not.
    func disableCompressNewData(_ disable: Bool) {
        WCDBDatabaseDisableCompressNewData(database, disable)
    }

    /// Manually compress 100 rows of existing data.
    /// You can call this method periodically until all data is compressed.
    ///
    /// - Throws: `Error`
    func stepCompression() throws {
        if !WCDBDatabaseStepCompression(database) {
            throw getError()
        }
    }

    /// Configure the database to automatically compress 100 rows of data every two seconds.
    ///
    /// - Parameter enable: enable auto-compression or not.
    func enableAutoCompression(_ enable: Bool) {
        WCDBDatabaseEnableAutoCompression(database, enable)
    }

    /// Triggered when a table is compressed completely.
    /// When a table is compressed successfully, tableName will be valid.
    /// When a database is totally compressed, tableName will be nil.
    typealias CompressedCallback = (_ database: Database, _ table: String?) -> Void

    /// Register a callback for compression notification.
    /// The callback will be called when each table completes the compression.
    ///
    /// - Parameter callback: see `CompressedCallback`.
    func setNotificationWhenCompressed(_ callback: CompressedCallback?) {
        if let callback = callback {
            let cppCallback: @convention(c) (UnsafeMutableRawPointer, CPPDatabase, UnsafePointer<CChar>?) -> Void = {
                cppContext, cppDatabase, cppTableName in
                let callbackWrap: ValueWrap<CompressedCallback>? = ObjectBridge.extractTypedSwiftObject(cppContext)
                guard let callbackWrap = callbackWrap else {
                    return
                }
                let database = Database(with: cppDatabase)
                var tableName: String?
                if let cppTableName = cppTableName {
                    tableName = String(cString: cppTableName)
                }
                callbackWrap.value(database, tableName)
            }
            let callbackWrap = ValueWrap(callback)
            let callbackWrapPointer = ObjectBridge.getUntypeSwiftObject(callbackWrap)
            WCDBDatabaseSetNotificationWhenCompressed(database, cppCallback, callbackWrapPointer, ObjectBridge.objectDestructor)
        } else {
            WCDBDatabaseSetNotificationWhenCompressed(database, nil, nil, nil)
        }
    }

    /// Check if all tables in the database has finished compression.
    /// It only check an internal flag of database.
    ///
    /// - Returns: true if all tables in the database has finished compression.
    func isCompressed() -> Bool {
        return WCDBDatabaseIsCompressed(database)
    }
}

// FTS
public struct BuiltinTokenizer {
    /// The following four are sqlite built-in fts tokenizers.
    /// `Simple` can be used in fts3/4 and the others can be used in fts3/4/5.
    public static let Simple = String(cString: WCDBTokenizerSimple)
    public static let Porter = String(cString: WCDBTokenizerPorter)
    public static let ICU = String(cString: WCDBTokenizerICU)
    public static let Unicode61 = String(cString: WCDBTokenizerUnicode61)

    /// `OneOrBinary`is WCDB implemented tokenizers for fts3/4.
    /// It's compatible with WCDB tokenizer of older versions.
    ///
    /// When tokenizing, symbol characters are ignored by default.
    /// Consecutive English letters will be recognized as an English token, and this token will be stemmed using the porter stemming algorithm.
    /// Continuous Arabic numerals will be recognized as a single numeric token.
    /// For other Unicode characters, each character will be recognized as one token.
    ///
    /// For example, the sentence "The phone number of 张三 is 12345" will be split into these tokens: "the", "phone", "number", "of", "张", "三", "is", "12345".
    public static let OneOrBinary = String(cString: WCDBTokenizerLegacyOneOrBinary)

    /// The following two are WCDB implemented tokenizers for fts5.
    ///
    /// `Verbatim` has the same tokenize rules as `OneOrBinary`.
    /// `Pinyin` is designed for pinyin search. You can use the simplified or full pinyin of Chinese characters to search for Chinese characters.
    /// Before using this tokenizer, you need to use `config(pinyinDict:)` to configure the mapping relationship between Chinese characters and their pinyin.
    public static let Verbatim = String(cString: WCDBTokenizerVerbatim)
    public static let Pinyin = String(cString: WCDBTokenizerPinyin)

    public struct Parameter {
        /// The following three are optional parameters for WCDB implemented tokenizers.
        ///
        /// Configuring `NeedSymbol` allows the tokenizer to recognize each symbol character as a token.
        /// Configuring `SimplifyChinese` enables the tokenizer to convert each traditional Chinese character into a simplified Chinese character, so that you can use Simplified Chinese characters to search Traditional Chinese characters. Note that you need to call `config(traditionalChineseDict:)` to configure the mapping relationship between traditional Chinese characters and simplified Chinese characters before using the tokenizer.
        /// Configuring `SkipStemming` will disable the stemming during tokenization.
        public static let NeedSymbol = String(cString: WCDBTokenizerParameter_NeedSymbol)
        public static let SimplifyChinese = String(cString: WCDBTokenizerParameter_SimplifyChinese)
        public static let SkipStemming = String(cString: WCDBTokenizerParameter_SkipStemming)
    }
}

public struct BuiltinAuxiliaryFunction {
    /// `SubstringMatchInfo` is a WCDB implemented auxiliary function for fts5.
    ///
    /// When you need to concat multiple contents together with multi-level separators and save them in a single column of the fts5 table,
    /// you can use this auxiliary function to improve the accuracy of searching for the contents of this column.
    ///
    /// Suppose you have a friend named 张三, whose address is 广州 and 上海, and phone number is 12345 and 67890.
    /// You can use semicolon and comma as two-level separators to concat your friend's information into "张三;广州,上海;12345,67890",
    /// and save it in a column named "friends" in an fts5 table. Then you can use following code to search your friend:
    ///
    ///     fts5Table.getColumn(on: Column(named: fts5Table.name).substringMatchInfo().arguments(0, ";,"), where: Column(named: "friends").match("州"))
    ///
    /// The first argument of SubstringMatchInfo is a column named with table name.
    /// The second argument is the index of the colum you need to search. The index is are numbered starting from 0.
    /// The third parameter is the separator used when concating the content, which should be arranged according to their level.
    ///
    /// Then you will get this string result "1,0;广;州;3;". The result is made by concating a variety of information with the separator you passed in.
    /// The first part of the result "1,0" gives the hierarchical position of the matched content within matched column.
    /// The second part "广;州" is the original text of the matching content, you can use it for highlighting.
    /// The third part "3" is the byte offset of the matching content in the original text, you can use it to sort the results.
    ///
    /// The search results may contain some empty strings, which are invalid results.
    /// This kind of results appear when the content of some rows contain the tokens you are searching for,
    /// but these tokens are located in different parts separated by separators. You just need to ignore these results.
    public static let SubstringMatchInfo = String(cString: WCDBAuxiliaryFunction_SubstringMatchInfo)
}

public extension Database {
    /// Enable to automatically merge b-tree segments of fts5 index after modifying the content in fts5 table.
    ///
    /// The merge operation will be performed in a separate sub-thread, and will be suspended when it detects that other threads need to modify the fts table.
    /// So it will improve the performance of modifying the fts5 table.
    ///
    /// Using auto-merge, WCDB can maintain only one segment per level in the fts5 index structure.
    /// So it will significantly improve the search performance of the fts5 index.
    func setAutoMergeFTS5Index(enable: Bool) {
        WCDBDatabaseEnableAutoMergeFTS5Index(database, enable)
    }

    /// Setup tokenizer with name for current database.
    ///
    /// You can set up the built-in tokenizers of sqlite and the tokenizers implemented by WCDB directly.
    /// If you want to use your custom tokenizer, you should firstly register it through `register(tokenizer:of:of:)`
    ///
    /// - Parameter tokenizer: Name of tokenizer.
    func add(tokenizer: String) {
        WCTAPIBridge.configDefaultSymbolDetectorAndUnicodeNormalizer()
        WCDBDatabaseAddTokenizer(database, tokenizer.cString)
    }

    /// Register custom tokenizer.
    /// The custom tokenizer needs to implement the `Tokenizer` protocal.
    ///
    /// - Parameters:
    ///  - tokenizer:Type of tokenizer.
    ///  - name: Name of Tokenizer.
    ///  - version: version of fts.
    static func register<TokenizerType: Tokenizer>(tokenizer: TokenizerType.Type, of name: String, of version: FTSVersion) {
        FTSBridge.register(tokenizer: tokenizer, of: name, of: version)
    }

    /// Setup auxiliary function with name for current database.
    ///
    /// You can set up the built-in auxiliary functions of sqlite and the auxiliary functions implemented by WCDB directly.
    ///
    /// - Parameter auxFunction: Name of auxiliary function.
    func add(auxFunction: String) {
        WCDBDatabaseAddAuxiliaryFunction(database, auxFunction.cString)
    }

    /// Configure the mapping relationship between Chinese characters and their pinyin.
    /// It is designed for the builtin pinyin tokenizer.
    static func config(pinyinDict: [String /*Chinese character*/ : [String] /*Pinyin array*/]) {
        WCTAPIBridge.configPinyinDict(pinyinDict)
    }

    /// Configure the mapping relationship between traditional Chinese characters and simplified Chinese characters.
    static func config(traditionalChineseDict: [String /*Traditional Chinese character*/ : String /*Simplified Chinese character*/]) {
        WCTAPIBridge.configTraditionalChineseDict(traditionalChineseDict)
    }
}

// checkpoint
public extension Database {
    /// Checkpoint database in passive mode.
    /// WCDB will automatically checkpoint databases after it changes.
    /// You don't need to checkpoint periodically, and you don't even need to care about checkpoint.
    func passiveCheckpoint() throws {
        if !WCDBDatabasePassiveCheckpoint(database) {
            throw getError()
        }
    }

    /// Checkpoint database in truncate mode.
    func truncateCheckpoint() throws {
        if !WCDBDatabaseTruncateCheckpoint(database) {
            throw getError()
        }
    }
}

extension Database: HandleRepresentable {}

extension Database: InsertChainCallInterface {}
extension Database: UpdateChainCallInterface {}
extension Database: DeleteChainCallInterface {}
extension Database: RowSelectChainCallInterface {}
extension Database: SelectChainCallInterface {}
extension Database: MultiSelectChainCallInterface {}

extension Database: InsertInterface {}
extension Database: UpdateInterface {}
extension Database: DeleteInterface {}
extension Database: RowSelectInterface {}
extension Database: SelectInterface {}
extension Database: StatementSelectInterface {}
extension Database: TableInterface {}
extension Database: TransactionInterface {}
