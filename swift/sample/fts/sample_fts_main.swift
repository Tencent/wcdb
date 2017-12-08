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

func sample_fts_main(baseDirectory: String) {
    print("Sample-fts Begin")
    
    let classNameOrigin = String(describing: SampleFTSOrigin.self)
    let pathOrigin = URL(fileURLWithPath: baseDirectory).appendingPathComponent(classNameOrigin).path
    let tableNameOrigin = classNameOrigin
    
    let databaseOrigin = Database(withPath: pathOrigin)
    databaseOrigin.close(onClosed: { 
        try? databaseOrigin.removeFiles()
    })
        
    do {
        try databaseOrigin.create(table: tableNameOrigin, of: SampleFTSOrigin.self)
    }catch let error {
        print("creat table error: \(error)")
    }
        
    //prepare
    do {
        do {
            //English
            let object = SampleFTSOrigin()
            object.name = "English"
            object.content = "This is sample content"
            object.isAutoIncrement = true
            try databaseOrigin.insert(objects: object, intoTable: tableNameOrigin)
        }
        do {
            //Number
            let object = SampleFTSOrigin()
            object.name = "Number"
            object.content = "123"
            object.isAutoIncrement = true
            try databaseOrigin.insert(objects: object, intoTable: tableNameOrigin)
        }
        do {
            //Chinese
            let object = SampleFTSOrigin()
            object.name = "Chinese"
            object.content = "这是测试内容"
            object.isAutoIncrement = true
            try databaseOrigin.insert(objects: object, intoTable: tableNameOrigin)
        }
    }catch let error {
        print("prepare error: \(error)")
    }
        
    let classNameFTS = String(describing: SampleFTSData.self)
    let pathFTS = URL(fileURLWithPath: baseDirectory).appendingPathComponent(classNameFTS).path
    let tableNameFTS = classNameFTS
    
    let databaseFTS = Database(withPath: pathFTS)
    databaseFTS.close(onClosed: { 
        try databaseFTS.removeFiles()
    })
    
    databaseFTS.setTokenizes(.WCDB)
        
    do {
        try databaseFTS.create(virtualTable: tableNameFTS, of: SampleFTSData.self)
    }catch let error {
        print("create virtual table error: \(error)")
    }
        
    //Build Full-Text-Search Index
    do {
        let objects: [SampleFTSOrigin] = try databaseOrigin.getObjects(fromTable: tableNameOrigin)
        var ftsDataArray: [SampleFTSData] = []
        for object in objects {
            let ftsData = SampleFTSData()
            ftsData.name = object.name
            ftsData.content = object.content
            ftsDataArray.append(ftsData)
        }
        try databaseFTS.insert(objects: ftsDataArray, intoTable: tableNameFTS)
    }catch let error {
        print("build full-text-search index error: \(error)")
    }
    
    //Full-Text-Search by `match`
    do {
        let ftsDataArray: [SampleFTSData] = try databaseFTS.getObjects(fromTable: tableNameFTS, where: Column(named: tableNameFTS).match("Eng*"))
        for ftsData in ftsDataArray {
            print("Match name:\(ftsData.name ?? "") content:\(ftsData.content ?? "")")
        }
    }catch let error {
        print("full-text-search error: \(error)")
    }
    
    //Full-Text-Search info by `match`
    //See http://www.sqlite.org/fts3.html#snippet for further information
    do {
        let tableProperty = Column(named: tableNameFTS) 
        let row: FundamentalRow = (try databaseFTS.getRow(on: tableProperty.snippet(), tableProperty.offsets(), fromTable: tableNameFTS, where: tableProperty.match("12*")))!
        
        print("Snippet: \(String(describing: row[0])) Offset: \(String(describing: row[1]))")
    }catch let error {
        print("full-text-search info error: \(error)")
    }

    print("Sample-fts End")
}
