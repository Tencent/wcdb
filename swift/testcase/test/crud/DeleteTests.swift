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

class DeleteTests: CRUDTestCase {
    
    var delete: Delete!
    
    override func setUp() {
        super.setUp()
        
        let optionalDelete = WCDBAssertNoThrowReturned(try database.prepareDelete(fromTable: CRUDObject.name), whenFailed: nil)
        XCTAssertNotNil(optionalDelete)
        delete = optionalDelete!
    }

    func testDelete() {
        //When
        XCTAssertNoThrow(try delete.execute())
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: CRUDObject.name))
        XCTAssertEqual(results.count, 0)
    }
    
    func testConditionalDelete() {
        //When
        XCTAssertNoThrow(try delete.where(CRUDObject.Properties.variable1 == 2).execute())
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: CRUDObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }
    
    func testOrderedDelete() {
        //Give
        let order = (CRUDObject.Properties.variable1).asOrder(by: .Descending)
        //When
        XCTAssertNoThrow(try delete.order(by: order).limit(1).execute())
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: CRUDObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }
    
    func testLimitedDelete() {
        //When
        XCTAssertNoThrow(try delete.limit(1).execute())
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: CRUDObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[1])
    }
    
    func testOffsetDelete() {
        //When
        XCTAssertNoThrow(try delete.limit(1, offset: 1).execute())
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: CRUDObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }    
    
    func testDeleteChanges() {
        //When
        XCTAssertNoThrow(try delete.execute())
        //Then
        XCTAssertEqual(delete.changes, preInsertedObjects.count)
    }
}
