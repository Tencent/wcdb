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

public final class OrderingTerm: Identifier<CPPOrderingTerm> {
    public init(with expressionConvertible: ExpressionConvertible) {
        let expression = expressionConvertible.asExpression()
        let cppOrderingTerm = withExtendedLifetime(expression) {
            WCDBOrderingTermCreate(expression.cppObj)
        }
        super.init(with: cppOrderingTerm)
    }

    @discardableResult
    public func order(_ order: Order?) -> OrderingTerm {
        if let order = order {
            WCDBOrderingTermConfigOrder(cppObj, order.cValue)
        }
        return self
    }

    @discardableResult
    public func collate(_ collation: String) -> OrderingTerm {
        WCDBOrderingTermConfigCollation(cppObj, collation.cString)
        return self
    }

    public static func ascendingRowid() -> OrderingTerm {
        return Column.rowid().order(.ascending)
    }

    public static func descendingRowid() -> OrderingTerm {
        return Column.rowid().order(.descending)
    }
}

extension OrderingTerm: OrderingTermConvertible {
    public func asOrder() -> OrderingTerm {
        return self
    }
}
