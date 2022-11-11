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
public final class HandleStatement {
    internal let recyclableStmt: RecyclableCPPHandleStatement
    internal let handle: Handle
    public let tag: String

    internal init(with stmt: CPPHandleStatement, and handle: Handle, and tag: String = "") {
        self.recyclableStmt = ObjectBridge.createRecyclableCPPObject(stmt)
        self.handle = handle
        self.tag = tag
    }

    deinit {
        finalize()
    }
}

public protocol RawStatementmentRepresentable {
    func getRawStatement() -> CPPHandleStatement
}

extension HandleStatement: RawStatementmentRepresentable {
    public func getRawStatement() -> CPPHandleStatement {
        return self.recyclableStmt.raw
    }
}

extension HandleStatement: StatementInterface {}
