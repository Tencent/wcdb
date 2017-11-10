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
        case Begin
        case Commit
        case Rollback
    }
    
    public private(set) var transactionType: TransactionType? = nil

    public init() {
        super.init(with: .Transaction)
    }
    
    public enum Mode: CustomStringConvertible {
        case Defered
        case Immediate
        case Exclusive
        
        public var description: String {
            switch self {
            case .Defered:
                return "DEFERED"
            case .Immediate:
                return "IMMEDIATE"
            case .Exclusive:
                return "EXCLUSIVE"
            }
        }
    }

    @discardableResult
    public func begin(_ mode: Mode? = nil) -> StatementTransaction{
        self.transactionType = .Begin
        description.append("BEGIN")
        if mode != nil {
            description.append(" \(mode!.description)")
        }
        return self
    }
    
    @discardableResult
    public func commit() -> StatementTransaction{
        self.transactionType = .Commit
        description.append("COMMIT")
        return self
    }
    
    @discardableResult
    public func rollback() -> StatementTransaction{
        self.transactionType = .Rollback
        description.append("ROLLBACK")
        return self
    }
}
