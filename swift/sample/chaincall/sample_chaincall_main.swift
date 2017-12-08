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

func sample_chailcall_main(baseDirectory: String) {
    print("Sample-chaincall Begin")
    
    let className = String(describing: SampleChainCall.self)
    let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent(className).path
    let database = Database(withPath: path)
    let tableName = className
    database.close(onClosed: { 
        try? database.removeFiles()
    })
        
    do{
        try database.create(table: tableName, of: SampleChainCall.self)
    }catch let error {
        print("create table error: \(error)")
    }
        
    //Insert objects
    do {
        var objects: [SampleChainCall] = []
        let object1 = SampleChainCall()
        object1.intValue = 1
        object1.stringValue = "Insert objects"
        objects.append(object1)
        let object2 = SampleChainCall()
        object2.intValue = 2
        object2.stringValue = "Insert objects"
        objects.append(object2)
        let insert = try database.prepareInsert(of: SampleChainCall.self, intoTable: tableName)
        try insert.execute(with: objects)            
    }catch let error {
        print("insert objects error: \(error)")
    }
    
    //Select objects
    do {
        let select = try database.prepareSelect(on: (SampleChainCall.Properties.intValue).max().as((SampleChainCall.Properties.intValue)), fromTable: tableName).where(SampleChainCall.Properties.intValue==1).group(by: SampleChainCall.Properties.intValue).limit(3)
        let objects: [SampleChainCall] = try select.allObjects()
    }catch let error {
        print("select objects error: \(error)")
    }
    
    //Select rows
    do {
        let rowSelect = try database.prepareRowSelect(on: SampleChainCall.Properties.all, fromTable: tableName).where(SampleChainCall.Properties.intValue==1).limit(3)
        let needBreak = true
        while let row = try rowSelect.nextRow() {
            if needBreak {
                break
            } 
        }
    }catch let error {
        print("select rows error: \(error)")
    }
    
    //Update by object
    do {
        let update = try database.prepareUpdate(table: tableName, on: SampleChainCall.Properties.stringValue)
        let object = SampleChainCall()
        object.stringValue = "Update by object"
        try update.execute(with: object)
    }catch let error {
        print("update by object error: \(error)")
    }
    
    //Delete
    do {
        let delete = try database.prepareDelete(fromTable: tableName)
        try delete.execute()
    }catch let error {
        print("delete error: \(error)")
    }
        
    print("Sample-chaincall End")
}

