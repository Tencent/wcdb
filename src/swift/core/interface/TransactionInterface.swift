//
// Created by qiuwenchen on 2022/9/13.
//

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

public protocol TransactionInterface {
    /// Separate interface of `run(transaction:)`
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread.
    /// - Throws: `Error`
    func begin() throws

    /// Separate interface of `run(transaction:)`
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread.
    /// - Throws: `Error`
    func commit() throws

    /// Separate interface of run(transaction:)
    /// You should call `begin`, `commit`, `rollback` and all other operations in same thread.
    /// Throws: `Error`
    func rollback() throws

    /// Check whether the current database has begun a transaction in the current thread.
    var isInTransaction: Bool { get }

    /// Run a transaction in closure. Transaction supports nesting.
    ///
    ///     try database.run(transaction: { _ in
    ///         try database.insert(objects, intoTable: table)
    ///     })
    ///
    /// - Parameter transaction: Operation inside transaction
    /// - Throws: `Error`
    typealias TransactionClosure = (Handle) throws -> Void
    func run(transaction: @escaping TransactionClosure) throws

    /// Run a controllable transaction in closure
    ///
    ///     try database.run(controllableTransaction: { _ in
    ///         try database.insert(objects, intoTable: table)
    ///         return true // return true to commit transaction and return false to rollback transaction.
    ///     })
    ///
    /// - Parameter controllableTransaction: Operation inside transaction
    /// - Throws: `Error`
    typealias ControlableTransactionClosure = (Handle) throws -> Bool
    func run(controllableTransaction: @escaping ControlableTransactionClosure) throws

    typealias PausableTransactionClosure = (Handle, inout Bool, Bool) throws -> Void
    /// Run a pausable transaction in block.
    ///
    /// Firstly, WCDB will begin a transaction and call the block.
    /// After the block is finished, WCDB will check whether the main thread is suspended due to the current transaction.
    /// If not, it will call the block again; if it is, it will temporarily commit the current transaction.
    /// Once database operations in main thread are finished, WCDB will rebegin a new transaction in the current thread and call the block.
    /// This process will be repeated until the second parameter of the block is specified as true, or some error occurs during the transaction.
    ///
    ///     try self.database.run(pausableTransaction: { (handle, stop, isNewTransaction) in
    ///         if (isNewTransaction) {
    ///             // Do some initialization for new transaction.
    ///         }
    ///
    ///         // Perform a small amount of data processing.
    ///
    ///         if( All database operations are finished ) {
    ///             stop = true;
    ///         }
    ///     }
    ///
    /// - Parameter pausableTransaction: Operation inside transaction for one loop.
    /// - Throws: `Error`
    func run(pausableTransaction: @escaping PausableTransactionClosure) throws
}

extension TransactionInterface where Self: HandleRepresentable {
    public func begin() throws {
        let handle = try getHandle(writeHint: true)
        if !WCDBHandleBeginTransaction(handle.cppHandle) {
            throw handle.getError()
        }
    }

    public func commit() throws {
        let handle = try getHandle(writeHint: true)
        if !WCDBHandleCommitTransaction(handle.cppHandle) {
            throw handle.getError()
        }
    }

    public func rollback() throws {
        let handle = try getHandle(writeHint: true)
        WCDBHandleRollbackTransaction(handle.cppHandle)
    }

    public var isInTransaction: Bool {
        guard let handle = try? getHandle(writeHint: true) else {
            return false
        }
        return WCDBHandleIsInTransaction(handle.cppHandle)
    }

    public func run(transaction: @escaping TransactionClosure) throws {
        let handle = try getHandle(writeHint: true)
        if handle.isInTransaction {
            try transaction(handle)
            return
        }
        let cppTransaction: @convention(c) (UnsafeMutableRawPointer, CPPHandle) -> Bool = {
            cppContext, cppHandle in
            let transactionWrap: ValueWrap<(CPPHandle) -> Bool>? = ObjectBridge.extractTypedSwiftObject(cppContext)
            guard let transactionWrap = transactionWrap else {
                return false
            }
            return transactionWrap.value(cppHandle)
        }
        let transactionBlock: (CPPHandle) -> Bool = {
            cppHandle in
            let handle = Handle(withCPPHandle: cppHandle, database: getDatabase())
            var ret = true
            do {
                try transaction(handle)
            } catch {
                ret = false
            }
            return ret
        }
        let transactionWrap = ValueWrap(transactionBlock)
        let transactionWrapPointer = ObjectBridge.getUntypeSwiftObject(transactionWrap)
        let ret = WCDBHandleRunTransaction(handle.cppHandle, transactionWrapPointer, cppTransaction)
        ObjectBridge.releaseSwiftObject(transactionWrapPointer)
        if !ret {
            throw handle.getError()
        }
    }

    public func run(controllableTransaction: @escaping ControlableTransactionClosure) throws {
        var transactionRet = true
        let transactionBlock: (CPPHandle) -> Bool = {
            cppHandle in
            let handle = Handle(withCPPHandle: cppHandle, database: getDatabase())
            var ret = true
            do {
                transactionRet = try controllableTransaction(handle)
            } catch {
                ret = false
            }
            return ret && transactionRet
        }
        let transactionWrap = ValueWrap(transactionBlock)
        let transactionWrapPointer = ObjectBridge.getUntypeSwiftObject(transactionWrap)

        let cppTransaction: @convention(c) (UnsafeMutableRawPointer, CPPHandle) -> Bool = {
            cppContext, cppHandle in
            let transactionWrap: ValueWrap<(CPPHandle) -> Bool>? = ObjectBridge.extractTypedSwiftObject(cppContext)
            guard let transactionWrap = transactionWrap else {
                return false
            }
            return transactionWrap.value(cppHandle)
        }

        let handle = try getHandle(writeHint: true)
        let ret = WCDBHandleRunTransaction(handle.cppHandle, transactionWrapPointer, cppTransaction)
        ObjectBridge.releaseSwiftObject(transactionWrapPointer)

        if !ret && transactionRet {
            throw handle.getError()
        }
    }

    public func run(pausableTransaction: @escaping PausableTransactionClosure) throws {
        let handle = try getHandle(writeHint: true)
        let transactionBlock: (CPPHandle, UnsafeMutablePointer<Bool>, Bool) -> Bool = {
            _, cStop, isNewTransaction in
            var ret = true
            var stop = false
            do {
                try pausableTransaction(handle, &stop, isNewTransaction)
                cStop.pointee = stop
            } catch {
                ret = false
            }
            return ret
        }
        let transactionWrap = ValueWrap(transactionBlock)
        let transactionWrapPointer = ObjectBridge.getUntypeSwiftObject(transactionWrap)
        let cppTransaction: @convention(c) (UnsafeMutableRawPointer, CPPHandle, UnsafeMutablePointer<Bool>, Bool) -> Bool = {
            cppContext, cppHandle, canContinue, isNewTransaction in
            let transactionWrap: ValueWrap<(CPPHandle, UnsafeMutablePointer<Bool>, Bool) -> Bool>? = ObjectBridge.extractTypedSwiftObject(cppContext)
            guard let transactionWrap = transactionWrap else {
                return false
            }
            return transactionWrap.value(cppHandle, canContinue, isNewTransaction)
        }

        let ret = WCDBHandleRunPausableTransaction(handle.cppHandle, transactionWrapPointer, cppTransaction)
        ObjectBridge.objectDestructor(transactionWrapPointer)
        if !ret {
            throw handle.getError()
        }
    }
}
