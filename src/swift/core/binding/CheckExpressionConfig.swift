//
// Created by qiuwenchen on 2022/11/10.
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

public final class CheckExpressionConfig: TableConfiguration {
    private let constraints: [TableConstraint]

    public func config(with tableBinding: TableBindingBase) {
        for constraint in constraints {
            withExtendedLifetime(constraint) {
                WCDBBindingAddTableConstraint(tableBinding.cppBinding, $0.cppObj)
            }
        }
    }

    public init(@CheckExpressionBuilder _ constraintBuilder: () -> [TableConstraint]) {
        constraints = constraintBuilder()
    }

    @resultBuilder
    public struct CheckExpressionBuilder {
        public static func buildBlock() -> [TableConstraint] {
            return []
        }
        public static func buildBlock(_ enums: Expression...) -> [TableConstraint] {
            return enums.map { exp in
                TableConstraint().check(exp)
            }
        }
    }
}
