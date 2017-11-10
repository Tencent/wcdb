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

func sample_convenient_main(baseDirectory: String) {
    print("Sample-convenient Begin")
    
    do {
        let className = String(describing: SampleConvenient.self)
        let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent(className).path
        let database = Database(withPath: path)
        let tableName = className
        database.close(onClosed: { 
            try database.removeFiles()
        })
        
        try database.create(table: tableName, of: SampleConvenient.self)
        
        //Insert one object
        do {
            let object = SampleConvenient()
            object.intValue = 1
            object.stringValue = "Insert onr object"
            try database.insert(objects: object, into: tableName)
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
            try database.insert(objects: objects, into: tableName)
        }
        
        //Insert or replace objects 
        do {
           let object = SampleConvenient()
            object.isAutoIncrement = true
            object.stringValue = "Insert auto increment"
            try database.insert(objects: object, into: tableName)
        }
        
        //Update by objects
        do {
            let object = SampleConvenient()
            object.stringValue = "Update by object"
            try database.update(table: tableName, on: \SampleConvenient.stringValue, with: object)
        }
        
        //Update with condition/order/offset/limit
        do {
            let object = SampleConvenient()
            object.stringValue = "Update with condition/order/offset/limit"
            try database.update(table: tableName, on: \SampleConvenient.stringValue, with: object, where: \SampleConvenient.intValue > 0)
        }
        
        //Select one object
        do {
            let object = try database.getObject(of: SampleConvenient.self, from: tableName)
        }
        
        //Select objects
        do {
            let objects = try database.getObjects(of: SampleConvenient.self, from: tableName)
        }
        
        //Select objects with condition/order/offset/limit
        do {
            let objects = try database.getObjects(of: SampleConvenient.self, from: tableName, orderBy: [(\SampleChainCall.intValue).asOrder(by: .Ascending)], limit: 1, offset: 2)
        }
        
        //Select part of objects
        do {
            let objects: [SampleConvenient] = try database.getObjects(on: \SampleConvenient.stringValue, from: tableName)
        }
        
        //Select column
        do {
            let column: OneColumn = try database.getColumn(on: \SampleConvenient.stringValue, from: tableName)
            for string in column {
                //do sth
            }
        }
        
        //Select row
        do {
            let row: OneRow? = try database.getRow(on: \SampleConvenient.intValue, \SampleConvenient.stringValue, from: tableName)
            let intValue = row?[0]
            let stringValue = row?[1]
        }
        
        //Select one value
        do {
            let count = try database.getValue(on: SampleConvenient.anyProperty.count(), from: tableName)
        }
        
        //Select aggregation
        do {
            let row: OneRow? = try database.getRow(on: (\SampleConvenient.intValue).avg(), (\SampleConvenient.stringValue).count(), from: tableName)
        }
        
        //Select distinct result
        do {
            let distinctCount = try database.getDistinctValue(on: \SampleConvenient.intValue, from: tableName)
        }
        
        //Delete
        do {
            try database.delete(from: tableName)
        }
        
        //Delete with condition/order/offset/limit
        do {
            try database.delete(from: tableName, where: (\SampleConvenient.intValue).in(1, 2, 3))
        }
    }catch let error {
        print(error)
    }
    
    print("Sample-convenient End")
}
