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

func sample_core_main(baseDirectory: String) {
    print("Sample-core Begin")
    
    do {
        let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent("core").path
        let database = Database(withPath: path)
        database.close(onClosed: { 
            try database.removeFiles()
        })
        
        //set config
        try database.setConfig(named: "demo", with: { (handle) in
            try handle.exec(StatementPragma().pragma(.secureDelete, to: true))
        }, orderBy: 1)
        
        //run unwrapped SQL
        try database.exec(StatementPragma().pragma(.caseSensitiveLike, to: true))
        
        //get value from unwrapped SQL
        do {
            let coreStatement = try database.prepare(StatementPragma().pragma(.cacheSize))
            try coreStatement.step()
            let value: Int32 = coreStatement.value(atIndex: 0)
            print("Cache size \(value)")
        }
        
        //complex statement 1
        //SAVEPOINT mySavepoint
        try database.exec(StatementSavepoint().savepoint("mySavepoint"))
        //...
        //RELEASE SAVEPOINT mySavepoint
        try database.exec(StatementRelease().release(savepoint: "mySavepoint"))
        
        //complex statement 2
        //EXPLAIN CREATE TABLE message(localID INTEGER PRIMARY KEY ASC, content TEXT);
        do {
            print("Explain:")
            //Column def for localID
            let localIDColumnDef = ColumnDef(with: \SampleCore.localID, and: .Integer32).makePrimary(orderBy: .Ascending)
            
            //Column def for content
            let contentColumnDef = ColumnDef(with: \SampleCore.content, and: .Text)
            
            //Combine table name and column def list into create table statement
            let statementCreate = StatementCreateTable().create(table: "message", with: localIDColumnDef, contentColumnDef)
            
            let coreStatement = try database.prepare(StatementExplain().explain(statementCreate))
            try coreStatement.step()
            for i in 0..<coreStatement.columnCount() {
                let columnName = coreStatement.columnName(atIndex: i)
                let value: OneValue = coreStatement.value(atIndex: i)
                print("\(columnName): \(value ?? "")")
            }
        }
        
        //complex statement 3
        //SELECT message.content, message_ext.createTime FROM message LEFT OUTER JOIN message_ext ON message.localID=message_ext.localID
        do {
            //Column result list
            let resultList = [(\SampleCore.content).in(table: "message"), (\SampleCoreExt.createTime).in(table: "message_ext")]
            
            //Join clause
            let joinClause = JoinClause(withTable: "message").join("message_ext", with: .LeftOuter).on((\SampleCore.localID).in(table: "message") == (\SampleCoreExt.localID).in(table: "message_ext"))
            
            let statementSelect = StatementSelect().select(resultList).from(joinClause)
            let coreStatement = try database.prepare(statementSelect)
            while try coreStatement.step() {
                for i in 0..<coreStatement.columnCount() {
                    let columnName = coreStatement.columnName(atIndex: i)
                    let value: OneValue = coreStatement.value(atIndex: i)
                    print("\(columnName): \(value)")
                }
            }
        }
    }catch let error {
        print(error)
    }
    print("Sample-core End")
}
