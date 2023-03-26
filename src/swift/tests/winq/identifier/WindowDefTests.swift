//
// Created by qiuwenchen on 2022/6/10.
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
#if TEST_WCDB_SWIFT
import WCDBSwift
#else
import WCDB
#endif

class WindowDefTests: BaseTestCase {

    func testWindowDef() {

        WINQAssertEqual(WindowDef(), "")

        WINQAssertEqual(WindowDef().partition(1, 2), "(PARTITION BY 1, 2)")

        WINQAssertEqual(WindowDef().order(1, 2), "(ORDER BY 1, 2)")

        WINQAssertEqual(WindowDef().frameSpec(FrameSpec().range().unboundedPreceding()), "(RANGE UNBOUNDED PRECEDING)")

        WINQAssertEqual(WindowDef().partition(1, 2).order(1, 2).frameSpec(FrameSpec().range().unboundedPreceding()), "(PARTITION BY 1, 2 ORDER BY 1, 2 RANGE UNBOUNDED PRECEDING)")

    }
}
