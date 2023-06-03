//
// Created by qiuwenchen on 2022/6/9.
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

public final class FrameSpec: Identifier<CPPFrameSpec> {

    public init() {
        super.init(with: WCDBFrameSpecCreate())
    }

    @discardableResult
    public func range() -> FrameSpec {
        WCDBFrameSpecConfigRange(cppObj)
        return self
    }

    @discardableResult
    public func rows() -> FrameSpec {
        WCDBFrameSpecConfigRows(cppObj)
        return self
    }

    @discardableResult
    public func unboundedPreceding() -> FrameSpec {
        WCDBFrameSpecConfigUnboundedPreceding(cppObj)
        return self
    }

    @discardableResult
    public func preceding(_ expressionConverable: ExpressionConvertible) -> FrameSpec {
        let expression = expressionConverable.asExpression()
        withExtendedLifetime(expression) {
            WCDBFrameSpecConfigPreceding(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func currentRow() -> FrameSpec {
        WCDBFrameSpecConfigCurrentRow(cppObj)
        return self
    }

    @discardableResult
    public func betweenUnboundedPreceding() -> FrameSpec {
        WCDBFrameSpecConfigBetweenUnboundedPreceding(cppObj)
        return self
    }

    @discardableResult
    public func betweenPreceding(_ expressionConverable: ExpressionConvertible) -> FrameSpec {
        let expression = expressionConverable.asExpression()
        withExtendedLifetime(expression) {
            WCDBFrameSpecConfigBetweenPreceding(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func betweenCurrentRow() -> FrameSpec {
        WCDBFrameSpecConfigBetweenCurrentRow(cppObj)
        return self
    }

    @discardableResult
    public func betweenFollowing(_ expressionConverable: ExpressionConvertible) -> FrameSpec {
        let expression = expressionConverable.asExpression()
        withExtendedLifetime(expression) {
            WCDBFrameSpecConfigBetweenFollowing(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func andCurrentRow() -> FrameSpec {
        WCDBFrameSpecConfigAndCurrentRow(cppObj)
        return self
    }

    @discardableResult
    public func andPreceding(_ expressionConverable: ExpressionConvertible) -> FrameSpec {
        let expression = expressionConverable.asExpression()
        withExtendedLifetime(expression) {
            WCDBFrameSpecConfigAndPreceding(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func andUnboundedFollowing() -> FrameSpec {
        WCDBFrameSpecConfigAndUnboundedFollowing(cppObj)
        return self
    }

    @discardableResult
    public func andFollowing(_ expressionConverable: ExpressionConvertible) -> FrameSpec {
        let expression = expressionConverable.asExpression()
        withExtendedLifetime(expression) {
            WCDBFrameSpecConfigAndFollowing(cppObj, $0.cppObj)
        }
        return self
    }
}
