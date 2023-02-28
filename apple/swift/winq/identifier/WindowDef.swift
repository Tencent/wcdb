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
public final class WindowDef: Identifier<CPPWindowDef> {

    public init() {
        super.init(with: WCDBWindowDefCreate())
    }

    @discardableResult
    public func partition(_ expressionConvertibleList: ExpressionConvertible...) -> WindowDef {
        self.partition(expressionConvertibleList)
    }

    @discardableResult
    public func partition(_ expressionConvertibleList: [ExpressionConvertible]) -> WindowDef {
        let expressions = expressionConvertibleList.map { $0.asExpression() }
        withExtendedLifetime(expressions) {
            let partitions = $0.map { $0.cppObj }
            partitions.withUnsafeBufferPointer { cppPartitions in
                WCDBWindowDefConfigPartition(cppObj, cppPartitions.baseAddress, Int32(cppPartitions.count))
            }
        }
        return self
    }

    @discardableResult
    public func order(_ orderConvertibleList: OrderingTermConvertible...) -> WindowDef {
        self.order(orderConvertibleList)
    }

    @discardableResult
    public func order(_ orderConvertibleList: [OrderingTermConvertible]) -> WindowDef {
        let orderingTerms = orderConvertibleList.map { $0.asOrder() }
        withExtendedLifetime(orderingTerms) {
            let cppOrderingTerms = $0.map { $0.cppObj }
            cppOrderingTerms.withUnsafeBufferPointer {
                WCDBWindowDefConfigOrder(cppObj, $0.baseAddress, Int32($0.count))
            }
        }
        return self
    }

    @discardableResult
    public func frameSpec(_ frameSpec: FrameSpec) -> WindowDef {
        withExtendedLifetime(frameSpec) {
            WCDBWindowDefConfigFrameSpec(cppObj, $0.cppObj)
        }
        return self
    }
}
