//
// Created by 陈秋文 on 2023/8/13.
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

class HandleTests: DatabaseTestCase {

    func testCancellationSignal() {
        XCTAssertNoThrow(try database.create(table: TestObject.name, of: TestObject.self))
        var objects: [TestObject] = []
        for i in 0...10000 {
            let object = TestObject()
            object.variable1 = i
            object.variable2 = "\(i)"
            objects.append(object)
        }

        XCTAssertNoThrow(try database.insert(objects, intoTable: TestObject.name))

        var hasTestInterrupt = false
        database.trace { error in
            guard error.level == .Error else {
                return
            }
            XCTAssertEqual(error.code, .Interrupt)
            hasTestInterrupt = true
        }

        let signal = Handle.CancellatiionSignal()
        let group = DispatchGroup()
        DispatchQueue(label: "test").async(group: group) {
            let handle = try? self.database.getHandle()
            guard let handle = handle else {
                return
            }
            handle.attach(cancellationSignal: signal)
            let objects: [TestObject]? = try? handle.getObjects(on: TestObject.Properties.all, fromTable: TestObject.name)
            XCTAssertNil(objects)
            handle.detachCancellationSignal()
        }
        usleep(10000)
        signal.cancel()
        group.wait()
        XCTAssertTrue(hasTestInterrupt)
        self.database.trace(ofError: nil)
    }
}
