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
    internal final let recyclableDatabase: RecyclableCPPDatabase
    internal final var database: CPPDatabase {
        return recyclableDatabase.raw
    }

    /// The tag of the related database.
    public var tag: Tag? {
        let tag = WCDBDatabaseGetTag(database)
        if tag != 0 {
            return tag
        }
        return nil
    }

    /// The path of the related database.
    public final var path: String {
        if let path = WCDBDatabaseGetPath(database) {
            return String(cString: path)
        }
        return ""
    }

    internal init(with cppDatabase: CPPDatabase) {
        DispatchQueue.once(name: "com.Tencent.WCDB.swift.purge", {
            ObjectBridge.initializeCPPAPI()
        })
        self.recyclableDatabase = ObjectBridge.createRecyclableCPPObject(cppDatabase)
    }

    internal func getHandle() throws -> Handle {
        let cppHandle = WCDBDatabaseGetHandle(database)
        let handle = Handle(withCPPHandle: cppHandle)
        if !WCDBHandleCheckValid(cppHandle) {
            throw getError()
        }
        return handle
    }

    internal func getError() -> Error {
        let cppError = WCDBDatabaseGetError(database)
        return ErrorBridge.getErrorFrom(cppError: cppError)
    }

    internal final func prepare(_ statement: Statement,
                                in handle: Handle) throws -> HandleStatement {
        let handleStatement = try handle.prepare(statement)
        return handleStatement
    }

    internal final func exec(_ statement: Statement, in handle: Handle) throws {
        return try handle.exec(statement)
    }

    internal func prepare(_ statement: Statement) throws -> HandleStatement {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    internal func exec(_ statement: Statement) throws {
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
        let ret = WCDBDatabaseExistTable(database, table)
        if !ret.hasValue {
            let error = WCDBDatabaseGetError(database)
            throw ErrorBridge.getErrorFrom(cppError: error)
        }
        return ret.value
    }

    func begin() throws {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    func commit() throws {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }

    func rollback() {
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
        try begin()
        do {
            try transaction()
            try commit()
        } catch let error {
            rollback()
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
        try begin()
        var shouldRollback = true
        do {
            if try controllableTransaction() {
                try commit()
            } else {
                shouldRollback = false
                rollback()
            }
        } catch let error {
            if shouldRollback {
                rollback()
            }
            throw error
        }
    }

    internal func run(embeddedTransaction: TransactionClosure) throws {
        fatalError("It should not be called. If you think it's a bug, please report an issue to us.")
    }
}

internal protocol CoreRepresentable: AnyObject {
    var tag: Tag? {get}
    var path: String {get}
}
