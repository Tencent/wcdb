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

class CRUDTestCase: DatabaseTestCase {

    let preInsertedObjects: [TestObject] = {
        let object1 = TestObject()
        object1.variable1 = 1
        object1.variable2 = "object1"
        let object2 = TestObject()
        object2.variable1 = 2
        object2.variable2 = "object2"
        return [object1, object2]
    }()

    override func setUp() {
        super.setUp()

        XCTAssertNoThrow(try database.create(table: TestObject.name, of: TestObject.self))

        XCTAssertNoThrow(try database.insert(preInsertedObjects, intoTable: TestObject.name))
    }
}
