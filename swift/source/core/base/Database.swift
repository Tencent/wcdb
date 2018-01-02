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
#if WCDB_IOS
import UIKit
#endif //WCDB_IOS

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
    public init(withFileURL url: URL) {
        #if swift(>=4.0)
        #else
            Error.fatalError("Swift 4.0 is required.")
        #endif
        #if WCDB_IOS
            DispatchQueue.once(name: "com.Tencent.WCDB.swift.purge", {
                let purgeFreeHandleQueue: DispatchQueue = DispatchQueue(label: "com.Tencent.WCDB.swift.purge")
                _ = NotificationCenter.default.addObserver(
                    forName: .UIApplicationDidReceiveMemoryWarning,
                    object: nil,
                    queue: nil,
                    using: { (_) in
                        purgeFreeHandleQueue.async {
                            Database.purge()
                        }
                    })
            })
        #endif //WCDB_IOS     
        super.init(with: HandlePool.getPool(withPath: url.standardizedFileURL.path,
                                            defaultConfigs: Database.defaultConfigs))
    }

    /// Init a database from existing tag.  
    /// Note that all database objects with same path share the same core. 
    /// So you can create multiple database objects. WCDB will manage them automatically.  
    /// Note that WCDB will not generate a sqlite handle until the first operation, 
    /// which is also called as lazy initialization.  
    ///
    /// - Parameter tag: An existing tag.
    /// - Throws: `Error` while tag is not exists
    public init(withExistingTag tag: Tag) throws {
        super.init(with: try HandlePool.getExistingPool(with: tag))
    }

    init(withExistingPath path: String) throws {
        super.init(with: try HandlePool.getExistingPool(withPath: path))
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
            return handlePool.tag
        }
        set {
            handlePool.tag = newValue
        }
    }

    private static var threadedHandles = ThreadLocal<[String: RecyclableHandle]>(defaultTo: [:])

    private func flowOut() throws -> RecyclableHandle {
        let threadedHandles = Database.threadedHandles.value
        if let handle = threadedHandles[path] {
            return handle
        }
        return try handlePool.flowOut()
    }

    /// Since WCDB is using lazy initialization, 
    /// `init(withPath:)`, `init(withFileURL:)` never failed even the database can't open. 
    /// So you can call this to check whether the database can be opened.  
    /// Return false if an error occurs during sqlite handle initialization.
    public var canOpen: Bool {
        return !handlePool.isDrained || ((try? handlePool.fillOne()) != nil)
    }

    /// Check database is already opened.
    public var isOpened: Bool {
        return !handlePool.isDrained
    }

    /// Check whether database is blockaded.
    public var isBlockaded: Bool {
        return handlePool.isBlockaded
    }

    public typealias OnClosed = HandlePool.OnDrained

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
    ///        `MultiSelect`, `CoreStatement`, `Transaction`. These objects should not be kept.  
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
    public func close(onClosed: OnClosed) rethrows {
        try handlePool.drain(onDrained: onClosed)
    }

    /// Close the database.
    public func close() {
        handlePool.drain()
    }

    /// Blockade the database.
    public func blockade() {
        handlePool.blockade()
    }

    /// Unblockade the database.
    public func unblockade() {
        handlePool.unblockade()
    }

    /// Purge all unused memory of this database.  
    /// WCDB will cache and reuse some sqlite handles to improve performance.   
    /// The max count of free sqlite handles is same
    /// as the number of concurrent threads supported by the hardware implementation.  
    /// You can call it to save some memory.
    public func purge() {
        handlePool.purgeFreeHandles()
    }

    /// Purge all unused memory of all databases.  
    /// Note that WCDB will call this interface automatically while it receives memory warning on iOS.
    public static func purge() {
        HandlePool.purgeFreeHandlesInAllPools()
    }

    override func prepare(_ statement: Statement) throws -> RecyclableHandleStatement {
        let recyclableHandle = try flowOut()
        return try prepare(statement, in: recyclableHandle)
    }

    /// Exec a specific sql.  
    /// Note that you can use this interface to execute a SQL that is not contained in the WCDB interface layer. 
    ///
    /// - Parameter statement: WINQ statement
    /// - Throws: `Error`
    public override func exec(_ statement: Statement) throws {
        try exec(statement, in: flowOut())
    }

    /// Separate interface of `run(transaction:)`  
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread.  
    /// To do a cross-thread transaction, use `getTransaction`.
    /// - Throws: `Error`
    public override func begin(_ mode: StatementTransaction.Mode = .immediate) throws {
        let statement = mode == .immediate ?
            CommonStatement.beginTransactionImmediate :
            StatementTransaction().begin(mode)
        let recyableHandlePool = try flowOut()
        try recyableHandlePool.raw.handle.exec(statement)
        Database.threadedHandles.value[path] = recyableHandlePool
    }

    /// Separate interface of `run(transaction:)`  
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread. 
    /// To do a cross-thread transaction, use `getTransaction`.
    /// - Throws: `Error`
    public override func commit() throws {
        let recyableHandlePool = try flowOut()
        try recyableHandlePool.raw.handle.exec(CommonStatement.commitTransaction)
        Database.threadedHandles.value.removeValue(forKey: path)
    }

    /// Separate interface of run(transaction:)
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread.  
    /// To do a cross-thread transaction, use `getTransaction`.
    /// - Throws: `Error`
    public override func rollback() throws {
        Database.threadedHandles.value.removeValue(forKey: path)
        let recyableHandlePool = try flowOut()
        try recyableHandlePool.raw.handle.exec(CommonStatement.rollbackTransaction)
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
        if Database.threadedHandles.value[path] != nil {
            return try embeddedTransaction()
        }
        return try run(transaction: embeddedTransaction)
    }
}

//Config
extension Database {

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
    public func setCipher(key optionalKey: Data?, pageSize: Int = 4096) {
        if let key = optionalKey {
            handlePool.setConfig(named: DefaultConfigOrder.cipher.description, with: { (handle: Handle) throws in
                let statementPragmaPageSize = StatementPragma().pragma(.cipherPageSize, to: pageSize)
                try handle.setCipher(key: key)
                try handle.exec(statementPragmaPageSize)
            })
        } else {
            handlePool.setConfig(named: DefaultConfigOrder.cipher.description, with: { (_) in })
        }
    }

    public typealias PerformanceTracer = Handle.PerformanceTracer
    public typealias SQLTracer = Handle.SQLTracer

    static private var performanceTracer = Atomic<PerformanceTracer?>()
    static private var sqlTracer = Atomic<SQLTracer?>()

    /// You can register a tracer to monitor the performance of all SQLs.  
    /// It returns  
    /// 1. The collection of SQLs and the executions count of each SQL.  
    /// 2. Time consuming in nanoseconds.  
    /// 3. Tag of database.  
    ///
    /// Note that:  
    /// 1. You should register trace before all db operations.   
    /// 2. Global tracer will be recovered by db tracer.  
    ///
    ///     Database.globalTrace(ofPerformance: { (tag, sqls, cost) in
    ///         if let wrappedTag = tag {
    ///             print("Tag: \(wrappedTag) ")
    ///         }else {
    ///             print("Nil tag")
    ///         }
    ///         sqls.forEach({ (arg) in
    ///             print("SQL: \(arg.key) Count: \(arg.value)")
    ///         })
    ///         print("Total cost \(cost) nanoseconds")
    ///     })
    ///
    /// Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
    ///
    /// - Parameter trace: trace. Nil to disable global preformance trace.
    public static func globalTrace(ofPerformance trace: @escaping PerformanceTracer) {
        performanceTracer.assign(trace)
    }
    public static func globalTrace(ofPerformance: Void?) {
        performanceTracer.assign(nil)
    }

    /// You can register a tracer to monitor the execution of all SQLs.  
    /// It returns a prepared or executed SQL.  
    /// Note that you should register trace before all db operations.
    ///
    ///     Database.globalTrace(ofSQL: { (sql) in
    ///         print("SQL: \(sql)")
    ///     })
    ///
    /// Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
    ///
    /// - Parameter trace: trace. Nil to disable global sql trace.
    public static func globalTrace(ofSQL trace: @escaping SQLTracer) {
        sqlTracer.assign(trace)
    }
    public static func globalTrace(ofSQL: Void?) {
        sqlTracer.assign(nil)
    }

    /// You can register a reporter to monitor all errors.
    ///
    ///     Database.globalTrace(ofError: { (error) in
    ///         switch error.type {
    ///         case .sqliteGlobal:
    ///             debugPrint("[WCDB][DEBUG] \(error.description)")
    ///         case .warning:
    ///             print("[WCDB][WARNING] \(error.description)")
    ///         default:
    ///             print("[WCDB][ERROR] \(error.description)")
    ///         }
    ///      })
    ///
    /// - Parameter errorReporter: report
    public static func globalTrace(ofError errorReporter: @escaping Error.Reporter) {
        Error.setReporter(errorReporter)
    }
    public static func globalTrace(ofError: Void?) {
        Error.setReporter(nil)
    }
    /// Reset global error trace to default trace.
    public static func resetGlobalTraceOfError() {
        Error.resetReporter()
    }

    private static let subthreadCheckpointDelay: TimeInterval = 2
    private static let subthreadCheckpointPages: Int = 1000
    private static let timedQueue = TimedQueue<String>(withDelay: subthreadCheckpointDelay)

    private static let defaultConfigs: Configs = Configs(ContiguousArray<Configs.Config>(arrayLiteral:
        Configs.Config(named: DefaultConfigOrder.fileProtection.description, with: { (handle: Handle) throws in
            #if WCDB_IOS
                try Handle.subfixs.forEach {
                    try File.addFirstUserAuthenticationFileProtection(atPath: handle.path+$0)
                }
            #endif //WCDB_IOS
        }, orderBy: DefaultConfigOrder.fileProtection.rawValue),
        Configs.Config(named: DefaultConfigOrder.trace.description, with: { (handle: Handle) throws in
            if let sqlTracer = Database.sqlTracer.raw {
                handle.trace(sql: sqlTracer)
            }
            if let performanceTracer = Database.performanceTracer.raw {
                handle.trace(performance: performanceTracer)
            }
        }, orderBy: DefaultConfigOrder.trace.rawValue),
        Configs.Config(emptyConfigNamed: DefaultConfigOrder.cipher.description,
                       orderBy: DefaultConfigOrder.cipher.rawValue),
        Configs.Config(named: DefaultConfigOrder.basic.description, with: { (handle: Handle) throws in
            guard !handle.isReadonly else {
                let handleStatement = try handle.prepare(CommonStatement.getJournalMode)
                try handleStatement.step()
                let journalMode: String = handleStatement.columnValue(atIndex: 0)
                try handleStatement.finalize()
                assert(journalMode.caseInsensitiveCompare("WAL") == ComparisonResult.orderedSame,
                       "It is not possible to open read-only WAL databases.")
                return
            }
            //Locking Mode
            do {
                //Get Locking Mode
                let handleStatement = try handle.prepare(CommonStatement.getLockingMode)
                try handleStatement.step()
                let lockingMode: String = handleStatement.columnValue(atIndex: 0)
                try handleStatement.finalize()
                //Set Locking Mode
                if lockingMode.caseInsensitiveCompare("NORMAL") != ComparisonResult.orderedSame {
                    try handle.exec(CommonStatement.enableLockingModeNormal)
                }
            }
            //Synchronous
            do {
                try handle.exec(CommonStatement.enableSynchronousNormal)
            }
            //Journal Mode
            do {
                //Get Journal Mode
                let handleStatement = try handle.prepare(CommonStatement.getJournalMode)
                try handleStatement.step()
                let journalMode: String = handleStatement.columnValue(atIndex: 0)
                try handleStatement.finalize()
                //Set Journal Mode
                if journalMode.caseInsensitiveCompare("WAL") != ComparisonResult.orderedSame {
                    try handle.exec(CommonStatement.enableJournalModeWAL)
                }
            }
            //Fullfsync
            do {
                try handle.exec(CommonStatement.enableFullfsync)
            }
        }, orderBy: DefaultConfigOrder.basic.rawValue),
        Configs.Config(emptyConfigNamed: DefaultConfigOrder.synchronous.description,
                       orderBy: DefaultConfigOrder.synchronous.rawValue),
        Configs.Config(named: DefaultConfigOrder.checkpoint.description, with: { (handle: Handle) throws in
            handle.register(onCommitted: { (handle, pages, _) in
                guard pages > subthreadCheckpointPages else {
                    return
                }
                DispatchQueue.once(name: "com.Tencent.WCDB.swift.checkpoint", {
                    DispatchQueue(label: "com.Tencent.WCDB.swift.checkpoint").async {
                        while true {
                            Database.timedQueue.wait(untilExpired: {
                                try? Database(withExistingPath: $0).exec(CommonStatement.checkpoint)
                            })
                        }
                    }
                })
                Database.timedQueue.reQueue(with: handle.path)
            })
        }, orderBy: DefaultConfigOrder.checkpoint.rawValue),
        Configs.Config(emptyConfigNamed: DefaultConfigOrder.tokenize.description,
                       orderBy: DefaultConfigOrder.tokenize.rawValue)
    ))

    /// Default config order
    public enum DefaultConfigOrder: Int, CustomStringConvertible {
        case fileProtection = 0
        case trace = 1
        case cipher = 2
        case basic = 3
        case synchronous = 4
        case checkpoint = 5
        case tokenize = 6

        public var description: String {
            switch self {
            case .fileProtection:
                return "fileProtection"
            case .trace:
                return "trace"
            case .cipher:
                return "cipher"
            case .basic:
                return "basic"
            case .synchronous:
                return "synchronous"
            case .checkpoint:
                return "checkpoint"
            case .tokenize:
                return "tokenize"
            }
        }
    }

    public typealias Config = HandlePool.Config
    public typealias ConfigOrder = HandlePool.ConfigOrder

    /// Set config for this database.
    ///
    /// Since WCDB is a multi-handle database, an executing handle will not apply this config immediately.  
    /// Instead, all handles will run this config before its next operation.
    ///
    ///     database.setConfig(named: "demo", with: { (handle: Handle) throws in
    ///         try handle.exec(StatementPragma().pragma(.secureDelete, to: true))
    ///     }, orderBy: 1)
    ///
    /// - Parameters:
    ///   - name: The Identifier for this config
    ///   - callback: config
    ///   - order: The smaller number is called first
    public func setConfig(named name: String, with callback: @escaping Config, orderBy order: ConfigOrder) {
        handlePool.setConfig(named: name, with: callback, orderBy: order)
    }

    /// This interface is equivalent to `database.setConfig(named: name, with: callback, orederBy: Int.max)`.
    ///
    /// - Parameters:
    ///   - name: The Identifier for this config
    ///   - callback: config
    public func setConfig(named name: String, with callback: @escaping Config) {
        handlePool.setConfig(named: name, with: callback)
    }

    /// Set Synchronous for this database. It will disable checkpoint opti to avoid performance degradation.  
    /// Synchronous can improve the stability of the database and reduce database damage, 
    /// but there will be performance degradation.
    ///
    /// - Parameter isFull: enable or disable full synchronous
    public func setSynchronous(isFull: Bool) {
        if isFull {
            handlePool.setConfig(named: DefaultConfigOrder.synchronous.description, with: { (handle: Handle) throws in
                try handle.exec(CommonStatement.enableSynchronousFull)
            })
            handlePool.setConfig(named: DefaultConfigOrder.checkpoint.description, with: { (handle: Handle) throws in
            })
        } else {
            handlePool.setConfig(named: DefaultConfigOrder.synchronous.description, with: { (handle: Handle) throws in
            })
            let checkpointConfig = Database.defaultConfigs.config(
                by: DefaultConfigOrder.checkpoint.description)
            assert(checkpointConfig != nil,
                   "It should not be failed. If you think it's a bug, please report an issue to us.")
            handlePool.setConfig(named: DefaultConfigOrder.checkpoint.description,
                                 with: checkpointConfig!)
        }
    }

    /// This interface is equivalent to `database.setTokenizes(tokenizes)`
    ///
    /// - Parameter tokenizes: registed tokenizeName. You can use builtin tokenizer named `.WCDB` or `.Apple`
    public func setTokenizes(_ tokenizes: Tokenize...) {
        setTokenizes(tokenizes)
    }

    /// Setup multiple tokenizers with names for current database.
    ///
    /// - Parameter tokenizes: registed tokenizeName. You can use builtin tokenizer named .WCDB or .Apple
    public func setTokenizes(_ tokenizes: [Tokenize]) {
        handlePool.setConfig(named: DefaultConfigOrder.tokenize.description) { (handle: Handle) throws in
            try tokenizes.forEach({ (tokenize) in
                let module = tokenize.module
                let handleStatement = try handle.prepare(CommonStatement.fts3Tokenizer)
                handleStatement.bind(module.name, toIndex: 1)
                handleStatement.bind(module.address, toIndex: 2)
                try handleStatement.step()
                try handleStatement.finalize()
            })
        }
    }
}

//Transaction
extension Database {
    /// Generation a `Transaction` object to do a transaction.
    ///
    /// - Returns: Transaction
    /// - Throws: `Error`
    public func getTransaction() throws -> Transaction {
        return Transaction(with: recyclableHandlePool, and: try flowOut())
    }
}

//Table
extension Database {
    /// Get a wrapper from an existing table.
    ///
    /// - Parameters:
    ///   - name: The name of the table.
    ///   - type: A class conform to TableCodable protocol.
    /// - Returns: Nil for a non-existent table.
    /// - Throws: `Error`
    public func getTable<Root: TableCodable>(
        named name: String,
        of type: Root.Type = Root.self) throws -> Table<Root>? {
        guard try isTableExists(name) else {
            return nil
        }
        return Table<Root>(withDatabase: self, named: name)
    }
}

//File
extension Database {
    /// Subfix of paths to all database-related files.
    public static var subfixs: [String] {
        return Handle.subfixs
    }

    /// URLs to all database-related files.
    public var urls: [URL] {
        return paths.map({ (path) -> URL in
            return URL(fileURLWithPath: path)
        })
    }

    /// Paths to all database-related files.
    public var paths: [String] {
        return Database.subfixs.map({ (subfix) -> String in
            return path+subfix
        })
    }

    /// Remove all database-related files.  
    /// You should call it on a closed database. Otherwise you will get a warning.
    ///
    /// - Throws: `Error`
    public func removeFiles() throws {
        if !isBlockaded || isOpened {
            Error.warning("Removing files on an opened database may cause unknown results")
        }
        try File.remove(files: paths)
    }

    /// This interface is equivalent `moveFiles(toDirectory:withExtraFiles:)`
    ///
    /// - Parameters:
    ///   - directory: destination
    ///   - extraFiles: extraFiles
    /// - Throws: `Error`
    public func moveFiles(toDirectory directory: String, withExtraFiles extraFiles: String...) throws {
        try moveFiles(toDirectory: directory, withExtraFiles: extraFiles)
    }

    /// Move all database-related files and some extra files to directory safely.  
    /// You should call it on a closed database. Otherwise you will get a warning and you may get a corrupted database.
    ///
    /// - Parameters:
    ///   - directory: destination
    ///   - extraFiles: extraFiles
    /// - Throws: `Error`
    public func moveFiles(toDirectory directory: String, withExtraFiles extraFiles: [String]) throws {
        try File.createDirectoryWithIntermediateDirectories(atPath: directory)
        var recovers: [String] = []
        let paths = self.paths + extraFiles
        do {
            try paths.forEach({ (path) in
                guard File.isExists(atPath: path) else {
                    return
                }
                let file = path.lastPathComponent
                let newPaths = directory.stringByAppending(pathComponent: file)
                if File.isExists(atPath: newPaths) {
                    try File.remove(files: [newPaths])
                }
                try File.hardlink(atPath: path, toPath: newPaths)
                recovers.append(newPaths)
            })
        } catch let error {
            try? File.remove(files: recovers)
            throw error
        }
        try? File.remove(files: paths)
    }

    /// Get the space used by the database files.  
    /// You should call it on a closed database. Otherwise you will get a warning.
    ///
    /// - Returns: The sum of files size in bytes.
    /// - Throws: `Error`
    public func getFilesSize() throws -> UInt64 {
        if !isBlockaded || isOpened {
            Error.warning("Getting files size on an opened database may get incorrect results")
        }
        return try File.getSize(ofFiles: paths)
    }
}

//Repair
extension Database {
    /// Backup metadata to recover.  
    /// Since metadata will be changed while a table or an index is created or dropped, 
    /// you should call this periodically.
    ///
    /// - Parameter key: The cipher key for backup. Nil for non-encrypted.
    /// - Throws: `Error`
    public func backup(withKey key: Data? = nil) throws {
        let handle = try flowOut()
        try handle.raw.handle.backup(withKey: key)
    }

    /// Recover data from a corruped db. You'd better to recover a closed database.  
    ///
    /// - Parameters:
    ///   - source: The path to the corrupted database
    ///   - pageSize: Page size of the corrupted database. It's default to 4096 on iOS. 
    ///               Page size never change unless you can call "PRAGMA page_size=NewPageSize" to set it. 
    ///               Also, you can call "PRAGMA page_size" to check the current value while database is not corrupted.
    ///   - databaseKey: The cipher key for corrupeted database
    ///   - backupKey: The cipher key for backup
    /// - Throws: `Error`
    public func recover(fromPath source: String,
                        withPageSize pageSize: Int32 = 4096,
                        databaseKey: Data? = nil,
                        backupKey: Data? = nil) throws {
        let handle = try flowOut()
        try handle.raw.handle.recover(fromPath: source,
                                      withPageSize: pageSize,
                                      databaseKey: databaseKey,
                                      backupKey: backupKey)
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
