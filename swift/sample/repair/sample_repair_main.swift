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

func sample_repair_main(baseDirectory: String) {
    print("Sample-repair Begin")
    
    let className = String(describing: SampleRepair.self)
    let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent(className).path
    let recoverPath = URL(fileURLWithPath: path).appendingPathComponent("_recover").path 
    
    let tableName = className
    let databaseCipher = "databaseCipher".data(using: .ascii)!
    let backupCipher = "backupCipher".data(using: .ascii)!
    
    let database = Database(withPath: path)
    database.setCipher(key: databaseCipher)
    database.close { 
        try? database.removeFiles()
    }
    
    //prepare
    do {
        try database.create(table: tableName, of: SampleRepair.self)
        for i in 0..<10 {
            let object = SampleRepair()
            object.identifier = i
            object.content = String(i)
            try database.insert(objects: object, into: tableName)
        }
        let count = try database.getValue(on: SampleRepair.anyProperty.count(), from: tableName) as! Int64
        print("The count of objects before: \(count)")
    }catch let error {
        print("prepare error: \(error)")
    }
    
    //backup
    do {
        try database.backup(withKey: backupCipher)
    }catch let error {
        print("backup error: \(error)")
    }
    
    //get page size
    var pageSize: Int32!
    //get page size from origin database.
    //Since page size never change unless you can call "PRAGMA page_size=NewPageSize" to set it. You have no need to get the page size like this. Instead, you can hardcode it.
    do {
        let coreStatement = try database.prepare(StatementPragma().pragma(.pageSize))
        try coreStatement.step()
        pageSize = coreStatement.value(atIndex: 0)
    }catch let error {
        print("getpagesize error: \(error)")
    }
    
    //do something
    //corrupt
    do {
        database.close(onClosed: { 
            let file = fopen(path, "rb+")
            let zeroPage = UnsafeMutablePointer<Int8>.allocate(capacity: 100)
            memset(zeroPage, 0, 100)
            fwrite(zeroPage, 100, 1, file)
            fclose(file)
        })
        
        let value = try? database.getValue(on: SampleRepair.anyProperty.count(), from: tableName)
        let count = value != nil ? value as! Int64 : 0
        print("The count of objects corrupted: \(count)")
    }
    
    //repair
    do {
        //Since recovering is a long time operation, you'd better call it in sub-thread.
        //let queue = DispatchQueue.global(qos: .background)
        let recover = Database(withPath: recoverPath)
        recover.close(onClosed: { 
            try? recover.removeFiles()
        })
        database.close(onClosed: { 
            try recover.recover(fromPath: path, withPageSize: pageSize, databaseKey: databaseCipher, backupKey: backupCipher)
        })
        let count = try database.getValue(on: SampleRepair.anyProperty.count(), from: tableName) as! Int64
        print("The count of objects repaired: \(count)")
    }catch let error {
        print("repair error: \(error)")
    }
    
    print("Sample-repair End")
}
