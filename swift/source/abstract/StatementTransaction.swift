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
public class StatementTransaction: Statement {
    public enum TransactionType {
        case begin
        case commit
        case rollback
    }

    public private(set) var transactionType: TransactionType?

    public init() {
        super.init(with: .transaction)
    }

    public enum Mode: CustomStringConvertible {
        case defered
        case immediate
        case exclusive

        public var description: String {
            switch self {
            case .defered:
                return "DEFERED"
            case .immediate:
                return "IMMEDIATE"
            case .exclusive:
                return "EXCLUSIVE"
            }
        }
    }

    @discardableResult
    public func begin(_ mode: Mode? = nil) -> StatementTransaction {
        self.transactionType = .begin
        description.append("BEGIN")
        if mode != nil {
            description.append(" \(mode!.description)")
        }
        return self
    }

    @discardableResult
    public func commit() -> StatementTransaction {
        self.transactionType = .commit
        description.append("COMMIT")
        return self
    }

    @discardableResult
    public func rollback() -> StatementTransaction {
        self.transactionType = .rollback
        description.append("ROLLBACK")
        return self
    }
}
