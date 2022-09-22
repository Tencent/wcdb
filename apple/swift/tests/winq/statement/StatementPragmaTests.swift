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

class StatementPragmaTests: BaseTestCase {

    func testStatementPragma() {

        WINQAssertEqual(StatementPragma().pragma(.pageSize), "PRAGMA main.page_size")

        WINQAssertEqual(StatementPragma().pragma(.secureDelete).to(true), "PRAGMA main.secure_delete = 1")

        WINQAssertEqual(StatementPragma().schema("testSchema").pragma(.secureDelete).to(true), "PRAGMA testSchema.secure_delete = 1")

        WINQAssertEqual(StatementPragma().schema("testSchema").pragma(.tableInfo).with("testTable"), "PRAGMA testSchema.table_info('testTable')")
    }
}
