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

public class Core: CoreRepresentable {
    final let recyclableHandlePool: RecyclableHandlePool
    final var handlePool: HandlePool {
        return recyclableHandlePool.raw
    }

    /// The tag of the related database.
    public var tag: Tag? {
        return handlePool.tag
    }

    /// The path of the related database.
    public final var path: String {
        return handlePool.path
    }

    init(with recyclableHandlePool: RecyclableHandlePool) {
        self.recyclableHandlePool = recyclableHandlePool
    }

    final func prepare(_ statement: Statement,
                       in recyclableHandle: RecyclableHandle) throws -> RecyclableHandleStatement {
        assert(statement.statementType != .transaction,
               "Using [begin], [commit], [rollback] or [Exec] method to do a transaction")
        let handleStatement = try recyclableHandle.raw.handle.prepare(statement)
        return RecyclableHandleStatement(recyclableHandle: recyclableHandle, handleStatement: handleStatement)
    }

    final func exec(_ statement: Statement, in recyclableHandle: RecyclableHandle) throws {
        assert(statement.statementType != .transaction,
               "Using [begin], [commit], [rollback] method to do a transaction")
        return try recyclableHandle.raw.handle.exec(statement)
    }

    func prepare(_ statement: Statement) throws -> RecyclableHandleStatement {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    func exec(_ statement: Statement) throws {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    /// Prepare a specific sql.  
    /// Note that you can use this interface to prepare a SQL that is not contained in the WCDB interface layer
    ///
    /// - Parameter statement: WINQ statement
    /// - Returns: CoreStatement
    /// - Throws: `Error`
    public func prepare(_ statement: Statement) throws -> CoreStatement {
        return CoreStatement(with: self, and: try prepare(statement))
    }

    public func isTableExists(_ table: String) throws -> Bool {
        let statementSelect = StatementSelect().select(1).from(table).limit(0)
        Error.threadedSlient.value = true; defer { Error.threadedSlient.value = false }
        do {
            let handleStatement: RecyclableHandleStatement = try prepare(statementSelect)
            try handleStatement.raw.step()
            return true
        } catch let error as Error {
            guard error.code.value == Int(SQLITE_ERROR) else {
                throw error
            }
        }
        return false
    }

    func begin(_ mode: StatementTransaction.Mode = .immediate) throws {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    func commit() throws {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    func rollback() throws {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    public typealias TransactionClosure = () throws -> Void

    public typealias ControlableTransactionClosure = () throws -> Bool

    /// Run a transaction in closure
    ///
    ///     try database.run(transaction: { () throws -> Void in 
    ///         try database.insert(objects: objects, intoTable: table)
    ///     })
    ///
    /// - Parameter transaction: Operation inside transaction
    /// - Throws: `Error`
    public func run(transaction: TransactionClosure) throws {
        try begin(.immediate)
        do {
            try transaction()
            try commit()
        } catch let error {
            try? rollback()
            throw error
        }
    }

    /// Run a controllable transaction in closure
    ///
    ///     try database.run(controllableTransaction: { () throws -> Bool in 
    ///         try database.insert(objects: objects, intoTable: table)
    ///         return true // return true to commit transaction and return false to rollback transaction.
    ///     })
    ///
    /// - Parameter controllableTransaction: Operation inside transaction
    /// - Throws: `Error`
    public func run(controllableTransaction: ControlableTransactionClosure) throws {
        try begin(.immediate)
        var shouldRollback = true
        do {
            if try controllableTransaction() {
                try commit()
            } else {
                shouldRollback = false
                try rollback()
            }
        } catch let error {
            if shouldRollback {
                try? rollback()
            }
            throw error
        }
    }

    func run(embeddedTransaction: TransactionClosure) throws {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }
}

protocol CoreRepresentable: class {
    var tag: Tag? {get}
    var path: String {get}
}
