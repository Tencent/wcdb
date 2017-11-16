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

func sample_transaction_main(baseDirectory: String) {
    print("Sample-transaction Begin")
    
    let className = String(describing: SampleTransaction.self)
    let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent(className).path
    let database = Database(withPath: path)
    let tableName = className
    database.close(onClosed: { 
        try? database.removeFiles()
    })
        
    //Run auto inner create table/indexes transaction
    do {
        try database.create(table: tableName, of: SampleTransaction.self)
    }catch let error {
        print("auto inner create table transaction error: \(error)")
    }
    
    //Run blocked transaction
    do {
        try database.run(transaction: { () -> () in  
            let object = SampleTransaction()
            try database.insert(objects: object, into: tableName)
        })
    }catch let error {
        print("blocked transaction error: \(error)")
    }
    
    //Run threaded transaction
    do {
        //[begin], [commit], [rollback] and all interfaces inside this transaction should run in same thread
        try database.begin()
        let object = SampleTransaction()
        do {
            try database.insert(objects: object, into: tableName)
            try database.commit()
        }catch let error {
            try? database.rollback()
            throw error 
        }
    }catch let error {
        print("threaded transaction error: \(error)")
    }
    
    //SampleTransaction using [Transaction]
    do {
        //You can do a transaction in different threads using [Transaction].
        //But it's better to run serially, or an inner thread mutex will guarantee this.
        let transaction = try database.transaction()
        try transaction.begin()
        DispatchQueue(label: "other thread").async {
            let object = SampleTransaction()
            do {
                try transaction.insert(objects: object, into: tableName)
                try transaction.commit()
            }catch let error {
                try? transaction.rollback()
            }
        }
    }catch let error {
        print("transaction error: \(error)")
    }
    
    //Run auto inner batch insert transaction
    do {
        //insertion will automatically run an inner transaction if it's not already in a transaction and objects is more than 1 objects
        var objects: [SampleTransaction] = []
        let object1 = SampleTransaction()
        objects.append(object1)
        let object2 = SampleTransaction()
        objects.append(object2)
        try database.insert(objects: objects, into: tableName)
    }catch let error {
        print("inner batch insert transaction error: \(error)")
    }

    print("Sample-transaction End")
}
