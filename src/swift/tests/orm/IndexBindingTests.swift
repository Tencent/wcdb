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

class IndexBindingTests: ORMTestCase {

    final class BaselineTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = BaselineTestObject
            case variable
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindIndex(variable, namedWith: "_index")
            }
        }
    }

    final class UniqueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = UniqueTestObject
            case variable
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindIndex(variable, namedWith: "_index", isUnique: true)
            }
        }
    }

    final class MultiIndexesTestObject: TableCodable {
        var variable1: Int = 0
        var variable2: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = MultiIndexesTestObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindIndex(variable1, variable2, namedWith: "_index")
            }
        }
    }

    func testIndexBinding() {
        ORMIndexBindingAssertEqual(
            BaselineTestObject.self,
            "CREATE INDEX IF NOT EXISTS BaselineTestObject_index ON BaselineTestObject(variable)"
        )

        ORMIndexBindingAssertEqual(
            UniqueTestObject.self,
            "CREATE UNIQUE INDEX IF NOT EXISTS UniqueTestObject_index ON UniqueTestObject(variable)"
        )

        ORMIndexBindingAssertEqual(
            MultiIndexesTestObject.self,
            """
            CREATE INDEX IF NOT EXISTS MultiIndexesTestObject_index \
            ON MultiIndexesTestObject(variable1, variable2)
            """)
    }
}
