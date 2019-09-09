//
// Created by sanhuazhang on 2019/05/04
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
import WCDBSwift

class ColumnTests: XCTestCase {

    func test_get_type() {
        XCTAssertEqual(Column("testColumn").syntax.type, SyntaxType.Column)
    }

    func test_column() {
        let testingSQL = Column("testColumn")
        let testingTypes = [ SyntaxType.Column ]

//    auto testingSQL = WCDB::Column(@"testColumn");
//
//    auto testingTypes = { WCDB::SQL::Type::Column };
//    TestCaseAssertIterateEqual(testingSQL, testingTypes);
//    TestCaseAssertSQLEqual(testingSQL, @"testColumn");
    }

    func test_rowid() {
//    TestCaseAssertSQLEqual(WCDB::Column::rowid(), @"rowid");
    }

}
