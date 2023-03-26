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

import XCTest
#if TEST_WCDB_SWIFT
import WCDBSwift
#else
import WCDB
#endif

class IndexedColumnTests: BaseTestCase {

    func testIndexedColumn() {
        // Give
        let column1 = Column(named: "column1")
        let expression1 = column1.abs()

        // Then
        WINQAssertEqual(IndexedColumn(with: column1), "column1")

        WINQAssertEqual(IndexedColumn(with: column1).order(.ascending), "column1 ASC")

        WINQAssertEqual(IndexedColumn(with: expression1), "ABS(column1)")

        WINQAssertEqual(IndexedColumn(with: expression1).order(.ascending), "ABS(column1) ASC")

        WINQAssertEqual(IndexedColumn(with: expression1).collate("BINARY"), "ABS(column1) COLLATE BINARY")
    }
}
