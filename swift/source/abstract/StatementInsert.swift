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
public final class StatementInsert: Statement {
    public private(set) var description: String = ""
    public var statementType: StatementType {
        return .insert
    }

    public init() {}

    @discardableResult
    public func insert(intoTable table: String,
                       with columnConvertibleList: ColumnConvertible...,
                       onConflict conflict: Conflict? = nil) -> StatementInsert {
        return insert(intoTable: table, with: columnConvertibleList, onConflict: conflict)
    }

    @discardableResult
    public func insert(intoTable table: String,
                       with columnConvertibleList: [ColumnConvertible]? = nil,
                       onConflict conflict: Conflict? = nil) -> StatementInsert {
        description.append("INSERT")
        if conflict != nil {
            description.append(" OR \(conflict!.description)")
        }
        description.append(" INTO \(table)")
        if columnConvertibleList != nil {
            description.append("(\(columnConvertibleList!.joined()))")
        }
        return self
    }

    @discardableResult
    public func values(_ expressionConvertibleList: ExpressionConvertible...) -> StatementInsert {
        return values(expressionConvertibleList)
    }

    @discardableResult
    public func values(_ expressionConvertibleList: [ExpressionConvertible]) -> StatementInsert {
        if !expressionConvertibleList.isEmpty {
            description.append(" VALUES(\(expressionConvertibleList.joined()))")
        }
        return self
    }
}
