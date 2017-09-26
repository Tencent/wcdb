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

#import "sample_table_main.h"
#import "WCTSampleTable+WCTTableCoding.h"
#import "WCTSampleTable.h"

void sample_table_main(NSString *baseDirectory)
{
    NSLog(@"Sample-table Begin");
    NSString *className = NSStringFromClass(WCTSampleTable.class);
    NSString *path = [baseDirectory stringByAppendingPathComponent:className];
    NSString *tableName = className;
    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
    [database close:^{
      [database removeFilesWithError:nil];
    }];

    //Create table and indexes from WCTTableCoding class
    {
        BOOL ret = [database createTableAndIndexesOfName:tableName
                                               withClass:WCTSampleTable.class];
    }
    //Drop table
    {
        BOOL ret = [database dropTableOfName:tableName];
    }
    //Create table from column def
    {
        BOOL ret = [database createTableOfName:tableName
                             withColumnDefList:{
                                                   WCTSampleTable.intValue.def(WCTColumnTypeInteger32),
                                                   WCTSampleTable.stringValue.def(WCTColumnTypeString),
                                               }];
    }

    //Create index from column index
    NSString *indexSubfix = @"_index";
    NSString *indexName = [tableName stringByAppendingString:indexSubfix];
    {
        BOOL ret = [database createIndexOfName:indexName
                                 withIndexList:{
                                                   WCTSampleTable.stringValue.index(WCTOrderedAscending)}
                                      forTable:tableName];
    }
    //Drop index
    {
        BOOL ret = [database dropIndexOfName:indexName];
    }

    //Add column
    {
        NSString *tableName2 = [tableName stringByAppendingString:@"2"];
        BOOL ret = [database createTableOfName:tableName2
                             withColumnDefList:{
                                                   WCTSampleTable.intValue.def(WCTColumnTypeInteger32),
                                               }];
        ret = [database addColumn:WCTSampleTable.stringValue.def(WCTColumnTypeString)
                         forTable:tableName2];
    }

    WCTTable *table;
    //Get table
    {
        table = [database getTableOfName:tableName
                               withClass:WCTSampleTable.class];
    }
    //Insert/Update/Select/Delete using table
    {
        WCTSampleTable *object = [[WCTSampleTable alloc] init];
        BOOL ret = [table insertObject:object];
    }
    NSLog(@"Sample-table End");
}
