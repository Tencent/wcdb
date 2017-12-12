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
public final class StatementCreateVirtualTable: Statement {
    public private(set) var description: String = ""
    public var statementType: StatementType {
        return .createVirtualTable
    }

    public init() {}

    @discardableResult
    public func create(virtualTable table: String, ifNotExists: Bool = true) -> StatementCreateVirtualTable {
        description.append("CREATE VIRTUAL TABLE ")
        if ifNotExists {
            description.append("IF NOT EXISTS ")
        }
        description.append(table)
        return self
    }

    @discardableResult
    public func using(module: String, arguments: [ModuleArgument]? = nil) -> StatementCreateVirtualTable {
        description.append(" USING \(module)")
        if arguments != nil && !arguments!.isEmpty {
            description.append("(\(arguments!.joined()))")
        }
        return self
    }

    @discardableResult
    public func using(module: String, arguments: ModuleArgument...) -> StatementCreateVirtualTable {
        return using(module: module, arguments: arguments)
    }
}
