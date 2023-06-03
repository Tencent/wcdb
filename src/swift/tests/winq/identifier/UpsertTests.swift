//
// Created by qiuwenchen on 2022/6/12.
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

class UpsertTests: BaseTestCase {

    func testUpsert() {
        let column = Column(named: "testColumn1")
        let columns = [
            Column(named: "testColumn2"),
            Column(named: "testColumn3")
        ]
        let indexedColumn1 = Column(named: "testColumn1")
        let indexedColumn2 = Column(named: "testColumn2")
        let condition = 1
        let value1 = 2
        let value2 = 3
        let updateCondition = 4

        WINQAssertEqual(Upsert().onConflict().doNothing(), "ON CONFLICT DO NOTHING")

        WINQAssertEqual(Upsert().onConflict().indexed(by: indexedColumn1).doNothing(), "ON CONFLICT(testColumn1) DO NOTHING")

        WINQAssertEqual(Upsert().onConflict().indexed(by: indexedColumn1, indexedColumn2).doNothing(), "ON CONFLICT(testColumn1, testColumn2) DO NOTHING")

        WINQAssertEqual(Upsert().onConflict().indexed(by: indexedColumn1).where(condition).doNothing(), "ON CONFLICT(testColumn1) WHERE 1 DO NOTHING")

        WINQAssertEqual(Upsert().onConflict().doUpdate().set(column).to(value1), "ON CONFLICT DO UPDATE SET testColumn1 = 2")

        WINQAssertEqual(Upsert().onConflict().doUpdate().set(column).to(value1).set(columns).to(value2), "ON CONFLICT DO UPDATE SET testColumn1 = 2, (testColumn2, testColumn3) = 3")

        WINQAssertEqual(Upsert().onConflict().doUpdate().set(column).to(value1).where(updateCondition), "ON CONFLICT DO UPDATE SET testColumn1 = 2 WHERE 4")
    }
}
