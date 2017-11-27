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

class WCDBTestCase: XCTestCase {
    static let baseDirectory = FileManager.default.temporaryDirectory.appendingPathComponent(String(describing: WCDBTestCase.self))
    
    lazy var recommendedDirectory = URL(fileURLWithPath: self.className, relativeTo: WCDBTestCase.baseDirectory) .path
    
    lazy var recommendedPath = URL(fileURLWithPath: String(self.name.hash), relativeTo: URL(fileURLWithPath: recommendedDirectory)).path 
    
    let fileManager = FileManager.default
    
    override func setUp() {
        super.setUp()
        if fileManager.fileExists(atPath: recommendedDirectory) {
            XCTAssertNoThrow(try fileManager.removeItem(atPath: recommendedDirectory))
        }
        XCTAssertNoThrow(try fileManager.createDirectory(atPath: recommendedDirectory, withIntermediateDirectories: true, attributes: nil))
    }    
}
