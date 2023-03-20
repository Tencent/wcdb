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
import WCDB

class ColumnDefTests: BaseTestCase {

    func generateColumnDef() -> ColumnDef {
        return ColumnDef(with: Column(named: "testColumnDef"), and: .integer32)
    }

    func testColumnDef() {
        WINQAssertEqual(
            ColumnDef(with: Column(named: "testColumnDef"), and: .float),
            "testColumnDef REAL"
        )

        WINQAssertEqual(
            ColumnDef(with: Column(named: "testColumnDef"), and: .text),
            "testColumnDef TEXT"
        )

        WINQAssertEqual(
            ColumnDef(with: Column(named: "testColumnDef"), and: .BLOB),
            "testColumnDef BLOB"
        )

        WINQAssertEqual(
            ColumnDef(with: Column(named: "testColumnDef"), and: .null),
            "testColumnDef NULL"
        )

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
            generateColumnDef().makePrimary(onConflict: .Ignore),
            "testColumnDef INTEGER PRIMARY KEY ON CONFLICT IGNORE"
        )

        WINQAssertEqual(
            generateColumnDef().makeNotNull(),
            "testColumnDef INTEGER NOT NULL"
        )

        WINQAssertEqual(
            generateColumnDef().makeUnique(),
            "testColumnDef INTEGER UNIQUE"
        )

        WINQAssertEqual(
            generateColumnDef().makeForeignKey(ForeignKey().references(with: "testColumnDefTable")),
            "testColumnDef INTEGER REFERENCES testColumnDefTable"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: Expression(nil)),
            "testColumnDef INTEGER DEFAULT NULL"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: -1),
            "testColumnDef INTEGER DEFAULT -1"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: 171798691840),
            "testColumnDef INTEGER DEFAULT 171798691840"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: false),
            "testColumnDef INTEGER DEFAULT 0"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: "SampleText"),
            "testColumnDef INTEGER DEFAULT 'SampleText'"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: 1.2),
            "testColumnDef INTEGER DEFAULT 1.2"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: "SampleText".data(using: .ascii)!),
            "testColumnDef INTEGER DEFAULT 'SampleText'"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: 1),
            "testColumnDef INTEGER DEFAULT 1"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: LiteralValue.currentDate()),
            "testColumnDef INTEGER DEFAULT CURRENT_DATE"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: LiteralValue.currentTime()),
            "testColumnDef INTEGER DEFAULT CURRENT_TIME"
        )

        WINQAssertEqual(
            generateColumnDef().makeDefault(to: LiteralValue.currentTimestamp()),
            "testColumnDef INTEGER DEFAULT CURRENT_TIMESTAMP")
    }
}
