//
// Created by qiuwenchen on 2022/5/31.
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

public final class RaiseFunction: Identifier<CPPRaiseFunction> {

    public init() {
        super.init(with: WCDBRaiseFunctionCreate())
    }

    @discardableResult
    public func ignore() -> RaiseFunction {
        WCDBRaiseFunctionSetAction(cppObj, WCDBSyntaxRaiseAction_Ignore, nil)
        return self
    }

    @discardableResult
    public func rollback(with errorMessage: String) -> RaiseFunction {
        WCDBRaiseFunctionSetAction(cppObj, WCDBSyntaxRaiseAction_Rollback, errorMessage.cString)
        return self
    }

    @discardableResult
    public func abort(with errorMessage: String) -> RaiseFunction {
        WCDBRaiseFunctionSetAction(cppObj, WCDBSyntaxRaiseAction_Abort, errorMessage.cString)
        return self
    }

    @discardableResult
    public func fail(with errorMessage: String) -> RaiseFunction {
        WCDBRaiseFunctionSetAction(cppObj, WCDBSyntaxRaiseAction_Fail, errorMessage.cString)
        return self
    }
}

extension RaiseFunction: ExpressionConvertible {
    public func asExpression() -> Expression {
        return Expression(with: self)
    }
}
