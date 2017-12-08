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

func sample_file_main(baseDirectory: String) {
    print("Sample-file Begin")
    
    let className = String(describing: SampleFile.self)
    let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent(className).path
    let otherDirectory = URL(fileURLWithPath: baseDirectory).appendingPathComponent("moved").path 
    let tableName = className
    
    
    let database = Database(withPath: path)
    database.close(onClosed: { 
        try? database.removeFiles()
    })
    
    do {
        try database.create(table: tableName, of: SampleFile.self)
    }catch let error {
        print("create table error: \(error)")
    }
    
    //Get file size
    do {
        var fileSize: UInt64!
        database.close(onClosed: { 
            fileSize = try database.getFilesSize()
        })
        print("file size: \(fileSize)")
    }catch let error {
        print("file size error: \(error)")
    }
    
    //Move files
    do {
        database.close(onClosed: { 
            try database.moveFiles(toDirectory: otherDirectory)
        })
    }catch let error {
        print("move file error: \(error)")
    }
    
    //Get paths
    do {
        let paths = database.paths
        print("paths: \(paths)")
    }
    
    print("Sample-file End")
}

