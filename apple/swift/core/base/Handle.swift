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

public final class Handle {
    private let recyclableHandle: RecyclableCPPHandle
    internal let cppHandle: CPPHandle

    internal init(withCPPHandle cppHandle: CPPHandle) {
        self.recyclableHandle = ObjectBridge.createRecyclableCPPObject(cppHandle)
        self.cppHandle = cppHandle
    }

    deinit {
        finalizeAllStatement()
    }

    internal func getError() -> WCDBError {
        let cppError = WCDBHandleGetError(cppHandle)
        return ErrorBridge.getErrorFrom(cppError: cppError)
    }

    public func exec(_ statement: Statement) throws {
        let executed = withExtendedLifetime(statement) {
            WCDBHandleExecute(cppHandle, $0.unmanagedCPPStatement)
        }
        if !executed {
            let cppError = WCDBHandleGetError(cppHandle)
            throw ErrorBridge.getErrorFrom(cppError: cppError)
        }
    }

    public func getOrCreatePreparedStatement(with statement: Statement) throws -> PreparedStatement {
        let cppHandleStatement = withExtendedLifetime(statement) { WCDBHandleGetOrCreatePreparedStatement(cppHandle, $0.unmanagedCPPStatement)
        }
        let preparedStatement = PreparedStatement(with: cppHandleStatement)
        if !WCDBHandleStatementCheckPrepared(cppHandleStatement) {
            throw getError()
        }
        return preparedStatement
    }

    public func finalizeAllStatement() {
        WCDBHandleFinalizeStatements(cppHandle)
    }

    public var changes: Int {
        return Int(WCDBHandleGetChange(cppHandle))
    }

    public var totalChanges: Int {
        return Int(WCDBHandleGetTotalChange(cppHandle))
    }

    public var lastInsertedRowID: Int64 {
        return WCDBHandleGetLastInsertedRowID(cppHandle)
    }
}

public protocol HandleRepresentable {
    func getHandle() throws -> Handle
}

extension Handle: HandleRepresentable {
    public func getHandle() throws -> Handle {
        return self
    }
}

extension Handle: RawStatementmentRepresentable {
    public func getRawStatement() -> CPPHandleStatement {
        return WCDBHandleGetMainStatement(cppHandle)
    }

    /// The wrapper of `sqlite3_prepare`
    ///
    /// - Throws: `Error`
    public func prepare(_ statement: Statement) throws {
        let succeed = withExtendedLifetime(statement) {
            WCDBHandleStatementPrepare(getRawStatement(), $0.unmanagedCPPStatement)
        }
        if !succeed {
            let cppError = WCDBHandleStatementGetError(getRawStatement())
            throw ErrorBridge.getErrorFrom(cppError: cppError)
        }
    }

    /// Check if current statement is prepared
    public var isPrepared: Bool {
        WCDBHandleStatementCheckPrepared(getRawStatement())
    }

    /// The wrapper of `sqlite3_finalize`
    public func finalize() {
        WCDBHandleStatementFinalize(getRawStatement())
    }
}

extension Handle: StatementInterface {}

extension Handle: InsertChainCallInterface {}
extension Handle: UpdateChainCallInterface {}
extension Handle: DeleteChainCallInterface {}
extension Handle: RowSelectChainCallInterface {}
extension Handle: SelectChainCallInterface {}
extension Handle: MultiSelectChainCallInterface {}

extension Handle: InsertInterface {}
extension Handle: UpdateInterface {}
extension Handle: DeleteInterface {}
extension Handle: RowSelectInterface {}
extension Handle: SelectInterface {}
extension Handle: TransactionInterface {}
