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
public final class StatementTransaction: Statement {
    public private(set) var description: String = ""
    public var statementType: StatementType {
        return .transaction
    }

    public init() {}

    public enum TransactionType: Describable {
        case begin
        case commit
        case rollback

        public var description: String {
            switch self {
            case .begin:
                return "BEGIN"
            case .commit:
                return "COMMIT"
            case .rollback:
                return "ROLLBACK"
            }
        }
    }

    public private(set) var transactionType: TransactionType?

    public enum Mode: Describable {
        case deferred
        case immediate
        case exclusive

        public var description: String {
            switch self {
            case .deferred:
                return "DEFERRED"
            case .immediate:
                return "IMMEDIATE"
            case .exclusive:
                return "EXCLUSIVE"
            }
        }
    }

    @discardableResult
    public func begin(_ mode: Mode? = nil) -> StatementTransaction {
        transactionType = .begin
        description.append(transactionType!.description)
        if mode != nil {
            description.append(" \(mode!.description)")
        }
        return self
    }

    @discardableResult
    public func commit() -> StatementTransaction {
        transactionType = .commit
        description.append(transactionType!.description)
        return self
    }

    @discardableResult
    public func rollback() -> StatementTransaction {
        transactionType = .rollback
        description.append(transactionType!.description)
        return self
    }
}
