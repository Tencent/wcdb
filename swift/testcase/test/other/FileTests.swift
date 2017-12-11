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

class FileTests: BaseTestCase {
    var database: Database!

    override func setUp() {
        super.setUp()
        database = Database(withFileURL: self.recommendedPath)
    }

    func testPaths() {
        //Give
        let path = self.recommendedPath.path
        let expertedPaths = [path, path+"-wal", path+"-shm", path+"-journal", path+"-backup"]
        //Then
        XCTAssertEqual(database.paths.sorted(), expertedPaths.sorted())
    }

    func testRemoveFiles() {
        //Give
        for path in database.paths {
            if fileManager.fileExists(atPath: path) {
                XCTAssertNoThrow(try fileManager.removeItem(atPath: path))
            }
            XCTAssertNoThrow(fileManager.createFile(atPath: path, contents: nil, attributes: nil))
        }
        //When
        database.close {
            XCTAssertNoThrow(try self.database.removeFiles())
        }
        //Then
        for path in database.paths {
            XCTAssertFalse(fileManager.fileExists(atPath: path))
        }
    }

    func testMoveFiles() {
        //Give
        let extraFile = self.recommendedDirectory.urlByAppendingPathComponent("extraFile").path
        let paths = database.paths + [extraFile]
        for path in paths {
            XCTAssertNoThrow(fileManager.createFile(atPath: path, contents: nil, attributes: nil))
        }

        let newDirectory = self.recommendedDirectory.urlByAppendingPathComponent("newDirectory").path

        let newPaths = paths.map { (path) -> String in
            let fileName = URL(fileURLWithPath: path).lastPathComponent
            return URL(fileURLWithPath: newDirectory).appendingPathComponent(fileName).path
        }

        for path in newPaths {
            if fileManager.fileExists(atPath: path) {
                XCTAssertNoThrow(try fileManager.removeItem(atPath: path))
            }
        }

        //When
        XCTAssertNoThrow(try database.moveFiles(toDirectory: newDirectory, withExtraFiles: extraFile))
        //Then
        for path in newPaths {
            XCTAssertTrue(fileManager.fileExists(atPath: path))
        }

        //Clear
        XCTAssertNoThrow(try fileManager.removeItem(atPath: newDirectory))
    }

    func testGetFilesSize() {
        //Give
        let data = "testGetFilesSize".data(using: .ascii)!
        let expectedFilesSize = data.count * database.paths.count
        for path in database.paths {
            if fileManager.fileExists(atPath: path) {
                XCTAssertNoThrow(try fileManager.removeItem(atPath: path))
            }
            XCTAssertNoThrow(fileManager.createFile(atPath: path, contents: data, attributes: nil))
        }
        //Then
        database.close {
            let filesSize = WCDBAssertNoThrowReturned(try self.database.getFilesSize()) ?? 0
            XCTAssertEqual(filesSize, UInt64(expectedFilesSize))
        }
    }
}
