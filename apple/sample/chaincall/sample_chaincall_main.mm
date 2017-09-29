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

#import "sample_chaincall_main.h"
#import "WCTSampleChainCall+WCTTableCoding.h"
#import "WCTSampleChainCall.h"

void sample_chaincall_main(NSString *baseDirectory)
{
    NSLog(@"Sample-chaincall Begin");
    NSString *className = NSStringFromClass(WCTSampleChainCall.class);
    NSString *path = [baseDirectory stringByAppendingPathComponent:className];
    NSString *tableName = className;
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
    [database close:^{
      [database removeFilesWithError:nil];
    }];
    BOOL ret = [database createTableAndIndexesOfName:tableName withClass:WCTSampleChainCall.class];
    assert(ret);

    //Insert objects
    {
        NSMutableArray *objects = [[NSMutableArray alloc] init];
        WCTSampleChainCall *object1 = [[WCTSampleChainCall alloc] init];
        object1.intValue = 1;
        object1.stringValue = @"Insert objects";
        [objects addObject:object1];
        WCTSampleChainCall *object2 = [[WCTSampleChainCall alloc] init];
        object2.intValue = 2;
        object2.stringValue = @"Insert objects";
        [objects addObject:object2];
        WCTInsert *insert = [database prepareInsertObjectsOfClass:WCTSampleChainCall.class
                                                             into:tableName];
        BOOL ret = [insert executeWithObjects:objects];
        if (!ret) {
            NSLog(@"Insert objects Error %@", insert.error);
        }
    }

    //Select objects
    {
        WCTSelect *select = [[[[database prepareSelectObjectsOnResults:WCTSampleChainCall.intValue.max()
                                                             fromTable:tableName] where:WCTSampleChainCall.intValue == 1] groupBy:{WCTSampleChainCall.intValue}] limit:3];
        NSArray *objects = select.allObjects;
        if (!objects) {
            NSLog(@"Select objects Error %@", select.error);
        }
    }
    //Select rows
    {
        WCTRowSelect *select = [[[database prepareSelectRowsOnResults:WCTSampleChainCall.AllProperties
                                                            fromTable:tableName] where:WCTSampleChainCall.intValue == 1] limit:3];
        WCTOneRow *row;
        BOOL needBreak = YES;
        while ((row = [select nextRow])) {
            if (needBreak) {
                break;
            }
        }
    }

    //Update by object
    {
        WCTUpdate *update = [database prepareUpdateTable:tableName
                                            onProperties:WCTSampleChainCall.stringValue];
        WCTSampleChainCall *object = [[WCTSampleChainCall alloc] init];
        object.stringValue = @"Update by object";
        BOOL ret = [update executeWithObject:object];
        if (!ret) {
            NSLog(@"Update by object Error %@", update.error);
        }
    }

    //Delete
    {
        WCTDelete *deletion = [database prepareDeleteFromTable:tableName];
        BOOL ret = [deletion execute];
        if (!ret) {
            NSLog(@"Delete Error %@", deletion.error);
        }
    }
    NSLog(@"Sample-chaincall End");
}
