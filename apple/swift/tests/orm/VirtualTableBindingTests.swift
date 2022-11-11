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

class VirtualTableBindingTests: BaseTestCase {

    final class FTS3TestObject: TableCodable, Named {
        var id: Int = 0
        var content: String = ""
        enum CodingKeys: String, CodingTableKey {
            typealias Root = FTS3TestObject
            case id
            case content
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindVirtualTable(withModule: .FTS3, and: BuiltinTokenizer.OneOrBinary, BuiltinTokenizer.Parameter.NeedSymbol)
                BindColumnConstraint(id, isNotIndexed: true)
            }
        }
    }

    func testFTS3VirtualBinding() {
        ORMVirtualTableBindingAssertEqual(
            FTS3TestObject.self,
            """
            CREATE VIRTUAL TABLE IF NOT EXISTS main.FTS3TestObject USING fts3\
            (tokenize = WCDB need_symbol, id INTEGER, content TEXT, notindexed=id)
            """
        )
    }

    final class FTS5TestObject: TableCodable, Named {
        var id: Int = 0
        var content: String = ""
        enum CodingKeys: String, CodingTableKey {
            typealias Root = FTS5TestObject
            case id
            case content
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindVirtualTable(withModule: .FTS5, and: BuiltinTokenizer.Verbatim, BuiltinTokenizer.Parameter.SkipStemming, BuiltinTokenizer.Parameter.SimplifyChinese)
                BindColumnConstraint(id, isNotIndexed: true)
            }
        }
    }

    func testFTS5VirtualBinding() {
        ORMVirtualTableBindingAssertEqual(
            FTS5TestObject.self,
            """
            CREATE VIRTUAL TABLE IF NOT EXISTS main.FTS5TestObject USING fts5\
            (tokenize = 'wcdb_verbatim skip_stemming chinese_traditional_to_simplified', id UNINDEXED, content)
            """
        )
    }
}
