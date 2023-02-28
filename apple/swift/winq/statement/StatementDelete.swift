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
public final class StatementDelete: Identifier<CPPStatementDelete>, Statement {
    public init() {
        super.init(with: WCDBStatementDeleteCreate())
    }

    @discardableResult
    public func with(recursive: Bool = false, _ expList: CommonTableExpression...) -> StatementDelete {
        self.with(recursive: recursive, expList)
    }

    @discardableResult
    public func with(recursive: Bool = false, _ expList: [CommonTableExpression]) -> StatementDelete {
        withExtendedLifetime(expList) {
            let cppExplist = $0.map { $0.cppObj }
            cppExplist.withUnsafeBufferPointer {
                WCDBStatementDeleteConfigWith(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        if recursive {
            WCDBStatementDeleteConfigRecursive(cppObj)
        }
        return self
    }

    @discardableResult
    public func delete(from qualifiedTableConvertible: QualifiedTableConvertible) -> StatementDelete {
        let table = qualifiedTableConvertible.asQualifiedTable()
        withExtendedLifetime(table) {
            WCDBStatementDeleteConfigDeleteFrom(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func `where`(_ expressionConvertible: ExpressionConvertible) -> StatementDelete {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBStatementDeleteConfigWhere(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func order(by orderConvertibleList: OrderingTermConvertible...) -> StatementDelete {
        return order(by: orderConvertibleList)
    }

    @discardableResult
    public func order(by orderConvertibleList: [OrderingTermConvertible]) -> StatementDelete {
        let orders = orderConvertibleList.map { $0.asOrder() }
        withExtendedLifetime(orders) {
            let cppOrders = orders.map { $0.cppObj }
            cppOrders.withUnsafeBufferPointer {
                WCDBStatementDeleteConfigOrder(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func limit(from expressionConvertibleFrom: ExpressionConvertible,
                      to expressionConvertibleTo: ExpressionConvertible) -> StatementDelete {
        let from = expressionConvertibleFrom.asExpression()
        let to = expressionConvertibleTo.asExpression()
        ObjectBridge.extendLifetime(from, to) {
            WCDBStatementDeleteConfigLimitRange(self.cppObj, from.cppObj, to.cppObj)
        }
        return self
    }

    @discardableResult
    public func limit(_ expressionConvertibleLimit: ExpressionConvertible) -> StatementDelete {
        let limit = expressionConvertibleLimit.asExpression()
        withExtendedLifetime(limit) {
            WCDBStatementDeleteConfigLimitCount(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func offset(_ expressionConvertibleOffset: ExpressionConvertible) -> StatementDelete {
        let offset = expressionConvertibleOffset.asExpression()
        withExtendedLifetime(offset) {
            WCDBStatementDeleteConfigOffset(cppObj, $0.cppObj)
        }
        return self
    }
}
