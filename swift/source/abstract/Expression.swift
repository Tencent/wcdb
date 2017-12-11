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

public final class Expression: Describable {
    public private(set) var description: String

    public static let bindingParameter = Expression(withRaw: "?")

    public init(with columnConvertible: ColumnConvertible) {
        description = columnConvertible.asColumn().description
    }

    public init(with literalValueConvertible: LiteralValueConvertible) {
        description = literalValueConvertible.asLiteralValue().description
    }

    public init(with statementSelect: StatementSelect) {
        description = statementSelect.description
    }

    init(withRaw raw: String) {
        description = raw
    }
}

extension Expression: ExpressibleByNilLiteral {
    public convenience init(nilLiteral value: ()) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByBooleanLiteral {
    public convenience init(booleanLiteral value: Bool) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByIntegerLiteral {
    public convenience init(integerLiteral value: Int) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByFloatLiteral {
    public convenience init(floatLiteral value: Double) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByStringLiteral {
    public convenience init(stringLiteral value: String) {
        self.init(with: LiteralValue(value))
    }
}

extension Expression: ExpressionOperable {
    public func asExpression() -> Expression {
        return self
    }
}
