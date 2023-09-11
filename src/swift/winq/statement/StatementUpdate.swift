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
public final class StatementUpdate: Identifier<CPPStatementUpdate>, Statement {

    public init() {
        super.init(with: WCDBStatementUpdateCreate())
    }

    @discardableResult
    public func with(recursive: Bool = false, _ expList: CommonTableExpression...) -> StatementUpdate {
        self.with(recursive: recursive, expList)
    }

    @discardableResult
    public func with(recursive: Bool = false, _ expList: [CommonTableExpression]) -> StatementUpdate {
        withExtendedLifetime(expList) {
            let cppExplist = $0.map { $0.cppObj }
            cppExplist.withUnsafeBufferPointer {
                WCDBStatementUpdateConfigWith(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        if recursive {
            WCDBStatementUpdateConfigRecursive(cppObj)
        }
        return self
    }

    @discardableResult
    public func update(table qualifiedTableConvertible: QualifiedTableConvertible) -> StatementUpdate {
        let qualifiedTable = qualifiedTableConvertible.asQualifiedTable()
        withExtendedLifetime(qualifiedTable) {
            WCDBStatementUpdateConfigTable(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func onConflict(_ conflict: ConflictAction? = nil) -> StatementUpdate {
        if let conflict = conflict {
            WCDBStatementUpdateConfigConfiction(cppObj, conflict.cValue)
        }
        return self
    }

    @discardableResult
    public func set(_ columnConvertibleList: ColumnConvertible...) -> StatementUpdate {
        set(columnConvertibleList)
    }

    @discardableResult
    public func set(_ columnConvertibleList: [ColumnConvertible]) -> StatementUpdate {
        let columns = columnConvertibleList.map { $0.asColumn() }
        withExtendedLifetime(columns) {
            let cppColumns = columns.map { $0.cppObj }
            cppColumns.withUnsafeBufferPointer {
                WCDBStatementUpdateConfigColumns(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func to(_ value: ExpressionConvertible) -> StatementUpdate {
        let expression = value.asExpression()
        withExtendedLifetime(expression) {
            WCDBStatementUpdateConfigValue(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func `where`(_ condition: ExpressionConvertible) -> StatementUpdate {
        let expression = condition.asExpression()
        withExtendedLifetime(expression) {
            WCDBStatementUpdateConfigCondition(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func order(by orderConvertibleList: OrderingTermConvertible...) -> StatementUpdate {
        return order(by: orderConvertibleList)
    }

    @discardableResult
    public func order(by orderConvertibleList: [OrderingTermConvertible]) -> StatementUpdate {
        let orderingTerms = orderConvertibleList.map { $0.asOrder() }
        withExtendedLifetime(orderingTerms) {
            let cppOrderingTerms = $0.map { $0.cppObj }
            cppOrderingTerms.withUnsafeBufferPointer {
                WCDBStatementUpdateConfigOrders(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func limit(from expressionConvertibleFrom: ExpressionConvertible,
                      to expressionConvertibleTo: ExpressionConvertible) -> StatementUpdate {
        let from = expressionConvertibleFrom.asExpression()
        let to = expressionConvertibleTo.asExpression()
        ObjectBridge.extendLifetime(from, to) {
            WCDBStatementUpdateConfigLimitRange(self.cppObj, from.cppObj, to.cppObj)
        }
        return self
    }

    @discardableResult
    public func limit(_ expressionConvertibleLimit: ExpressionConvertible) -> StatementUpdate {
        let limit = expressionConvertibleLimit.asExpression()
        withExtendedLifetime(limit) {
            WCDBStatementUpdateConfigLimitCount(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func offset(_ expressionConvertibleOffset: ExpressionConvertible) -> StatementUpdate {
        let offset = expressionConvertibleOffset.asExpression()
        withExtendedLifetime(offset) {
            WCDBStatementUpdateConfigLimitOffset(cppObj, $0.cppObj)
        }
        return self
    }

}
