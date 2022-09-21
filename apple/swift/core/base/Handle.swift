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

public final class Handle {
    private let recyclableHandle: RecyclableCPPHandle
    internal let cppHandle: CPPHandle
    private var handleStatementDic: [String: HandleStatement]

    internal init(withCPPHandle cppHandle: CPPHandle) {
        self.recyclableHandle = ObjectBridge.createRecyclableCPPObject(cppHandle)
        self.cppHandle = cppHandle
        handleStatementDic = [:]
    }

    deinit {
        finalizeAllStatement()
    }

    internal func getError() -> Error {
        let cppError = WCDBHandleGetError(cppHandle)
        return ErrorBridge.getErrorFrom(cppError: cppError)
    }

    public func exec(_ statement: Statement) throws {
        let excuted = withExtendedLifetime(statement) {
            WCDBHandleExcute(cppHandle, $0.unmanagedCPPStatement)
        }
        if !excuted {
            let cppError = WCDBHandleGetError(cppHandle)
            throw ErrorBridge.getErrorFrom(cppError: cppError)
        }
    }

    public func getOrCreateHandleStatement(withTag tag: String) -> HandleStatement {
        var handleStatment = handleStatementDic[tag]
        if handleStatment == nil {
            handleStatment = HandleStatement(with: WCDBHandleGetStatement(cppHandle), and: self, and: tag)
            handleStatementDic[tag] = handleStatment!
        }
        return handleStatment!
    }

    public func finalizeAllStatement() {
        for (_, handleStatment) in self.handleStatementDic {
            handleStatment.finalize()
            WCDBHandleReturnStatement(cppHandle, handleStatment.getRawStatement())
        }
        self.handleStatementDic.removeAll()
        finalize()
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
