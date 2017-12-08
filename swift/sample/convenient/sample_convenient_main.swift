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
import WCDBSwift

func sample_convenient_main(baseDirectory: String) {
    print("Sample-convenient Begin")
    
    let className = String(describing: SampleConvenient.self)
    let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent(className).path
    let database = Database(withPath: path)
    let tableName = className
    database.close(onClosed: { 
        try? database.removeFiles()
    })
        
    do{
        try database.create(table: tableName, of: SampleConvenient.self)
    }catch let error {
        print("create table error: \(error)")
    }
        
    //Insert one object
    do {
        let object = SampleConvenient()
        object.intValue = 1
        object.stringValue = "Insert onr object"
        try database.insert(objects: object, intoTable: tableName)
    }catch let error {
        print("insert one object error: \(error)")
    }
    
    //Insert objects
    do {
        var objects: [SampleConvenient] = []
        let object1 = SampleConvenient()
        object1.intValue = 2
        object1.stringValue = "Insert objects"
        objects.append(object1)
        let object2 = SampleConvenient()
        object2.intValue = 3
        object2.stringValue = "Insert objects"
        objects.append(object2)
        try database.insert(objects: objects, intoTable: tableName)
    }catch let error {
        print("insert objects error: \(error)")
    }
    
    //Insert or replace objects 
    do {
       let object = SampleConvenient()
        object.isAutoIncrement = true
        object.stringValue = "Insert auto increment"
        try database.insert(objects: object, intoTable: tableName)
    }catch let error {
        print("insert or replace objects error: \(error)")
    }
    
    //Update by objects
    do {
        let object = SampleConvenient()
        object.stringValue = "Update by object"
        try database.update(table: tableName, on: SampleConvenient.Properties.stringValue, with: object)
    }catch let error {
        print("update by objects \(error)")
    }
    
    //Update with condition/order/offset/limit
    do {
        let object = SampleConvenient()
        object.stringValue = "Update with condition/order/offset/limit"
        try database.update(table: tableName, on: SampleConvenient.Properties.stringValue, with: object, where: SampleConvenient.Properties.intValue > 0)
    }catch let error {
        print("Update with condition/order/offset/limit \(error)")
    }
    
    //Select one object
    do {
        let object: SampleConvenient? = try database.getObject(fromTable: tableName)
    }catch let error {
        print("select one object error: \(error)")
    }
    
    //Select objects
    do {
        let objects: [SampleConvenient] = try database.getObjects(fromTable: tableName)
    }catch let error {
        print("select objects error: \(error)")
    }
    
    //Select objects with condition/order/offset/limit
    do {
        let objects: [SampleConvenient] = try database.getObjects(fromTable: tableName, orderBy: [(SampleChainCall.Properties.intValue).asOrder(by: .Ascending)], limit: 1, offset: 2)
    }catch let error {
        print("select objects error: \(error)")
    }
    
    //Select part of objects
    do {
        let objects: [SampleConvenient] = try database.getObjects(on: SampleConvenient.Properties.stringValue, fromTable: tableName)
    }catch let error {
        print("select part of objects error: \(error)")
    }
    
    //Select column
    do {
        let column: FundamentalColumn = try database.getColumn(on: SampleConvenient.Properties.stringValue, fromTable: tableName)
        for string in column {
            //do sth
        }
    }catch let error {
        print("select column error: \(error)")
    }
    
    //Select row
    do {
        let row: FundamentalRow? = try database.getRow(on: SampleConvenient.Properties.intValue, SampleConvenient.Properties.stringValue, fromTable: tableName)
        let intValue = row?[0]
        let stringValue = row?[1]
    }catch let error {
        print("select row error: \(error)")
    }
    
    //Select one value
    do {
        let count = try database.getValue(on: SampleConvenient.Properties.any.count(), fromTable: tableName)
    }catch let error {
        print("select one value error: \(error)")
    }
    
    //Select aggregation
    do {
        let row: FundamentalRow? = try database.getRow(on: (SampleConvenient.Properties.intValue).avg(), (SampleConvenient.Properties.stringValue).count(), fromTable: tableName)
    }catch let error {
        print("select aggregation error: \(error)")
    }
    
    //Select distinct result
    do {
        let distinctCount = try database.getDistinctValue(on: SampleConvenient.Properties.intValue, fromTable: tableName)
    }catch let error {
        print("select distinct result error: \(error)")
    }
    
    //Delete
    do {
        try database.delete(fromTable: tableName)
    }catch let error {
        print("delete error: \(error)")
    }
    
    //Delete with condition/order/offset/limit
    do {
        try database.delete(fromTable: tableName, where: (SampleConvenient.Properties.intValue).in(1, 2, 3))
    }catch let error {
        print("delete with condition/order/offset/limit error: \(error)")
    }
    
    print("Sample-convenient End")
}
