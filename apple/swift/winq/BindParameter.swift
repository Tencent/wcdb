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

// MARK: Syntax
public class BindParameter: SQL {
    public enum Identifier: SyntaxIdentifier {
        case QuestionSign(n: Int)
        case ColonSign(name: String)
        case AtSign(name: String)
        case DollarSign(name: String)

        public var type: SyntaxType {
            return .BindParameter
        }
        public var description: String {
            switch self {
            case .QuestionSign(let n):
                return "?\(n)"
            case .ColonSign(let name):
                return ":\(name)"
            case .AtSign(let name):
                return "@\(name)"
            case .DollarSign(let name):
                return "$\(name)"
            }
        }
    }

    public var syntax: Identifier

    required init(syntax: Identifier) {
        self.syntax = syntax
    }
}

// MARK: SQL
public extension BindParameter {
    convenience init(_ n: Int) {
        self.init(syntax: .QuestionSign(n: n))
    }

    convenience init(_ name: String) {
        self.init(syntax: .ColonSign(name: name))
    }

    static func bindParameters(_ count: Int) -> Array<BindParameter> {
        var bps = Array<BindParameter>()
        for i in 0..<count {
            bps.append(BindParameter(i))
        }
        return bps
    }

    static func at(_ name: String) -> BindParameter {
        return BindParameter(syntax: .AtSign(name: name))
    }

    static func colon(_ name: String) -> BindParameter {
        return BindParameter(syntax: .ColonSign(name: name))
    }

    static func dollar(_ name: String) -> BindParameter {
        return BindParameter(syntax: .DollarSign(name: name))
    }
}
