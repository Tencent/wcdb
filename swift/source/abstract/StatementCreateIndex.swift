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

public final class StatementCreateIndex: Statement {
    public private(set) var description: String = ""
    public var statementType: StatementType {
        return .createIndex
    }

    public init() {}

    @discardableResult
    public func create(index: String, isUnique: Bool = false, ifNotExists: Bool = true) -> StatementCreateIndex {
        description.append("CREATE ")
        if isUnique {
            description.append("UNIQUE ")
        }
        description.append("INDEX ")
        if ifNotExists {
            description.append("IF NOT EXISTS ")
        }
        description.append(index)
        return self
    }

    @discardableResult
    public func on(table: String,
                   indexesBy columnIndexConvertibleList: ColumnIndexConvertible...) -> StatementCreateIndex {
        return on(table: table, indexesBy: columnIndexConvertibleList)
    }

    @discardableResult
    public func on(table: String,
                   indexesBy columnIndexConvertibleList: [ColumnIndexConvertible]) -> StatementCreateIndex {
        description.append(" ON \(table)(\(columnIndexConvertibleList.joined()))")
        return self
    }

    @discardableResult
    public func `where`(_ expressionConvertible: ExpressionConvertible) -> StatementCreateIndex {
        let expression = expressionConvertible.asExpression()
        if !expression.description.isEmpty {
            description.append(" WHERE \(expression.description)")
        }
        return self
    }
}
