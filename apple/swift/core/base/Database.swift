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

/// Thread-safe Database object
public final class Database: Core {

    /// Init a database from path.  
    /// Note that all database objects with same path share the same core.
    /// So you can create multiple database objects. WCDB will manage them automatically.  
    /// Note that WCDB will not generate a sqlite handle until the first operation, 
    /// which is also called as lazy initialization.
    ///
    /// - Parameter path: Path to your database
    public convenience init(withPath path: String) {
        self.init(withFileURL: URL(fileURLWithPath: path))
    }

    /// Init a database from file url.  
    /// Note that all database objects with same path share the same core. 
    /// So you can create multiple database objects. WCDB will manage them automatically.  
    /// Note that WCDB will not generate a sqlite handle until the first operation, 
    /// which is also called as lazy initialization.
    ///
    /// - Parameter url: File url to your database
    public convenience init(withFileURL url: URL) {
        #if swift(>=5)
        #else
            Error.fatalError("Swift 5 is required.")
        #endif
        let database = WCDBCoreCreateDatabase(url.standardizedFileURL.path)
        self.init(with: database)
    }

    /// The tag of the database. Default to nil.  
    /// You should set it on a database and can get it from all kind of Core objects, 
    /// including `Database`, `Table`, `Transaction`, `Select`, `RowSelect`, `MultiSelect`, `Insert`, `Delete`, 
    /// `Update` and so on.   
    /// Note that core objects with same path share this tag, even they are not the same object.
    ///
    ///     let database1 = Database(withPath: path)
    ///     let database2 = Database(withPath: path)
    ///     database1.tag = 1
    ///     print("Tag: \(database2.tag!)") // print 1
    ///
    public override var tag: Tag? {
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
    ///        `MultiSelect`, `CoreStatement`. These objects should not be kept.
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
        let closeCallback : @convention(block) () -> Void = {
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

    override func prepare(_ statement: Statement) throws -> HandleStatement {
        let handle = try getHandle()
        return try prepare(statement, in: handle)
    }

    /// Exec a specific sql.  
    /// Note that you can use this interface to execute a SQL that is not contained in the WCDB interface layer. 
    ///
    /// - Parameter statement: WINQ statement
    /// - Throws: `Error`
    public override func exec(_ statement: Statement) throws {
        try exec(statement, in: getHandle())
    }

    /// Separate interface of `run(transaction:)`  
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread.  
    /// To do a cross-thread transaction, use `getTransaction`.
    /// - Throws: `Error`
    public override func begin() throws {
        if !WCDBDatabaseBeginTransaction(database) {
            throw getError()
        }
    }

    /// Separate interface of `run(transaction:)`  
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread. 
    /// To do a cross-thread transaction, use `getTransaction`.
    /// - Throws: `Error`
    public override func commit() throws {
        if !WCDBDatabaseCommitTransaction(database) {
            throw getError()
        }
    }

    /// Separate interface of run(transaction:)
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread.  
    /// To do a cross-thread transaction, use `getTransaction`.
    /// - Throws: `Error`
    public override func rollback() {
        WCDBDatabaseRollbackTransaction(database)
    }

    /// Run a embedded transaction in closure  
    /// The embedded transaction means that it will run a transaction if it's not in other transaction, 
    /// otherwise it will be executed within the existing transaction.
    ///
    ///     try database.run(embeddedTransaction: { () throws -> Void in 
    ///         try database.insert(objects: objects, intoTable: table)
    ///     })
    ///
    /// - Parameter embeddedTransaction: Operation inside transaction
    /// - Throws: `Error`
    public override func run(embeddedTransaction: TransactionClosure) throws {
        if WCDBDatabaseIsInTransaction(database) {
            return try embeddedTransaction()
        }
        return try run(transaction: embeddedTransaction)
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
        let callback : @convention(block) (UnsafePointer<CChar>, UInt64, UnsafePointer<CChar>, Double) -> Void = {
            (path, handleId, sql, cost) in
            trace(String(cString: path), handleId, String(cString: sql), cost)
        }
        let imp = imp_implementationWithBlock(callback)
        WCDBDatabaseGlobalTracePerformance(imp)
    }
    static func globalTrace(ofPerformance: Void?) {
        WCDBDatabaseGlobalTracePerformance(nil)
    }

    /// You can register a tracer to monitor the execution of all SQLs.
    /// It returns
    /// 1. Every SQL executed by the database.
    /// 2. Path of database.
    /// 4. The id of the handle executing this SQL.
    /// Note that you should register trace before all db operations.
    ///
    ///     Database.globalTrace(ofSQL: { (path, handleId, sql) in
    ///         print("Path: \(path)")
    ///         print("The handle with id \(handleId) excuted \(sql)")
    ///     })
    ///
    /// Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
    ///
    /// - Parameter trace: trace. Nil to disable global sql trace.
    static func globalTrace(ofSQL trace: @escaping SQLTracer) {
        let callback : @convention(block) (UnsafePointer<CChar>, UInt64, UnsafePointer<CChar>) -> Void = {
            (path, handleId, sql) in
            trace(String(cString: path), handleId, String(cString: sql))
        }
        let imp = imp_implementationWithBlock(callback)
        WCDBDatabaseGlobalTraceSQL(imp)
    }
    static func globalTrace(ofSQL: Void?) {
        WCDBDatabaseGlobalTraceSQL(nil)
    }

    /// You can register a reporter to monitor all errors.
    ///
    ///     Database.globalTrace(ofError: { (error) in
    ///         print("[WCDB] \(error.description)")
    ///     })
    ///
    /// - Parameter errorReporter: report
    static func globalTrace(ofError errorReporter: @escaping (Error) -> Void) {
        let callback : @convention(block) (CPPError) -> Void = {
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
        let invocationBlock : @convention(block) (CPPHandle) -> Bool = {
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
            let uninvocationBlock : @convention(block) (CPPHandle) -> Bool = {
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

//    /// This interface is equivalent to `database.setTokenizes(tokenizes)`
//    ///
//    /// - Parameter tokenizes: registed tokenizeName. You can use builtin tokenizer named `.WCDB`
//    public func setTokenizes(_ tokenizes: Tokenize...) {
//        setTokenizes(tokenizes)
//    }
//
//    /// Setup multiple tokenizers with names for current database.
//    ///
//    /// - Parameter tokenizes: registed tokenizeName. You can use builtin tokenizer named .WCDB
//    public func setTokenizes(_ tokenizes: [Tokenize]) {
//        handlePool.setConfig(named: DefaultConfigOrder.tokenize.description) { (handle: Handle) throws in
//            try tokenizes.forEach({ (tokenize) in
//                let module = tokenize.module
//                let handleStatement = try handle.prepare(CommonStatement.fts3Tokenizer)
//                handleStatement.bind(module.name, toIndex: 1)
//                handleStatement.bind(module.address, toIndex: 2)
//                try handleStatement.step()
//                try handleStatement.finalize()
//            })
//        }
//    }
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
    func getTable<Root: WCTTableCoding>(
        named name: String,
        of type: Root.Type = Root.self) throws -> Table<Root>? {
        guard try isTableExists(name) else {
            return nil
        }
        return Table<Root>(withDatabase: self, named: name)
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
        let enumerator : @convention(block) (UnsafePointer<CChar>) -> Void = {
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

//// Repair
public extension Database {
    typealias OnCorrupted = (_ corruptedDatabase: Database) -> Void
    /// You can register a notification callback for database corruption.
    ///
    /// After database corruption, you can call retrieve method to repair the database, or just remove it.
    ///
    /// - Parameter callback: The callback of notification.
    func setNotification(whenCorrupted callback: OnCorrupted?) {
        if let callback = callback {
            let internalCallBack : @convention(block) (CPPDatabase) -> Void = {cppDatabase in
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
    /// You'd better not call this method on the main thread, because it is time consuming.
    func checkIfCorrupted() -> Bool {
        return WCDBDatabaseCheckIfCorrupted(database)
    }

    /// Check if the database has been marked as corrupted
    func isAlreadyCorrupted() -> Bool {
        return WCDBDatabaseCheckIsAlreadyCorrupted(database)
    }

    /// Set the flag to enable auto-backup.
    ///
    /// Once enabled, database will automatically backup itself after there are updates.
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
    /// The filter will be called for ecery table in database during the initialization of backup logic.
    /// If there are some tables that do not need to be backed up, return false when these table names are passed into the filter.
    ///
    /// - Parameter filter: The table filter. A table name will be pass to filter when it's called.
    func filterBackup(tableShouldBeBackedUp filter: BackupFiilter?) {
        if let filter = filter {
            let internalFilter : @convention(block) (UnsafePointer<CChar>) -> Bool = {tableName in
                return filter(String(cString: tableName))
            }
            let internalFilterImp = imp_implementationWithBlock(internalFilter)
            WCDBDatabaseFilterBackup(database, internalFilterImp)
        } else {
            WCDBDatabaseFilterBackup(database, nil)
        }
    }

    typealias RetrieveProgress = (_ percentage: Double, _ increment: Double) -> Void
    /// Recover data from a corruped db. You'd better to recover a closed database.
    /// You'd better not call this method on the main thread, because it is time consuming.
    ///
    /// - Parameter progress: A closure that receives the repair progress.
    /// - Returns: Percentage of repaired data. 0 means data recovery failed. 1 means data is fully recovered.
    func retrieve(with progress: RetrieveProgress?) -> Double {
        var internalProgressImp: IMP?
        if let progress = progress {
            let internalProgress : @convention(block) (OpaquePointer, Double, Double) -> Void = {
                _, percentage, increment in
                progress(percentage, increment)
            }
            internalProgressImp = imp_implementationWithBlock(internalProgress)

        }
        return WCDBDatabaseRetrieve(database, internalProgressImp)
    }

    /// Deposit database.
    /// This method is designed for conditions where the database is corrupted and cannot be repaired temporarily. It can deposit the corrupted database to a new location and create a temporary database for the user. The contents of the deposited database will be restored when you recover the database.
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
        if !WCDBDatabasePassiveCheckpoint(database) {
            throw getError()
        }
    }
}

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
