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

class BaselineReadBenchmark: BaseBenchmark {
    
    override func setUp() {
        super.setUp()

        XCTAssertNoThrow(try database.create(table: BenchmarkObject.name, of: BenchmarkObject.self))
        
        var objects = [BenchmarkObject]()
        for i in 0..<config.readCount {
            objects.append(BenchmarkObject(withKey: i, and: randomGenerator.data(withLength: config.valueLength)))
        }
        XCTAssertNoThrow(try database.insert(objects: objects, intoTable: BenchmarkObject.name))
        database.close()
        
        XCTAssertTrue(database.canOpen)
    }

    func testBaselineRead() {
        var results: [BenchmarkObject]? = nil
        self.measure {
            results = try? database.getObjects(fromTable: BenchmarkObject.name)
        }
        XCTAssertEqual(results?.count, config.readCount)
    }
}
