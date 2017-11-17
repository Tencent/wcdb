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

func sample_orm_main(baseDirectory: String) {
    print("Sample-ORM Begin")
    
    let clses: [CodableTable.Type] = [ SampleORM.self, SampleORMIndex.self, SampleORMColumnConstraint.self, SampleORMTableConstraint.self ]
    for cls in clses {
        let className = String(describing: cls)
        let filename = className
        let tableName = className
        
        let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent(filename).path
        let database = Database(withPath: path)
        database.close(onClosed: { 
            try? database.removeFiles()
        })
        
        do {
            try database.create(table: tableName, of: cls)
        }catch let error {
            print("create table error: \(error)")
        }
        
        do {
            let schemas: [Master] = try database.getObjects(on: \Master.name, \Master.sql, from: Master.tableName)
            schemas.forEach({ (table) in
                print("SQL of \(table.name ?? ""): \(table.sql ?? "" )")
            })
        }catch let error {
            print("get schemas error: \(error)")
        }
    }
    
    print("Sample-ORM End")
}
