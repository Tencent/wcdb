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

class AdvanceCRUDTests: CRUDTestCase {

    func testSubthreadCheckpoint() {
        //Give
        let fileManager = self.fileManager
        let walFile = database.path+"-wal"
        let object = CRUDObject()
        object.variable2 = "testSubthreadCheckpoint"
        object.isAutoIncrement = true
        //When
        for _ in 0..<1000 {
            XCTAssertNoThrow(try database.insert(objects: object, intoTable: CRUDObject.name))
        }
        var expectedSize: Int64!
        do {
            let walSize = WCDBAssertNoThrowReturned(try fileManager.attributesOfItem(atPath: walFile)[.size])
            XCTAssertNotNil(walSize)
            let typedWalSize = walSize! as? Int64
            XCTAssertNotNil(typedWalSize)
            XCTAssertGreaterThan(typedWalSize!, 0)
            expectedSize = typedWalSize!
        }
        sleep(5)
        //Then
        do {
            let walSize = WCDBAssertNoThrowReturned(try fileManager.attributesOfItem(atPath: walFile)[.size])
            XCTAssertNotNil(walSize)
            let typedWalSize = walSize! as? Int64
            XCTAssertNotNil(typedWalSize)
            XCTAssertEqual(typedWalSize!, expectedSize)
        }
    }
}
