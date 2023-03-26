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

class TableOrSubqueryTests: BaseTestCase {

    func testSubquery() {
        WINQAssertEqual(TableOrSubquery(withTable: "testTable"), "testTable")

        WINQAssertEqual(TableOrSubquery(withTable: "testTable").of(schema: "testSchema"), "testSchema.testTable")

        WINQAssertEqual(TableOrSubquery(withTable: "testTable").indexed(by: "testIndex"), "testTable INDEXED BY testIndex")

        WINQAssertEqual(TableOrSubquery(withTable: "testTable").notIndexed(), "testTable NOT INDEXED")

        WINQAssertEqual(TableOrSubquery(withTable: "testTable").as(alias: "testAliasTable"), "testTable AS testAliasTable")

        WINQAssertEqual(TableOrSubquery.function("testFunction").of(schema: "testSchema"), "testSchema.testFunction()")

        WINQAssertEqual(TableOrSubquery.function("testFunction").arguments(1, 2), "testFunction(1, 2)")

        WINQAssertEqual(TableOrSubquery.function("testFunction").as(alias: "testAliasTable"), "testFunction() AS testAliasTable")

        WINQAssertEqual(TableOrSubquery(with: "testTable1", "testTable2"), "(testTable1, testTable2)")

        WINQAssertEqual(TableOrSubquery(with: "testTable1", "testTable2"), "(testTable1, testTable2)")

        WINQAssertEqual(TableOrSubquery.master(), "sqlite_master")

        // Give
        let joinClause = Join(with: "testTable")
        let statementSelect = StatementSelect().select(1).from("testTable")

        // Then
        WINQAssertEqual(TableOrSubquery(with: joinClause).asTableOrSubquery(), "(testTable)")

        WINQAssertEqual(TableOrSubquery(with: joinClause), "(testTable)")

        WINQAssertEqual(TableOrSubquery(with: statementSelect), "(SELECT 1 FROM testTable)")

        WINQAssertEqual(TableOrSubquery(with: statementSelect).as(alias: "testAliasTable"), "(SELECT 1 FROM testTable) AS testAliasTable")
    }
}
