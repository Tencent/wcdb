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

    /// Check whether database is blockaded.
    public var isBlockaded: Bool {
        return WCDBDatabaseIsBlockaded(database)
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
        let closeCallback: @convention(block) () -> Void = {
            do {
                try onClosed()
            } catch {
                callbackError = error
            }
        }
        let imp = imp_implementationWithBlock(closeCallback)
        WCDBDatabaseClose(database, imp)
        if callbackError != nil {
            throw callbackError!
        }
    }

    /// Close the database.
    public func close() {
        WCDBDatabaseClose(database, nil)
    }

    /// Blockade the database.
    public func blockade() {
        WCDBDatabaseBlockaded(database)
    }

    /// Unblockade the database.
    public func unblockade() {
        WCDBDatabaseUnblockaded(database)
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
    ///
    /// - Returns: A new `Handle`.
    /// - Throws: `Error`
    public func getHandle() throws -> Handle {
        let cppHandle = WCDBDatabaseGetHandle(database)
        let handle = Handle(withCPPHandle: cppHandle)
        if !WCDBHandleCheckValid(cppHandle) {
            throw getError()
        }
        return handle
    }

    /// Exec a specific sql.  
    /// Note that you can use this interface to execute a SQL that is not contained in the WCDB interface layer. 
    ///
    /// - Parameter statement: WINQ statement
    /// - Throws: `Error`
    public func exec(_ statement: Statement) throws {
        return try getHandle().exec(statement)
    }
}

// Config
public extension Database {

    /// Set cipher key for a database.   
    /// For an encrypted database, you must call it before all other operation.  
    /// The cipher page size defaults to 4096 in WCDB, but it defaults to 1024 in other databases. 
    /// So for an existing database created by other database framework, you should set it to 1024. 
    /// Otherwise, you'd better to use cipher page size with 4096 
    /// or simply call setCipherKey: interface to get better performance.
    ///
    /// - Parameters:
    ///   - optionalKey: Cipher key. Nil for no cipher.
    ///   - pageSize: Cipher page size.
    func setCipher(key optionalKey: Data?, pageSize: Int = 4096) {
        if let key = optionalKey {
            key.withUnsafeBytes { (buffer: UnsafeRawBufferPointer) in
                WCDBDatabaseConfigCipher(database, buffer.bindMemory(to: UInt8.self).baseAddress, Int32(buffer.count), Int32(pageSize))
            }
        } else {
            WCDBDatabaseConfigCipher(database, nil, 0, 0)
        }
    }

    typealias PerformanceTracer = (String, UInt64, String, Double) -> Void // Path, handleIdentifier, SQL, cost
    typealias SQLTracer = (String, UInt64, String) -> Void // Path, handleIdentifier, SQL

    /// You can register a tracer to monitor the performance of all SQLs.  
    /// It returns  
    /// 1. Every SQL executed by the database.
    /// 2. Time consuming in seconds.
    /// 3. Path of database.
    /// 4. The id of the handle executing this SQL.
    ///
    /// Note that:  
    /// 1. You should register trace before all db operations.   
    /// 2. Global tracer will be recovered by db tracer.  
    ///
    ///     Database.globalTrace(ofPerformance: { (path, handleId, sql, cost) in
    ///         print("Path: \(path)")
    ///         print("The handle with id \(handleId) took \(cost) seconds to execute \(sql)")
    ///     })
    ///
    /// Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
    ///
    /// - Parameter trace: trace. Nil to disable global preformance trace.
    static func globalTrace(ofPerformance trace: @escaping PerformanceTracer) {
        let callback: @convention(block) (UnsafePointer<CChar>, UInt64, UnsafePointer<CChar>, Double) -> Void = {
            (path, handleId, sql, cost) in
            trace(String(cString: path), handleId, String(cString: sql), cost)
        }
        let imp = imp_implementationWithBlock(callback)
        WCDBDatabaseGlobalTracePerformance(imp)
    }
    static func globalTrace(ofPerformance: Void?) {
        WCDBDatabaseGlobalTracePerformance(nil)
    }

    /// You can register a reporter to monitor all errors of current database.
    /// Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
    ///
    /// - Parameter trace: trace. Nil to disable preformance trace.
    func trace(ofPerformance trace: @escaping PerformanceTracer) {
        let callback: @convention(block) (UnsafePointer<CChar>, UInt64, UnsafePointer<CChar>, Double) -> Void = {
            (path, handleId, sql, cost) in
            trace(String(cString: path), handleId, String(cString: sql), cost)
        }
        let imp = imp_implementationWithBlock(callback)
        WCDBDatabaseTracePerformance(database, imp)
    }
    func trace(ofPerformance: Void?) {
        WCDBDatabaseTracePerformance(database, nil)
    }

    /// You can register a tracer to monitor the execution of all SQLs.
    /// It returns
    /// 1. Every SQL executed by the database.
    /// 2. Path of database.
    /// 3. The id of the handle executing this SQL.
    /// Note that you should register trace before all db operations.
    ///
    ///     Database.globalTrace(ofSQL: { (path, handleId, sql) in
    ///         print("Path: \(path)")
    ///         print("The handle with id \(handleId) executed \(sql)")
    ///     })
    ///
    /// Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
    ///
    /// - Parameter trace: trace. Nil to disable global sql trace.
    static func globalTrace(ofSQL trace: @escaping SQLTracer) {
        let callback: @convention(block) (UnsafePointer<CChar>, UInt64, UnsafePointer<CChar>) -> Void = {
            (path, handleId, sql) in
            trace(String(cString: path), handleId, String(cString: sql))
        }
        let imp = imp_implementationWithBlock(callback)
        WCDBDatabaseGlobalTraceSQL(imp)
    }
    static func globalTrace(ofSQL: Void?) {
        WCDBDatabaseGlobalTraceSQL(nil)
    }

    /// You can register a tracer to monitor the execution of all SQLs executed in the current database.
    /// Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
    ///
    /// - Parameter trace: trace. Nil to disable sql trace.
    func trace(ofSQL trace: @escaping SQLTracer) {
        let callback: @convention(block) (UnsafePointer<CChar>, UInt64, UnsafePointer<CChar>) -> Void = {
            (path, handleId, sql) in
            trace(String(cString: path), handleId, String(cString: sql))
        }
        let imp = imp_implementationWithBlock(callback)
        WCDBDatabaseTraceSQL(database, imp)
    }
    func trace(ofSQL: Void?) {
        WCDBDatabaseTraceSQL(database, nil)
    }

    /// You can register a reporter to monitor all errors.
    ///
    ///     Database.globalTrace(ofError: { (error) in
    ///         print("[WCDB] \(error.description)")
    ///     })
    ///
    /// - Parameter errorReporter: report. Nil to disable error trace.
    static func globalTrace(ofError errorReporter: @escaping (WCDBError) -> Void) {
        let callback: @convention(block) (CPPError) -> Void = {
            (cppError) in
            let error = ErrorBridge.getErrorFrom(cppError: cppError)
            errorReporter(error)
        }
        let imp = imp_implementationWithBlock(callback)
        WCDBDatabaseGlobalTraceError(imp)
    }
    static func globalTrace(ofError: Void?) {
        WCDBDatabaseGlobalTraceError(nil)
    }

    /// You can register a reporter to monitor all errors of current database.
    ///
    /// - Parameter errorReporter: report. Nil to disable error trace.
    func trace(ofError errorReporter: @escaping (WCDBError) -> Void) {
        let callback: @convention(block) (CPPError) -> Void = {
            (cppError) in
            let error = ErrorBridge.getErrorFrom(cppError: cppError)
            errorReporter(error)
        }
        let imp = imp_implementationWithBlock(callback)
        WCDBDatabaseTraceError(path, imp)
    }
    func trace(ofError: Void?) {
        WCDBDatabaseTraceError(path, nil)
    }

    enum Operation: Int {
        case Create = 0
        case SetTag = 1
        case OpenHandle = 2
    }

    typealias OperationTracer = (Database, Database.Operation) -> Void

    /// You can register a tracer to these database events:
    /// 1. creating a database object for the first time;
    /// 2. setting a tag on the database;
    /// 3. opening a new database handle.
    ///
    /// - Parameter trace: trace. Nil to disable error trace.
    static func globalTrace(ofDatabaseOperation trace: @escaping OperationTracer) {
        let callback: @convention(block) (CPPDatabase, Int) -> Void = {
            (cppDatabase, operation) in
            let database = Database(with: cppDatabase)
            trace(database, Operation(rawValue: operation) ?? .Create)
        }
        let imp = imp_implementationWithBlock(callback)
        WCDBDatabaseGlobalTraceOperation(imp)
    }

    static func globalTrace(ofDatabaseOperation trace: Void?) {
        WCDBDatabaseGlobalTraceOperation(nil)
    }

    enum ConfigPriority: Int32 {
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
        let invocationBlock: @convention(block) (CPPHandle) -> Bool = {
            cppHandle in
            let handle = Handle(withCPPHandle: cppHandle)
            var ret = true
            do {
                try invocation(handle)
            } catch {
                ret = false
            }
            return ret
        }
        let invocationImp = imp_implementationWithBlock(invocationBlock)
        var uninvocationImp: IMP?
        if let uninvocation = uninvocation {
            let uninvocationBlock: @convention(block) (CPPHandle) -> Bool = {
                cppHandle in
                let handle = Handle(withCPPHandle: cppHandle)
                var ret = true
                do {
                    try uninvocation(handle)
                } catch {
                    ret = false
                }
                return ret
            }
            uninvocationImp = imp_implementationWithBlock(uninvocationBlock)
        }
        WCDBDatabaseConfig(database, name, invocationImp, uninvocationImp, priority.rawValue)
    }
}

// Table
public extension Database {
    /// Get a wrapper from an existing table.
    ///
    /// - Parameters:
    ///   - name: The name of the table.
    ///   - type: A class conform to TableCodable protocol.
    /// - Returns: Nil for a non-existent table.
    /// - Throws: `Error`
    func getTable<Root: TableCodable>(
        named name: String,
        of type: Root.Type = Root.self) throws -> Table<Root>? {
        guard try isTableExists(name) else {
            return nil
        }
        return Table<Root>(withDatabase: self, named: name)
    }

    /// Check the existence of the table.
    ///
    /// - Parameter table: The name of the table.
    /// - Returns: false for a non-existent table.
    /// - Throws: `Error`
    func isTableExists(_ table: String) throws -> Bool {
        let ret = WCDBDatabaseExistTable(database, table)
        if !ret.hasValue {
            let error = WCDBDatabaseGetError(database)
            throw ErrorBridge.getErrorFrom(cppError: error)
        }
        return ret.value
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
        let enumerator: @convention(block) (UnsafePointer<CChar>) -> Void = {
            path in
            paths.append(String(cString: path))
        }
        let enumeratorImp = imp_implementationWithBlock(enumerator)
        WCDBDatabaseGetPaths(database, enumeratorImp)
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
    /// WCDB will asynchronously use `PRAGMA integrity_check` to check whether this database is truely corrupted.
    /// Once confirmed, WCDB will notify you through the callback registered by this method.
    /// In the callback, you can delete the corrupted database or try to repair the database.
    ///
    /// - Parameter callback: The callback of notification.
    func setNotification(whenCorrupted callback: OnCorrupted?) {
        if let callback = callback {
            let internalCallBack: @convention(block) (CPPDatabase) -> Void = {cppDatabase in
                let database = Database(with: cppDatabase)
                callback(database)
            }
            let internalCallBackImp = imp_implementationWithBlock(internalCallBack)
            WCDBDatabaseSetNotificationWhenCorrupted(database, internalCallBackImp)
        } else {
            WCDBDatabaseSetNotificationWhenCorrupted(database, nil)
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
            let internalFilter: @convention(block) (UnsafePointer<CChar>) -> Bool = {tableName in
                return filter(String(cString: tableName))
            }
            let internalFilterImp = imp_implementationWithBlock(internalFilter)
            WCDBDatabaseFilterBackup(database, internalFilterImp)
        } else {
            WCDBDatabaseFilterBackup(database, nil)
        }
    }

    typealias RetrieveProgress = (_ percentage: Double, _ increment: Double) -> Void
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
    func retrieve(with progress: RetrieveProgress?) -> Double {
        var internalProgressImp: IMP?
        if let progress = progress {
            let internalProgress: @convention(block) (OpaquePointer, Double, Double) -> Void = {
                _, percentage, increment in
                progress(percentage, increment)
            }
            internalProgressImp = imp_implementationWithBlock(internalProgress)

        }
        return WCDBDatabaseRetrieve(database, internalProgressImp)
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
    ///     fts5Table.getColumn(on: Expression.function(BuiltinAuxiliaryFunction.SubstringMatchInfo).invoke().arguments(Column(named: fts5Table.name), 0, ";,"), where: Column(named: "friends").match("州"))
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
extension Database: TableInterface {}
extension Database: TransactionInterface {}
