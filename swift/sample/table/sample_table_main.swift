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

func sample_table_main(baseDirectory: String) {
    print("Sample-table Begin")
    
    let className = String(describing: SampleTable.self)
    let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent(className).path
    let tableName = className
    
    let database = Database(withPath: path)
    database.close(onClosed: { 
        try? database.removeFiles()
    })
        
    //Create table and indexes from WCTTableCoding class
    do {
        try database.create(table: tableName, of: SampleTable.self)
    }catch let error {
        print("creat table error: \(error)")
    }
    
    //Drop table
    do {
        try database.drop(table: tableName)
    }catch let error {
        print("drop table error: \(error)")
    }
        
    //Create table from column def
    do {
        try database.create(table: tableName, with: (\SampleTable.intValue).asDef(with: .Integer32), (\SampleTable.stringValue).asDef(with: .Text) )
    }catch let error {
        print("create table from column def error: \(error)")
    }

    //Create index from column index
    let indexSubfix = "_index"
    let indexName = tableName + indexSubfix
    do {
        try database.create(index: indexName, with: (\SampleTable.stringValue).asIndex(orderBy: .Ascending) , forTable: tableName)
    }catch let error {
        print("create index error: \(error)")
    }
    
    //Drop index
    do {
        try database.drop(index: indexName)
    }catch let error {
        print("drop index error: \(error)")
    }
    
    //Add column
    do {
        let tableName2 = tableName + "2"
        try database.create(table: tableName2, with: (\SampleTable.intValue).asDef(with: .Integer32) )
        try database.addColumn(with: (\SampleTable.stringValue).asDef(with: .Text), forTable: tableName2)
    }catch let error {
        print("add column error: \(error)")
    }
    
    do {
        //Get table
        let table: Table<SampleTable>? = try database.getTable(named: tableName)
        //Insert/Update/Select/Delete using table
        let object = SampleTable()
        try table?.insert(objects: object)
    }catch let error {
        print("get table error: \(error)")
    }
    
    print("Sample-table End")
}
