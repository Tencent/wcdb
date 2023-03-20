//
// Created by 陈秋文 on 2022/6/9.
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
import WCDB

class FrameSpecTests: BaseTestCase {

    func testFrameSpec() {
        WINQAssertEqual(FrameSpec().range().unboundedPreceding(), "RANGE UNBOUNDED PRECEDING")

        WINQAssertEqual(FrameSpec().rows().unboundedPreceding(), "ROWS UNBOUNDED PRECEDING")

        WINQAssertEqual(FrameSpec().range().preceding(1), "RANGE 1 PRECEDING")

        WINQAssertEqual(FrameSpec().range().currentRow(), "RANGE CURRENT ROW")

        WINQAssertEqual(FrameSpec().range().betweenUnboundedPreceding().andPreceding(2), "RANGE BETWEEN UNBOUNDED PRECEDING AND 2 PRECEDING")

        WINQAssertEqual(FrameSpec().range().betweenPreceding(1).andPreceding(2), "RANGE BETWEEN 1 PRECEDING AND 2 PRECEDING")

        WINQAssertEqual(FrameSpec().range().betweenCurrentRow().andPreceding(2), "RANGE BETWEEN CURRENT ROW AND 2 PRECEDING")

        WINQAssertEqual(FrameSpec().range().betweenFollowing(1).andPreceding(2), "RANGE BETWEEN 1 FOLLOWING AND 2 PRECEDING")

        WINQAssertEqual(FrameSpec().range().betweenUnboundedPreceding().andCurrentRow(), "RANGE BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW")

        WINQAssertEqual(FrameSpec().range().betweenUnboundedPreceding().andFollowing(2), "RANGE BETWEEN UNBOUNDED PRECEDING AND 2 FOLLOWING")

        WINQAssertEqual(FrameSpec().range().betweenUnboundedPreceding().andUnboundedFollowing(), "RANGE BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING")
    }
}
