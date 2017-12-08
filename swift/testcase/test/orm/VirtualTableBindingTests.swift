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

    class BaselineTestObject: TableCodable {
        let variable: Int = 0 
        required init() {}
        enum CodingKeys: String, CodingTableKey {
            typealias Root = BaselineTestObject
            case variable
            static let __objectRelationalMapping = TableBinding(CodingKeys.self)
            static var __virtualTableBinding: VirtualTableBinding? {
                return VirtualTableBinding(with: .fts3, and: ModuleArgument(left: "left", right: "right"), ModuleArgument(with: .WCDB))
            }
        }
    }

    func testVirtualBinding() {
        ORMVirtualTableBindingAssertEqual(BaselineTestObject.self, "CREATE VIRTUAL TABLE IF NOT EXISTS BaselineTestObject USING fts3(variable INTEGER, left=right, tokenize=WCDB)")
    }
}
