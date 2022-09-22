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

public typealias Tag = Int

public final class Handle {
    private let recyclableHandle: RecyclableCPPHandle
    internal let handle: CPPHandle

    internal init(withCPPHandle cppHandle: CPPHandle) {
        self.recyclableHandle = ObjectBridge.createRecyclableCPPObject(cppHandle)
        self.handle = cppHandle
    }

    internal func getError() -> Error {
        let cppError = WCDBHandleGetError(handle)
        return ErrorBridge.getErrorFrom(cppError: cppError)
    }

    public func prepare(_ statement: Statement) throws -> HandleStatement {
        let cppHandleStatement = withExtendedLifetime(statement) {
            WCDBHandlePrepare(handle, $0.unmanagedCPPStatement)
        }
        let handleStatement = HandleStatement(with: cppHandleStatement, and: self)
        if !WCDBHandleStatementCheckPrepared(cppHandleStatement) {
            throw getError()
        }
        return handleStatement
    }

    public func exec(_ statement: Statement) throws {
        let excuted = withExtendedLifetime(statement) {
            WCDBHandleExcute(handle, $0.unmanagedCPPStatement)
        }
        if !excuted {
            let cppError = WCDBHandleGetError(handle)
            throw ErrorBridge.getErrorFrom(cppError: cppError)
        }
    }

    public var changes: Int {
        return Int(WCDBHandleGetChange(handle))
    }

    public var totalChanges: Int {
        return Int(WCDBHandleGetTotalChange(handle))
    }

    public var isReadonly: Bool {
        return WCDBHandleIsReadOnly(handle)
    }

    public var isInTransaction: Bool {
        return WCDBHandleIsInTransaction(handle)
    }

    public var lasInsertedRowID: Int64 {
        return WCDBHandleGetLastInsertedRowID(handle)
    }
}
