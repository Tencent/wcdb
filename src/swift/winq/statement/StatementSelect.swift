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
public final class StatementSelect: Identifier<CPPStatementSelect>, Statement {

    public init() {
        super.init(with: WCDBStatementSelectCreate())
    }

    @discardableResult
    public func with(recursive: Bool = false, _ expList: CommonTableExpression...) -> StatementSelect {
        self.with(recursive: recursive, expList)
    }

    @discardableResult
    public func with(recursive: Bool = false, _ expList: [CommonTableExpression]) -> StatementSelect {
        withExtendedLifetime(expList) {
            let cppExplist = $0.map { $0.cppObj }
            cppExplist.withUnsafeBufferPointer {
                WCDBStatementSelectConfigWith(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        if recursive {
            WCDBStatementSelectConfigRecursive(cppObj)
        }
        return self
    }

    @discardableResult
    public func select(distinct: Bool = false,
                       _ resultColumnConvertibleList: ResultColumnConvertible...) -> StatementSelect {
        return select(distinct: distinct, resultColumnConvertibleList)
    }

    @discardableResult
    public func select(distinct: Bool = false,
                       _ resultColumnConvertibleList: [ResultColumnConvertible]) -> StatementSelect {
        let resultColumns = resultColumnConvertibleList.map { $0.asResultColumn() }
        withExtendedLifetime(resultColumns) {
            let cppColumns = $0.map { $0.cppObj }
            cppColumns.withUnsafeBufferPointer {
                WCDBStatementSelectConfigResultColumns(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        if distinct {
            WCDBStatementSelectConfigDistinct(cppObj)
        }
        return self
    }

    @discardableResult
    public func from(_ tableOrSubqueryConvertibleList: TableOrSubqueryConvertible...) -> StatementSelect {
        return from(tableOrSubqueryConvertibleList)
    }

    @discardableResult
    public func from(_ tableOrSubqueryConvertibleList: [TableOrSubqueryConvertible]) -> StatementSelect {
        let tableOrSubqueries = tableOrSubqueryConvertibleList.map { $0.asTableOrSubquery() }
        withExtendedLifetime(tableOrSubqueries) {
            let cppQueries = $0.map { $0.cppObj }
            cppQueries.withUnsafeBufferPointer {
                WCDBStatementSelectConfigFromTableOrSubqueries(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func `where`(_ expressionConvertible: ExpressionConvertible) -> StatementSelect {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBStatementSelectConfigWhere(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func group(by expressionConvertibleGroupList: ExpressionConvertible...) -> StatementSelect {
        return group(by: expressionConvertibleGroupList)
    }

    @discardableResult
    public func group(by expressionConvertibleGroupList: [ExpressionConvertible]) -> StatementSelect {
        let expressions = expressionConvertibleGroupList.map { $0.asExpression() }
        withExtendedLifetime(expressions) {
            let cppExpressions = $0.map { $0.cppObj }
            cppExpressions.withUnsafeBufferPointer {
                WCDBStatementSelectConfigGroups(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func having(_ expressionConvertibleHaving: ExpressionConvertible) -> StatementSelect {
        let having = expressionConvertibleHaving.asExpression()
        withExtendedLifetime(having) {
            WCDBStatementSelectConfigHaving(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func order(by orderConvertibleList: OrderingTermConvertible...) -> StatementSelect {
        return order(by: orderConvertibleList)
    }

    @discardableResult
    public func order(by orderConvertibleList: [OrderingTermConvertible]) -> StatementSelect {
        let orderingTerms = orderConvertibleList.map { $0.asOrder() }
        withExtendedLifetime(orderingTerms) {
            let cppOrderingTerms = $0.map { $0.cppObj }
            cppOrderingTerms.withUnsafeBufferPointer {
                WCDBStatementSelectConfigOrders(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func limit(from expressionConvertibleFrom: ExpressionConvertible,
                      to expressionConvertibleTo: ExpressionConvertible) -> StatementSelect {
        let from = expressionConvertibleFrom.asExpression()
        let to = expressionConvertibleTo.asExpression()
        ObjectBridge.extendLifetime(from, to) {
            WCDBStatementSelectConfigLimitRange(self.cppObj, from.cppObj, to.cppObj)
        }
        return self
    }

    @discardableResult
    public func limit(_ expressionConvertibleLimit: ExpressionConvertible) -> StatementSelect {
        let limit = expressionConvertibleLimit.asExpression()
        withExtendedLifetime(limit) {
            WCDBStatementSelectConfigLimitCount(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func offset(_ expressionConvertibleOffset: ExpressionConvertible) -> StatementSelect {
        let offset = expressionConvertibleOffset.asExpression()
        withExtendedLifetime(offset) {
            WCDBStatementSelectConfigLimitOffset(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func union() -> StatementSelect {
        WCDBStatementSelectConfigUnion(cppObj)
        return self
    }

    @discardableResult
    public func unionAll() -> StatementSelect {
        WCDBStatementSelectConfigUnionAll(cppObj)
        return self
    }

    @discardableResult
    public func intersect() -> StatementSelect {
        WCDBStatementSelectConfigIntersect(cppObj)
        return self
    }

    @discardableResult
    public func except() -> StatementSelect {
        WCDBStatementSelectConfigExcept(cppObj)
        return self
    }

    @discardableResult
    public func window(_ name: String) -> StatementSelect {
        WCDBStatementSelectConfigWindow(cppObj, name.cString)
        return self
    }

    @discardableResult
    public func `as`(_ windowDef: WindowDef) -> StatementSelect {
        withExtendedLifetime(windowDef) {
            WCDBStatementSelectConfigAs(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func value(_ expressionConvertibleList: ExpressionConvertible...) -> StatementSelect {
        self.value(expressionConvertibleList)
    }

    @discardableResult
    public func value(_ expressionConvertibleList: [ExpressionConvertible]) -> StatementSelect {
        let expressionList = expressionConvertibleList.map { $0.asExpression() }
        withExtendedLifetime(expressionList) {
            let cppExpressions = $0.map { $0.cppObj }
            cppExpressions.withUnsafeBufferPointer {
                WCDBStatementSelectConfigValues(cppObj, $0.baseAddress, Int32( $0.count ))
            }
        }
        return self
    }
}

extension StatementSelect: ExpressionConvertible, TableOrSubqueryConvertible {
    public func asExpression() -> Expression {
        return Expression(with: self)
    }
    public func asTableOrSubquery() -> TableOrSubquery {
        return TableOrSubquery(with: self)
    }
}
