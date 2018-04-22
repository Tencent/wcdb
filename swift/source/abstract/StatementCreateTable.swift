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
public final class StatementCreateTable: Statement {
    public private(set) var description: String = ""
    public var statementType: StatementType {
        return .createTable
    }

    public init() {}

    @discardableResult
    public func create(table: String, ifNotExists: Bool = true,
                       as statementSelect: StatementSelect) -> StatementCreateTable {
        description.append("CREATE TABLE ")
        if ifNotExists {
            description.append("IF NOT EXISTS ")
        }
        description.append("\(table) AS \(statementSelect.description)")
        return self
    }

    @discardableResult
    public func create(table: String,
                       ifNotExists: Bool = true,
                       with columnDefs: ColumnDef...,
                       and tableConstraints: [TableConstraint]? = nil) -> StatementCreateTable {
        return self.create(table: table, ifNotExists: ifNotExists, with: columnDefs, and: tableConstraints)
    }

    @discardableResult
    public func create(table: String,
                       ifNotExists: Bool = true,
                       with columnDefs: [ColumnDef],
                       and optionalTableConstraints: [TableConstraint]? = nil) -> StatementCreateTable {
        description.append("CREATE TABLE ")
        if ifNotExists {
            description.append("IF NOT EXISTS ")
        }
        description.append("\(table)(\(columnDefs.joined())")
        if let tableConstraints = optionalTableConstraints {
            description.append(", \(tableConstraints.joined())")
        }
        description.append(")")
        return self
    }

}
