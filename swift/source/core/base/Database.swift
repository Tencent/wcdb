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

public class Database {
    private let recyclableHandlePool: RecyclableHandlePool
    
    public convenience init(withPath path: String) {
        self.init(withFileURL: URL(fileURLWithPath: path))
    }
    
    #if WCDB_IOS
    private static let purgeFreeHandleQueue: DispatchQueue = DispatchQueue(label: "WCDB-PurgeFreeHandle")
    
    private static let once: Void = {
        _ = NotificationCenter.default.addObserver(forName: .UIApplicationDidReceiveMemoryWarning,
                                                   object: nil,
                                                   queue: nil, 
                                                   using: { (notification) in
                                                    Database.purgeFreeHandleQueue.async {
                                                        Database.purgeFreeHandlesInAllDatabase()
                                                    }
        })
    }()
    #endif //WCDB_IOS
    
    public init(withFileURL url: URL) {
        #if WCDB_IOS
        Database.once
        #endif //WCDB_IOS            
        self.recyclableHandlePool = HandlePool.getPool(withPath: url.standardizedFileURL.path, defaultConfigs: Database.defaultConfigs)
    }
    
    public init(with tag: Tag) {
        self.recyclableHandlePool = HandlePool.getPool(with: tag)!
    }
    
    private var handlePool: HandlePool {
        return recyclableHandlePool.raw
    }
    
    private static var threadedHandles = ThreadLocal<[String:RecyclableHandle]>(defaultTo: [:])
    
    private func flowOut() throws -> RecyclableHandle {
        let threadedHandles = Database.threadedHandles.value
        if let handle = threadedHandles[path] {
            return handle
        }
        return try handlePool.flowOut()
    }

    public var canOpen: Bool {
        return !handlePool.isDrained || ((try? handlePool.fillOne()) != nil)
    }
   
    public var isOpened: Bool {
        return !handlePool.isDrained
    }
    
    public typealias OnClosed = HandlePool.OnDrained
    public func close(onClosed: OnClosed? = nil) rethrows {
        handlePool.drain(onDrained: onClosed)
    }
    
    public func blockade() {
        handlePool.blockade()
    }
    
    public func unblockade() {
        handlePool.unblockade()
    }
    
    public var isBlockaded: Bool {
        return handlePool.isBlockaded
    }
    
    public func purgeFreeHandles() {
        handlePool.purgeFreeHandles()
    }
    
    public static func purgeFreeHandlesInAllDatabase() {
        HandlePool.purgeFreeHandlesInAllPool()
    }
}

//Config
extension Database {
    public func setCipher(key: Data, pageSize: Int = 4096) {
        handlePool.setConfig(named: DefaultConfigOrder.cipher.description, with: { (handle: Handle) throws in
            let statementPragmaPageSize = StatementPragma().pragma(.pageSize, to: pageSize)
            try handle.setCipher(key: key) 
            try handle.exec(statementPragmaPageSize) 
        })
    }

    public typealias PerformanceTracer = Handle.PerformanceTracer
    public typealias SQLTracer = Handle.SQLTracer

    static private var performanceTracer = Atomic<PerformanceTracer?>()
    static private var sqlTracer = Atomic<SQLTracer?>()
    
    public static func globalTrace(ofPerformance trace: @escaping PerformanceTracer) {
        performanceTracer.assign(trace)
    }
    public static func globalTrace(ofPerformance: Void?) {
        performanceTracer.assign(nil)
    }
    
    public static func globalTrace(ofSQL trace: @escaping SQLTracer) {
        sqlTracer.assign(trace)
    }
    public static func globalTrace(ofSQL: Void?) {
        sqlTracer.assign(nil)
    }
    
    public static func globalTrace(ofError errorReporter: @escaping Error.Reporter) {
        Error.setReporter(errorReporter)
    }
    public static func globalTrace(ofError: Void?) {
        Error.setReporter(nil)
    }
    public static func resetGlobalTraceOfError() {
        Error.resetReporter()
    }
    
    private static let timedQueue = TimedQueue<String>(withDelay: 2)
    private static let checkpointThread = DispatchQueue(label: "WCDB-"+Database.DefaultConfigOrder.checkpoint.description)
    
    private static let defaultConfigs: Configs = Configs(
        Configs.Config(named: DefaultConfigOrder.fileProtection.description, with: { (handle: Handle) throws in
            #if WCDB_IOS
                try handle.paths.forEach {
                    try File.addFirstUserAuthenticationFileProtection(atPath: $0)
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
        Configs.Config(emptyConfigNamed: DefaultConfigOrder.cipher.description, orderBy: DefaultConfigOrder.cipher.rawValue),
        Configs.Config(named: DefaultConfigOrder.basic.description, with: { (handle: Handle) throws in
            guard !handle.isReadonly else {
                return
            }
            do {
                let statementLockMode = StatementPragma().pragma(.lockingMode)
                let statementLockModeNormal = StatementPragma().pragma(.lockingMode, to: "NORMAL")
                let handleStatement = try handle.prepare(statementLockMode)
                try handleStatement.step()
                let lockingMode: String = handleStatement.columnValue(atIndex: 0)
                try handleStatement.finalize()
                if lockingMode.caseInsensitiveCompare("NORMAL") != ComparisonResult.orderedSame {
                    try handle.exec(statementLockModeNormal)
                }
            }
            do {
                let statementSynchronousNormal = StatementPragma().pragma(.synchronous, to: "NORMAL") 
                try handle.exec(statementSynchronousNormal) 
            }
            do {
                let statementJournalMode = StatementPragma().pragma(.journalMode)
                let statementJournalModeWAL = StatementPragma().pragma(.journalMode, to: "WAL")
                let handleStatement = try handle.prepare(statementJournalMode)
                try handleStatement.step()
                let journalMode: String = handleStatement.columnValue(atIndex: 0)
                try handleStatement.finalize()
                if journalMode.caseInsensitiveCompare("WAL") != ComparisonResult.orderedSame {
                    try handle.exec(statementJournalModeWAL)
                }
            }
        }, orderBy: DefaultConfigOrder.basic.rawValue),
        Configs.Config(emptyConfigNamed: DefaultConfigOrder.synchronous.description, orderBy: DefaultConfigOrder.synchronous.rawValue),
        Configs.Config(named: DefaultConfigOrder.checkpoint.description, with: { (handle: Handle) throws in
            handle.register(onCommitted: { (handle, pages, _) in
                guard pages > 1000 else {
                    return
                }
                Database.checkpointThread.async {
                    while true {
                        Database.timedQueue.wait(untilExpired: {
                            try? Database(withPath: $0).exec(StatementPragma().pragma(.walCheckpoint))
                        })
                    }
                }
            })
        }, orderBy: DefaultConfigOrder.checkpoint.rawValue),
        Configs.Config(emptyConfigNamed: DefaultConfigOrder.tokenize.description, orderBy: DefaultConfigOrder.tokenize.rawValue)
    )
    
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
    
    public func setConfig(named name: String, with callback: @escaping Config, orderBy order: ConfigOrder) {
        handlePool.setConfig(named: name, with: callback, orderBy: order)
    }
    
    public func setConfig(named name: String, with callback: @escaping Config) {
        handlePool.setConfig(named: name, with: callback)
    }
    
    public func setSynchronous(isFull: Bool) {
        if isFull {
            handlePool.setConfig(named: DefaultConfigOrder.synchronous.description, with: { (handle: Handle) throws in
                let statementSynchronousFull = StatementPragma().pragma(.synchronous, to: "FULL")
                try handle.exec(statementSynchronousFull) 
            })
            handlePool.setConfig(named: DefaultConfigOrder.checkpoint.description, with: { (handle: Handle) throws in
            })
        }else {
            handlePool.setConfig(named: DefaultConfigOrder.synchronous.description, with: { (handle: Handle) throws in
            })
            handlePool.setConfig(named: DefaultConfigOrder.checkpoint.description, with: Database.defaultConfigs.config(by: DefaultConfigOrder.checkpoint.description)!)
        }
    }
    
    public func trace(performance performanceTracer: @escaping PerformanceTracer) {
        handlePool.setConfig(named: DefaultConfigOrder.trace.description) { (handle) in
            handle.trace(performance: performanceTracer)
        }
    }
    
    public func setTokenizes(_ tokenizes: Tokenize...) {
        setTokenizes(tokenizes)
    }
    
    public func setTokenizes(_ tokenizes: [Tokenize]) {
        handlePool.setConfig(named: DefaultConfigOrder.tokenize.description) { (handle: Handle) throws in
            try tokenizes.forEach({ (tokenize) in
                let module = tokenize.module
                let handleStatement = try handle.prepare(StatementSelect.fts3Tokenizer)
                handleStatement.bind(module.name, toIndex: 1)
                handleStatement.bind(module.address, toIndex: 2)
                try handleStatement.step()
                try handleStatement.finalize()
            })
        }
    }
}

//Basic
extension Database {
    public func getTransaction() throws -> Transaction {
        let handle = try flowOut() 
        return Transaction(with: recyclableHandlePool, and: handle)
    }
}

//Core
extension Database: Core {    
    public var path: String {
        return handlePool.path
    }
    
    public var tag: Tag? {
        get {
            return handlePool.tag
        }
        set {
            handlePool.tag = newValue
        }
    }

    public func prepare(_ statement: Statement) throws -> CoreStatement {
        let recyclableHandle = try flowOut()
        let handleStatement = try recyclableHandle.raw.handle.prepare(statement)
        return CoreStatement(with: self, and: RecyclableHandleStatement(recyclableHandle: recyclableHandle, handleStatement: handleStatement))
    }
    
    public func exec(_ statement: Statement) throws {
        let recyclableHandle = try flowOut()
        try recyclableHandle.raw.handle.exec(statement)
    }
    
    public func isTableExists(_ table: String) throws -> Bool {
        let handle = try flowOut()
        let select = StatementSelect().select(1).from(table).limit(0)
        Error.threadedSlient.value = true
        let handleStatement = try? handle.raw.handle.prepare(select)
        Error.threadedSlient.value = false
        guard handleStatement != nil else {
            return false
        }
        try handleStatement!.step()
        return true
    }
    
    public func getTable<Root: CodableTable>(named name: String, ofType type: Root.Type = Root.self) throws -> Table<Root>? {
        guard try isTableExists(name) else {
            return nil
        }
        return Table<Root>(withDatabase: self, named: name)
    }
    
    public func begin() throws {
        try begin(.Immediate)
    }
    
    func begin(_ mode: StatementTransaction.Mode) throws {
        let recyclableHandle = try flowOut()
        try recyclableHandle.raw.handle.exec(StatementTransaction().begin(mode))
        Database.threadedHandles.value[path] = recyclableHandle
    }
    
    public func commit() throws {
        let recyclableHandle = try flowOut()
        try recyclableHandle.raw.handle.exec(StatementTransaction().commit())
        Database.threadedHandles.value.removeValue(forKey: path)
    }
    
    public func rollback() throws {
        let recyclableHandle = try flowOut()
        Database.threadedHandles.value.removeValue(forKey: path)
        try recyclableHandle.raw.handle.exec(StatementTransaction().rollback())
    }
    
    public func run(transaction: TransactionClosure) throws {
        try begin(.Immediate)
        do {
            try transaction()
            try commit()
        }catch let error {
            try! rollback()
            throw error
        }
    }

    public func run(controlableTransaction: ControlableTransactionClosure) throws {
        try begin(.Immediate)
        var shouldRollback = true
        do {
            if try controlableTransaction() {
                try commit()
            }else {
                shouldRollback = false
                try rollback()
            }
        }catch let error {
            if shouldRollback {
                try! rollback()
            }
            throw error
        }
    }
    
    public func run(embeddedTransaction: TransactionClosure) throws {
        if Database.threadedHandles.value[path] != nil {
            return try embeddedTransaction()
        }
        return try run(transaction: embeddedTransaction)
    }
}

//File
extension Database {
    public static var subfixs: [String] {
        return Handle.subfixs
    }
    
    public var urls: [URL] {
        return paths.map({ (path) -> URL in
            return URL(fileURLWithPath: path)
        })
    }
    
    public var paths: [String] {
        return Database.subfixs.map({ (subfix) -> String in
            return path+subfix
        })
    }
    
    public func removeFiles() throws {
        if !isBlockaded || isOpened {
            Error.warning("Removing files on an opened database may cause unknown results")
        }
        try File.remove(files: paths)
    }
    
    public func moveFiles(toDirectory directory: String, withExtraFiles extraFiles: String...) throws {
        try moveFiles(toDirectory: directory, withExtraFiles: extraFiles)
    }
    
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
                    try File.remove(files: newPaths)
                }
                try File.hardlink(atPath: path, toPath: newPaths)
                recovers.append(newPaths)
            })
        }catch let error {
            try? File.remove(files: recovers)
            throw error
        }
        try? File.remove(files: paths)
    }
    
    public func getFilesSize() throws -> UInt64 {
        if !isBlockaded || isOpened {
            Error.warning("Getting files size on an opened database may get incorrect results")
        }
        return try File.getSize(ofFiles: paths)
    }
}

//Repair
extension Database {
    public func backup(withKey key: Data? = nil) throws {
        let handle = try flowOut() 
        try handle.raw.handle.backup(withKey: key)        
    }
    
    public func recover(fromPath source: String, withPageSize pageSize: Int32, databaseKey: Data? = nil, backupKey: Data? = nil) throws {
        let handle = try flowOut()
        try handle.raw.handle.recover(fromPath: source, withPageSize: pageSize, databaseKey: databaseKey, backupKey: backupKey)
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

