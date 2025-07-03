//
// Created by qiuwenchen on 2022/5/27.
//

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

public final class BindParameter: Identifier<CPPBindParameter> {

    public convenience init(_ num: Int32) {
        self.init(with: WCDBBindparameterCreateQuestionSignType(num))
    }

    public convenience init(_ name: String) {
        self.init(with: WCDBBindparameterCreateColonSignType(name.cString))
    }

    public static func at(named name: String) -> BindParameter {
        BindParameter(with: WCDBBindparameterCreateAtSignType(name.cString))
    }

    public static func colon(named name: String) -> BindParameter {
        BindParameter(with: WCDBBindparameterCreateColonSignType(name.cString))
    }

    public static func dollar(named name: String) -> BindParameter {
        BindParameter(with: WCDBBindparameterCreateDollarSignType(name.cString))
    }

    public static func bindParameters(_ num: Int) -> [BindParameter] {
        var bindParameters: [BindParameter] = []
        for i in 1...num {
            bindParameters.append(BindParameter(Int32(i)))
        }
        return bindParameters
    }

    nonisolated(unsafe) public static let def: BindParameter = BindParameter(0) // ?
    nonisolated(unsafe) public static let _1: BindParameter = BindParameter(1)  // ?1
    nonisolated(unsafe) public static let _2: BindParameter = BindParameter(2)  // ?2
    nonisolated(unsafe) public static let _3: BindParameter = BindParameter(3)  // ?3
    nonisolated(unsafe) public static let _4: BindParameter = BindParameter(4)  // ?4
    nonisolated(unsafe) public static let _5: BindParameter = BindParameter(5)  // ?5
    nonisolated(unsafe) public static let _6: BindParameter = BindParameter(6)  // ?6
    nonisolated(unsafe) public static let _7: BindParameter = BindParameter(7)  // ?7
    nonisolated(unsafe) public static let _8: BindParameter = BindParameter(8)  // ?8
    nonisolated(unsafe) public static let _9: BindParameter = BindParameter(9)  // ?9
    nonisolated(unsafe) public static let _10: BindParameter = BindParameter(10)// ?10
}

extension BindParameter: ExpressionConvertible {
    public func asExpression() -> Expression {
        Expression(with: self)
    }
}
