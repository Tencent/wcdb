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

public class Database {
    private let recyclableHandlePool: RecyclableHandlePool
    
    public convenience init(withPath path: String) {
        self.init(withFileURL: URL(fileURLWithPath: path))
    }
    
    public init(withFileURL url: URL) {
        //TODO: File protection
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

    private static let defaultConfigs: Configs = Configs(
        Configs.Config(named: DefaultConfigOrder.trace.description, with: { (handle: Handle) throws in
            //TODO:
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
            //TODO:
        }, orderBy: DefaultConfigOrder.checkpoint.rawValue),
        Configs.Config(emptyConfigNamed: DefaultConfigOrder.tokenize.description, orderBy: DefaultConfigOrder.tokenize.rawValue)
    )
    
    public enum DefaultConfigOrder: Int, CustomStringConvertible {
        case trace = 0
        case cipher = 1
        case basic = 2
        case synchronous = 3
        case checkpoint = 4
        case tokenize = 5
        
        public var description: String {
            switch self {
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
}

//Basic
extension Database {
    public func transaction() throws -> Transaction {
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

    public func run(transaction: ControlableTransactionClosure) throws {
        try begin(.Immediate)
        var shouldRollback = true
        do {
            if try transaction() {
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

extension Database {
    public static let subfixs: [String] = ["", "-wal", "-journal", "-shm"]
    public var paths: [String] {
        return Database.subfixs.map({ (path) -> String in
            return self.path+path
        })
    }
    public func removeFiles() throws {
        if !isBlockaded || isOpened {
            Error.warning("Removing files on an opened database may cause unknown results")
        }
        try File.remove(files: paths)
    }
    //TODO: other file interface
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

