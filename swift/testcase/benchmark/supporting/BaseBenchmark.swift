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

class BaseBenchmark: BaseTestCase {
    
    var config = Config.default
    var database: Database!
    lazy var randomGenerator = RandomData(withSeed: config.randomSeed)
    
    var objects: [BenchmarkObject] = []
    
    override func setUp() {
        super.setUp()
        
        database = Database(withFileURL: self.recommendedPath)
    }
    
    func getTableName(withIndex index: Int = 0) -> String {
        return "\(BenchmarkObject.name)\(index)"
    }
    
    func setUpWithPreCreateTable(count: Int = 1) {
        XCTAssertNoThrow(try database.run(transaction: {
            for i in 0..<count {
                try database.create(table: getTableName(withIndex: i), of: BenchmarkObject.self)
            }
        }))
    }
    
    func setUpWithPreInsertObjects(count: Int, intoIndexedTable index: Int = 0) {
        var objects = [BenchmarkObject]()
        for i in 0..<count {
            objects.append(BenchmarkObject(withKey: i, and: randomGenerator.data(withLength: config.valueLength)))
        }
        XCTAssertNoThrow(try database.insert(objects: objects, intoTable: getTableName(withIndex: index)))
    }
    
    func setUpWithPreCreateObject(count: Int) {
        for i in 0..<count {
            objects.append(BenchmarkObject(withKey: i, and: randomGenerator.data(withLength: config.valueLength)))
        }
    }
    
    func setUpDatabaseCache() {
        XCTAssertTrue(database.canOpen)
    }
    
    func tearDownDatabaseCache() {
        database.close()
    }
    
    func tearDownDatabase() {
        database.close { 
            XCTAssertNoThrow(try self.database.removeFiles())
        }
    }
    
    func measure(onSetUp setUpBlock: () -> Swift.Void, for block: () -> Swift.Void, checkCorrectness correctnessBlock: () -> Swift.Void) {
        measureMetrics(type(of: self).defaultPerformanceMetrics, automaticallyStartMeasuring: false) { 
            setUpBlock()
            
            startMeasuring()
            
            block()
            
            stopMeasuring()
            
            correctnessBlock()
        }
    }
}
