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
public final class StatementInsert: Identifier<CPPStatementInsert>, Statement {
    public init() {
        super.init(with: WCDBStatementInsertCreate())
    }

    @discardableResult
    public func with(recursive: Bool = false, _ expList: CommonTableExpression...) -> StatementInsert {
        self.with(recursive: recursive, expList)
    }

    @discardableResult
    public func with(recursive: Bool = false, _ expList: [CommonTableExpression]) -> StatementInsert {
        withExtendedLifetime(expList) {
            let cppExplist = $0.map { $0.cppObj }
            cppExplist.withUnsafeBufferPointer {
                WCDBStatementInsertConfigWith(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        if recursive {
            WCDBStatementInsertConfigRecursive(cppObj)
        }
        return self
    }

    @discardableResult
    public func insert(intoTable table: String) -> StatementInsert {
        WCDBStatementInsertConfigTable(cppObj, table.cString)
        return self
    }

    @discardableResult
    public func of(schema schemaConvertibale: SchemaConvertible) -> StatementInsert {
        let schema = schemaConvertibale.asSchema()
        withExtendedLifetime(schema) {
            WCDBStatementInsertConfigSchema(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func `as`(_ alias: String) -> StatementInsert {
        WCDBStatementInsertConfigAlias(cppObj, alias.cString)
        return self
    }

    @discardableResult
    public func onConflict(_ conflict: ConflictAction? = nil) -> StatementInsert {
        if let conflict = conflict {
            WCDBStatementInsertConfigConfiction(cppObj, conflict.cValue)
        }
        return self
    }

    @discardableResult
    public func columns(_ columnConvertibleList: ColumnConvertible...) -> StatementInsert {
        columns(columnConvertibleList)
    }

    @discardableResult
    public func columns(_ columnConvertibleList: [ColumnConvertible]) -> StatementInsert {
        let columns = columnConvertibleList.map {$0.asColumn()}
        withExtendedLifetime(columns) {
            let cppColumns = $0.map {$0.cppObj}
            cppColumns.withUnsafeBufferPointer {
                WCDBStatementInsertConfigColumns(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func values(_ expressionConvertibleList: ExpressionConvertible...) -> StatementInsert {
        values(expressionConvertibleList)
    }

    @discardableResult
    public func values(_ expressionConvertibleList: [ExpressionConvertible]) -> StatementInsert {
        let expressions = expressionConvertibleList.map { $0.asExpression() }
        withExtendedLifetime(expressions) {
            let cppExpressions = $0.map { $0.cppObj }
            cppExpressions.withUnsafeBufferPointer {
                WCDBStatementInsertConfigValues(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func values(_ select: StatementSelect) -> StatementInsert {
        withExtendedLifetime(select) {
            WCDBStatementInsertConfigSelect(cppObj, $0.cppObj)
        }
        return self
    }

    public func defaultValues() -> StatementInsert {
        WCDBStatementInsertConfigDefaultValues(cppObj)
        return self
    }

    public func upsert(_ upsert: Upsert) -> StatementInsert {
        withExtendedLifetime(upsert) {
            WCDBStatementInsertConfigUpsert(cppObj, $0.cppObj)
        }
        return self
    }
}
