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

/// Thread-safe Transaction object
public final class Transaction: Core {
    private let recyclableHandle: RecyclableHandle
    /// Check whether is already in transaction.
    public private(set) var isInTransaction: Bool = false
    private var mutex: RecursiveMutex = RecursiveMutex()

    init(with recyclableHandlePool: RecyclableHandlePool, and recyclableHandle: RecyclableHandle) {
        self.recyclableHandle = recyclableHandle
        super.init(with: recyclableHandlePool)
    }

    deinit {
        if isInTransaction {
            try? rollback()
        }
    }

    private var handle: Handle {
        return recyclableHandle.raw.handle
    }

    override func prepare(_ statement: Statement) throws -> RecyclableHandleStatement {
        mutex.lock(); defer { mutex.unlock() }
        return try prepare(statement, in: recyclableHandle)
    }

    /// Exec a specific sql.  
    /// Note that you can use this interface to execute a SQL that is not contained in the WCDB interface layer. 
    ///
    /// - Parameter statement: WINQ statement
    /// - Throws: `Error`
    public override func exec(_ statement: Statement) throws {
        mutex.lock(); defer { mutex.unlock() }
        try exec(statement, in: recyclableHandle)
    }

    /// Prepare a specific sql.  
    /// Note that you can use this interface to prepare a SQL that is not contained in the WCDB interface layer
    ///
    /// - Parameter statement: WINQ statement
    /// - Returns: CoreStatement
    /// - Throws: `Error`
    public override func prepare(_ statement: Statement) throws -> CoreStatement {
        mutex.lock(); defer { mutex.unlock() }
        return try super.prepare(statement)
    }

    /// Check whether table exists
    ///
    /// - Parameter table: The name of the table to be checked.
    /// - Returns: True if table exists. False if table does not exist.
    /// - Throws: `Error`
    public override func isTableExists(_ table: String) throws -> Bool {
        mutex.lock(); defer { mutex.unlock() }
        return try super.isTableExists(table)
    }

    /// Run a transaction in closure
    ///
    ///     try transaction.run(transaction: { () throws -> Void in 
    ///         try transaction.insert(objects: objects, intoTable: table)
    ///     })
    ///
    /// - Parameter transaction: Operation inside transaction
    /// - Throws: `Error`
    public override func run(transaction: TransactionClosure) throws {
        mutex.lock(); defer { mutex.unlock() }
        try super.run(transaction: transaction)
    }

    /// Run a controllable transaction in closure
    ///
    ///     try transaction.run(controllableTransaction: { () throws -> Bool in 
    ///         try transaction.insert(objects: objects, intoTable: table)
    ///         return true // return true to commit transaction and return false to rollback transaction.
    ///     })
    ///
    /// - Parameter controllableTransaction: Operation inside transaction
    /// - Throws: `Error`
    public override func run(controllableTransaction: ControlableTransactionClosure) throws {
        mutex.lock(); defer { mutex.unlock() }
        try super.run(controllableTransaction: controllableTransaction)
    }

    /// Separate interface of `run(transaction:)`  
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread.  
    /// - Throws: `Error`
    public override func begin(_ mode: StatementTransaction.Mode = .immediate) throws {
        mutex.lock(); defer { mutex.unlock() }
        let statement = mode == .immediate ?
            CommonStatement.beginTransactionImmediate :
            StatementTransaction().begin(mode)
        try handle.exec(statement)
        isInTransaction = true
    }

    /// Separate interface of `run(transaction:)`  
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread. 
    /// - Throws: `Error`
    public override func commit() throws {
        mutex.lock(); defer { mutex.unlock() }
        try handle.exec(CommonStatement.commitTransaction)
        isInTransaction = false
    }

    /// Separate interface of run(transaction:)
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread.  
    /// - Throws: `Error`
    public override func rollback() throws {
        mutex.lock(); defer { mutex.unlock() }
        isInTransaction = false
        try handle.exec(CommonStatement.rollbackTransaction)
    }

    /// Run a embedded transaction in closure  
    /// The embedded transaction means that it will run a transaction if it's not in other transaction, 
    /// otherwise it will be executed within the existing transaction.
    ///
    ///     try transaction.run(embeddedTransaction: { () throws -> Void in 
    ///         try transaction.insert(objects: objects, intoTable: table)
    ///     })
    ///
    /// - Parameter embeddedTransaction: Operation inside transaction
    /// - Throws: `Error`
    public override func run(embeddedTransaction: TransactionClosure) throws {
        mutex.lock(); defer { mutex.unlock() }
        if isInTransaction {
            return try embeddedTransaction()
        }
        return try run(transaction: embeddedTransaction)
    }

    /// The number of changed rows in the most recent call.  
    /// It should be called after executing successfully
    public var changes: Int {
        mutex.lock(); defer { mutex.unlock() }
        return handle.changes
    }
}

extension Transaction: InsertChainCallInterface {}
extension Transaction: UpdateChainCallInterface {}
extension Transaction: DeleteChainCallInterface {}
extension Transaction: RowSelectChainCallInterface {}
extension Transaction: SelectChainCallInterface {}
extension Transaction: MultiSelectChainCallInterface {}

extension Transaction: InsertInterface {}
extension Transaction: UpdateInterface {}
extension Transaction: DeleteInterface {}
extension Transaction: RowSelectInterface {}
extension Transaction: SelectInterface {}
extension Transaction: TableInterface {}
