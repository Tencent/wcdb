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
import WCDBSwift

class ColumnDefTests: BaseTestCase {

    func generateColumnDef() -> ColumnDef {
        return ColumnDef(with: Column(named: "testColumnDef"), and: .integer32)
    }

    func testColumnDef() {
        WINQAssertEqual(
            generateColumnDef().makePrimary(),
            "testColumnDef INTEGER PRIMARY KEY"
        )

        WINQAssertEqual(
            generateColumnDef().makePrimary(orderBy: .ascending),
            "testColumnDef INTEGER PRIMARY KEY ASC"
        )

        WINQAssertEqual(
            generateColumnDef().makePrimary(isAutoIncrement: true),
            "testColumnDef INTEGER PRIMARY KEY AUTOINCREMENT"
        )

        WINQAssertEqual(
            generateColumnDef().makePrimary(onConflict: .ignore),
            "testColumnDef INTEGER PRIMARY KEY ON CONFLICT IGNORE"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: .null),
            "testColumnDef INTEGER DEFAULT NULL"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: .int32(-1)),
            "testColumnDef INTEGER DEFAULT -1"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: .int64(171798691840)),
            "testColumnDef INTEGER DEFAULT 171798691840"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: .bool(false)),
            "testColumnDef INTEGER DEFAULT false"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: .text("SampleText")),
            "testColumnDef INTEGER DEFAULT 'SampleText'"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: .float(0.1)),
            "testColumnDef INTEGER DEFAULT 0.1"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: .BLOB("SampleText".data(using: .ascii)!)),
            "testColumnDef INTEGER DEFAULT 'SampleText'"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: .expression(1)),
            "testColumnDef INTEGER DEFAULT 1"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: .currentDate),
            "testColumnDef INTEGER DEFAULT CURRENT_DATE"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: .currentTime),
            "testColumnDef INTEGER DEFAULT CURRENT_TIME"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: .currentTimestamp),
            "testColumnDef INTEGER DEFAULT CURRENT_TIMESTAMP")

        WINQAssertEqual(
            generateColumnDef().makeNotNull(),
            "testColumnDef INTEGER NOT NULL"
        )

        WINQAssertEqual(
            generateColumnDef().makeUnique(),
            "testColumnDef INTEGER UNIQUE"
        )

        WINQAssertEqual(
            generateColumnDef().makeForeignKey(ForeignKey(withForeignTable: "testColumnDefTable")),
            "testColumnDef INTEGER REFERENCES testColumnDefTable"
        )
    }
}
