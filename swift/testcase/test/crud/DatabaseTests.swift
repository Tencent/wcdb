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
        //Give
        let tag = self.recommendTag
        let path: String = self.recommendedPath.path
        var taggedDatabase: Database? = nil
        do {
            let database: Database = Database(withPath: path)
            database.tag = tag
            taggedDatabase = WCDBAssertNoThrowReturned(try Database(withExistingTag: tag))
        }
        XCTAssertNotNil(taggedDatabase)
        XCTAssertEqual(taggedDatabase!.tag, tag)
        XCTAssertEqual(taggedDatabase!.path, path)

        //Non-existent tag
        XCTAssertThrowsError(try Database(withExistingTag: tag - 1))
    }

    func testMultiDatabase() {
        //Give
        let path: String = self.recommendedPath.path
        let database: Database = Database(withPath: path)
        let database2: Database = Database(withPath: path)
        database.tag = self.recommendTag
        XCTAssertNotNil(database2.tag)
        XCTAssertEqual(database2.tag, database.tag)
    }

    func testFailedToCreateDirectory() {
        let blocks = self.recommendedPath
        fileManager.createFile(atPath: blocks.path, contents: Data(), attributes: nil)
        let path = URL(fileURLWithPath: blocks.path).appendingPathComponent("newPath")
        let database = Database(withFileURL: path)
        XCTAssertFalse(database.canOpen)
    }

    func testFailedToOpenDatabase() {
        let path = self.recommendedPath
        XCTAssertNoThrow(try fileManager.createDirectory(at: path, withIntermediateDirectories: true, attributes: nil))
        let database = Database(withFileURL: path)
        XCTAssertFalse(database.canOpen)
    }

    func testPurgeFreeHandles() {
        //Give
        let database: Database = Database(withFileURL: self.recommendedPath)
        //When
        XCTAssertTrue(database.canOpen)
        XCTAssertTrue(database.isOpened)
        database.purge()
        //Then
        XCTAssertFalse(database.isOpened)

        //When
        XCTAssertTrue(database.canOpen)
        XCTAssertTrue(database.isOpened)
        Database.purge()
        //Then
        XCTAssertFalse(database.isOpened)
    }
}
