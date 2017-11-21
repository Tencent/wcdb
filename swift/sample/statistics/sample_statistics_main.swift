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

import Foundation
import WCDB

func sample_statistics_main(baseDirectory: String) {
    print("Sample-statistics Begin")
    
    let className = String(describing: SampleStatistics.self)
    let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent(className).path
    let tableName = className
    
    let database = Database(withPath: path)
    database.close { 
        try? database.removeFiles()
    }
    
    //trace
    //You should register trace before all db operation.
    do {
        Database.globalTrace(ofPerformance: { (tag, sqls, cost) in
            print("Tag: \(tag ?? 0)")
            for (sql, count) in sqls {
                print("SQL: \(sql) Count: \(count)")
            }
            print("Total cost \(cost) nanoseconds")
        })
    }
    
    //SQL
    do {
        Database.globalTrace(ofSQL: { (sql) in
            print("SQL: \(sql)")
        })
    }
    
    //error
    do {
        Database.globalTrace(ofErrorReport: { (error) in
            print("Error: \(error)")
        })
    }
    
    do {
        try database.create(table: tableName, of: SampleStatistics.self)
    }catch let error {
        print("create table error: \(error)")
    }
    
    //trace for insertion
    do {
        let object = SampleStatistics()
        object.intValue = 100
        try database.insert(objects: object, into: tableName)
    }catch let error {
        print("insert error: \(error)")
    }
    
    //trace for transaction
    do {
        var objects: [SampleStatistics] = []
        for i in 0..<10 {
            let object = SampleStatistics()
            object.intValue = i
            objects.append(object)
        }
        try database.insert(objects: objects, into: tableName)
        
        try database.run(transaction: { 
            let results: [SampleStatistics] = try database.getObjects(from: tableName)
            for object in results {
                object.intValue = -(object.intValue ?? 0)
            }
            try database.insert(objects: results, into: tableName)
        })
    }catch let error{
        print("transaction error:\(error)")
    }
    
    print("Sample-statistics End")
}
