//
// Created by 陈秋文 on 2022/7/13.
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

import XCTest
import WCDB

class StatementCreateViewTests: BaseTestCase {

    func testCreateView() {
        // Give
        let column1 = Column(named: "column1")
        let column2 = Column(named: "column2")

        let statementSelect = StatementSelect().select(column1).from("testTable")

        WINQAssertEqual(
            StatementCreateView().create(view: "testView").with(columns: column1, column2).as(statementSelect),
            "CREATE VIEW main.testView(column1, column2) AS SELECT column1 FROM main.testTable"
        )

        WINQAssertEqual(
            StatementCreateView().create(view: "testView", isTemp: true).with(columns: column1, column2).as(statementSelect),
            "CREATE TEMP VIEW testView(column1, column2) AS SELECT column1 FROM main.testTable"
        )

        WINQAssertEqual(
            StatementCreateView().create(view: "testView").of(schema: "testSchema").with(columns: column1, column2).as(statementSelect),
            "CREATE VIEW testSchema.testView(column1, column2) AS SELECT column1 FROM main.testTable"
        )

        WINQAssertEqual(
            StatementCreateView().create(view: "testView").ifNotExists().with(columns: column1, column2).as(statementSelect),
            "CREATE VIEW IF NOT EXISTS main.testView(column1, column2) AS SELECT column1 FROM main.testTable"
        )
    }
}
