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

class VirtualTableBindingTests: BaseTestCase {

    class BaselineTestObject: TableCodable, Named {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = BaselineTestObject
            case variable
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var virtualTableBinding: VirtualTableBinding? {
                return VirtualTableBinding(withModule: "fts3",
                                           and: ModuleArgument(left: "left", right: "right"),
                                                ModuleArgument(with: .WCDB))
            }
            static var tableConstraintBindings: [TableConstraintBinding.Name: TableConstraintBinding]? {
                return [BaselineTestObject.name+"Constraint": MultiPrimaryBinding(indexesBy: variable)]
            }
        }
    }

    func testVirtualBinding() {
        ORMVirtualTableBindingAssertEqual(
            BaselineTestObject.self,
            """
            CREATE VIRTUAL TABLE IF NOT EXISTS BaselineTestObject USING fts3\
            (variable INTEGER, CONSTRAINT BaselineTestObjectConstraint PRIMARY KEY(variable), left=right, tokenize=WCDB)
            """
        )
    }
}
