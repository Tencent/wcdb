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

func sample_chailcall_main(baseDirectory: String) {
    print("Sample-chaincall Begin")
    
    do {
        let className = String(describing: SampleChainCall.self)
        let path = URL(fileURLWithPath: baseDirectory).appendingPathComponent(className).path
        let database = Database(withPath: path)
        let tableName = className
        database.close(onClosed: { 
            try database.removeFiles()
        })
        
        try database.create(table: tableName, of: SampleChainCall.self)
        
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
            let insert = try database.prepareInsert(ofClass: SampleChainCall.self, into: tableName)
            try insert.execute(with: objects)            
        }
        
        //Select objects
        do {
            let select = try database.prepareSelect(on: (\SampleChainCall.intValue).max().as((\SampleChainCall.intValue)), from: tableName).where(\SampleChainCall.intValue==1).group(by: \SampleChainCall.intValue).limit(3)
            let objects: [SampleChainCall] = try select.allObjects()
        }
        
        //Select rows
        do {
            let rowSelect = try database.prepareRowSelect(on: SampleChainCall.allProperties, from: tableName).where(\SampleChainCall.intValue==1).limit(3)
            let needBreak = true
            while let row = try rowSelect.nextRow() {
                if needBreak {
                    break
                } 
            }
        }
        
        //Update by object
        do {
            let update = try database.prepareUpdate(table: tableName, onProperties: \SampleChainCall.stringValue)
            let object = SampleChainCall()
            object.stringValue = "Update by object"
            try update.execute(with: object)
        }
        
        //Delete
        do {
            let delete = try database.prepareDelete(from: tableName)
            try delete.execute()
        }
        
        print("Sample-chaincall End")
    }catch let error {
        print(error)
    }
}
//void sample_chaincall_main(NSString *baseDirectory)
//{
//    NSLog(@"Sample-chaincall Begin");
//    NSString *className = NSStringFromClass(WCTSampleSampleChainCall.class);
//    NSString *path = [baseDirectory stringByAppendingPathComponent:className];
//    NSString *tableName = className;
//    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
//    [database close:^{
//        [database removeFilesWithError:nil];
//        }];
//    BOOL ret = [database createTableAndIndexesOfName:tableName withClass:WCTSampleSampleChainCall.class];
//    assert(ret);
//    
//    //Insert objects
//    {
//        NSMutableArray *objects = [[NSMutableArray alloc] init];
//        WCTSampleSampleChainCall *object1 = [[WCTSampleSampleChainCall alloc] init];
//        object1.intValue = 1;
//        object1.stringValue = @"Insert objects";
//        [objects addObject:object1];
//        WCTSampleSampleChainCall *object2 = [[WCTSampleSampleChainCall alloc] init];
//        object2.intValue = 2;
//        object2.stringValue = @"Insert objects";
//        [objects addObject:object2];
//        WCTInsert *insert = [database prepareInsertObjectsOfClass:WCTSampleSampleChainCall.class
//        into:tableName];
//        BOOL ret = [insert executeWithObjects:objects];
//        if (!ret) {
//            NSLog(@"Insert objects Error %@", insert.error);
//        }
//    }
//    
//    //Select objects
//    {
//        WCTSelect *select = [[[[database prepareSelectObjectsOnResults:WCTSampleSampleChainCall.intValue.max()
//            fromTable:tableName] where:WCTSampleSampleChainCall.intValue == 1] groupBy:{WCTSampleSampleChainCall.intValue}] limit:3];
//        NSArray *objects = select.allObjects;
//        if (!objects) {
//            NSLog(@"Select objects Error %@", select.error);
//        }
//    }
//        //Select rows
//        {
//            WCTRowSelect *select = [[[database prepareSelectRowsOnResults:WCTSampleSampleChainCall.AllProperties
//                fromTable:tableName] where:WCTSampleSampleChainCall.intValue == 1] limit:3];
//            WCTOneRow *row;
//            BOOL needBreak = YES;
//            while ((row = [select nextRow])) {
//                if (needBreak) {
//                    break;
//                }
//            }
//    }
//    
//    //Update by object
//    {
//        WCTUpdate *update = [database prepareUpdateTable:tableName
//            onProperties:WCTSampleSampleChainCall.stringValue];
//        WCTSampleSampleChainCall *object = [[WCTSampleSampleChainCall alloc] init];
//        object.stringValue = @"Update by object";
//        BOOL ret = [update executeWithObject:object];
//        if (!ret) {
//            NSLog(@"Update by object Error %@", update.error);
//        }
//    }
//    
//    //Delete
//    {
//        WCTDelete *deletion = [database prepareDeleteFromTable:tableName];
//        BOOL ret = [deletion execute];
//        if (!ret) {
//            NSLog(@"Delete Error %@", deletion.error);
//        }
//    }
//    NSLog(@"Sample-chaincall End");
//}

