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
public final class Join: Identifier<CPPJoin> {

    public init(with subqueryConvertible: TableOrSubqueryConvertible) {
        let subquery = subqueryConvertible.asTableOrSubquery()
        let cppJoin = withExtendedLifetime(subquery) {
            WCDBJoinCreateWithTableOrSubquery($0.cppObj)
        }
        super.init(with: cppJoin)
    }

    @discardableResult
    public func join(_ subqueryConvertible: TableOrSubqueryConvertible) -> Join {
        let subquery = subqueryConvertible.asTableOrSubquery()
        withExtendedLifetime(subquery) {
            WCDBJoinWithJoin(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func with(_ subqueryConvertible: TableOrSubqueryConvertible) -> Join {
        let subquery = subqueryConvertible.asTableOrSubquery()
        withExtendedLifetime(subquery) {
            WCDBJoinWith(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func leftOuterJoin(_ subqueryConvertible: TableOrSubqueryConvertible) -> Join {
        let subquery = subqueryConvertible.asTableOrSubquery()
        withExtendedLifetime(subquery) {
            WCDBJoinWithLeftOuterJoin(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func leftJoin(_ subqueryConvertible: TableOrSubqueryConvertible) -> Join {
        let subquery = subqueryConvertible.asTableOrSubquery()
        withExtendedLifetime(subquery) {
            WCDBJoinWithLeftJoin(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func innerJoin(_ subqueryConvertible: TableOrSubqueryConvertible) -> Join {
        let subquery = subqueryConvertible.asTableOrSubquery()
        withExtendedLifetime(subquery) {
            WCDBJoinWithInnerJoin(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func crossJoin(_ subqueryConvertible: TableOrSubqueryConvertible) -> Join {
        let subquery = subqueryConvertible.asTableOrSubquery()
        withExtendedLifetime(subquery) {
            WCDBJoinWithCrossJoin(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func naturalJoin(_ subqueryConvertible: TableOrSubqueryConvertible) -> Join {
        let subquery = subqueryConvertible.asTableOrSubquery()
        withExtendedLifetime(subquery) {
            WCDBJoinWithNaturalJoin(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func naturalLeftOuterJoin(_ subqueryConvertible: TableOrSubqueryConvertible) -> Join {
        let subquery = subqueryConvertible.asTableOrSubquery()
        withExtendedLifetime(subquery) {
            WCDBJoinWithNaturalLeftOuterJoin(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func naturalLeftJoin(_ subqueryConvertible: TableOrSubqueryConvertible) -> Join {
        let subquery = subqueryConvertible.asTableOrSubquery()
        withExtendedLifetime(subquery) {
            WCDBJoinWithNaturalLeftJoin(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func naturalInnerJoin(_ subqueryConvertible: TableOrSubqueryConvertible) -> Join {
        let subquery = subqueryConvertible.asTableOrSubquery()
        withExtendedLifetime(subquery) {
            WCDBJoinWithNaturalInnerJoin(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func naturalCrossJoin(_ subqueryConvertible: TableOrSubqueryConvertible) -> Join {
        let subquery = subqueryConvertible.asTableOrSubquery()
        withExtendedLifetime(subquery) {
            WCDBJoinWithNaturalCrossJoin(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func on(_ expressionConvertible: ExpressionConvertible) -> Join {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBJoinConfigOn(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func using(_ columnConvertibleList: ColumnConvertible...) -> Join {
        return using(columnConvertibleList)
    }

    @discardableResult
    public func using(_ columnConvertibleList: [ColumnConvertible]) -> Join {
        let columns = columnConvertibleList.map { $0.asColumn() }
        withExtendedLifetime(columns) {
            let cppColumns = $0.map { $0.cppObj }
            cppColumns.withUnsafeBufferPointer {
                WCDBJoinConfigUsingColumn(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }
}

extension Join: TableOrSubqueryConvertible {
    public func asTableOrSubquery() -> TableOrSubquery {
        return TableOrSubquery(with: self)
    }
}
