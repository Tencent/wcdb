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

public final class ForeignKey: Describable {
    public private(set) var description: String

    public init(withForeignTable table: String, and columnConvertibleList: [ColumnConvertible]) {
        description = "REFERENCES \(table)"
        if !columnConvertibleList.isEmpty {
            description.append("(\(columnConvertibleList.joined(separateBy: ", ")))")
        }
    }

    public convenience init(withForeignTable table: String, and columnConvertibleList: ColumnConvertible...) {
        self.init(withForeignTable: table, and: columnConvertibleList)
    }

    public enum Action: Describable {
        case setNull
        case setDefault
        case cascade
        case restrict
        case noAction

        public var description: String {
            switch self {
            case .setNull:
                return "SET NULL"
            case .setDefault:
                return "SET DEFAULT"
            case .cascade:
                return "CASCADE"
            case .restrict:
                return "RESTRICT"
            case .noAction:
                return "NO ACTION"
            }
        }
    }

    @discardableResult
    public func onDelete(_ action: Action) -> ForeignKey {
        description.append(" ON DELETE \(action.description)")
        return self
    }

    @discardableResult
    public func onUpdate(_ action: Action) -> ForeignKey {
        description.append(" ON UPDATE \(action.description)")
        return self
    }

    @discardableResult
    public func match(name: String) -> ForeignKey {
        description.append(" MATCH \(name)")
        return self
    }

    public enum Deferrable: Describable {
        case deferred
        case immediate

        public var description: String {
            switch self {
            case .deferred:
                return "INITIALLY DEFERRED"
            case .immediate:
                return "INITIALLY IMMEDIATE"
            }
        }
    }

    @discardableResult
    public func deferrable(_ deferrable: Deferrable) -> ForeignKey {
        description.append(" DEFERRABLE \(deferrable.description)")
        return self
    }

    @discardableResult
    public func notDeferrable(_ deferrable: Deferrable) -> ForeignKey {
        description.append(" NOT DEFERRABLE \(deferrable.description)")
        return self
    }
}
