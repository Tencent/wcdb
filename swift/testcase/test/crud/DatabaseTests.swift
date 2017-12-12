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

class DatabaseTests: BaseTestCase {

    func testTag() {
        let database: Database = Database(withFileURL: self.recommendedPath)
        database.tag = self.recommendTag
        let taggedDatabase: Database? = WCDBAssertNoThrowReturned(try Database(with: self.recommendTag))
        XCTAssertNotNil(taggedDatabase)
        XCTAssertEqual(taggedDatabase!.path, database.path)

        //Non-existent tag
        XCTAssertThrowsError(try Database(with: self.recommendTag - 1))
    }

    func testFailedToCreateDirectory() {
        let blocks = self.recommendedPath
        fileManager.createFile(atPath: blocks.path, contents: Data(), attributes: nil)
        let path = URL(fileURLWithPath: blocks.path).appendingPathComponent("newPath")
        let database = Database(withFileURL: path)
        XCTAssertFalse(database.canOpen)
    }
}
