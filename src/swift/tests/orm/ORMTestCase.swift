//
// Created by qiuwenchen on 2022/12/3.
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
#if TEST_WCDB_SWIFT
import WCDBSwift
#else
import WCDB
#endif

class ORMTestCase: DatabaseTestCase {

    func ORMConstraintBindingAssertEqual<TableCodableObject: TableCodable>(
        _ expression1: @autoclosure () -> TableCodableObject.Type,
        _ expression2: @autoclosure () -> String,
        file: StaticString = #file,
        line: UInt = #line) {
        let type = expression1()
        let className = String(describing: type)
        var description1: String?
        database.trace { _, _, sql in
            if sql.hasPrefix("CREATE TABLE") {
                description1 = sql
            }
        }
        WCDBAssertNoThrowReturned(try database.create(table: className, of: TableCodableObject.self))
        XCTAssertNotNil(description1)
        let description2 = expression2()
        XCTAssertEqual(description1!, description2, file: file, line: line)
    }

    func ORMIndexBindingAssertEqual<TableCodableObject: TableCodable>(
        _ expression1: @autoclosure () -> TableCodableObject.Type,
        _ expression2: @autoclosure () -> String,
        file: StaticString = #file,
        line: UInt = #line) {
        let type = expression1()
        let className = String(describing: type)
        var description1: String?
        database.trace { _, _, sql in
            if (sql.hasPrefix("CREATE INDEX") ||
                sql.hasPrefix("CREATE UNIQUE INDEX")) &&
                description1 == nil {
                description1 = sql
            }
        }
        WCDBAssertNoThrowReturned(try database.create(table: className, of: TableCodableObject.self))
        XCTAssertNotNil(description1)
        let description2 = expression2()
        XCTAssertEqual(description1!, description2, file: file, line: line)
    }

    func ORMVirtualTableBindingAssertEqual<TableCodableObject: TableCodable>(
        _ expression1: @autoclosure () -> TableCodableObject.Type,
        _ expression2: @autoclosure () -> String,
        file: StaticString = #file,
        line: UInt = #line) {
        let type = expression1()
        let className = String(describing: type)
        var description1: String?
        database.trace { _, _, sql in
            if sql.hasPrefix("CREATE VIRTUAL TABLE") {
                description1 = sql
            }
        }
        WCDBAssertNoThrowReturned(try database.create(virtualTable: className, of: TableCodableObject.self))
        XCTAssertNotNil(description1)
        let description2 = expression2()
        XCTAssertEqual(description1, description2, file: file, line: line)
    }
}
