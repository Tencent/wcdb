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

#import "sample_fts_main.h"
#import "WCTSampleFTSData+WCTTableCoding.h"
#import "WCTSampleFTSData.h"
#import "WCTSampleFTSOrigin+WCTTableCoding.h"
#import "WCTSampleFTSOrigin.h"

void sample_fts_main(NSString *baseDirectory)
{
    NSLog(@"Sample-fts Begin");
    NSString *classNameOrigin = NSStringFromClass(WCTSampleFTSOrigin.class);
    NSString *pathOrigin = [baseDirectory stringByAppendingPathComponent:classNameOrigin];
    NSString *tableNameOrigin = classNameOrigin;
    WCTDatabase *databaseOrigin = [[WCTDatabase alloc] initWithPath:pathOrigin];
    [databaseOrigin close:^{
      [databaseOrigin removeFilesWithError:nil];
    }];

    [databaseOrigin createTableAndIndexesOfName:tableNameOrigin withClass:WCTSampleFTSOrigin.class];

    //prepare
    {
        //English
        WCTSampleFTSOrigin *object = [[WCTSampleFTSOrigin alloc] init];
        object.name = @"English";
        object.content = @"This is sample content";
        object.isAutoIncrement = YES;
        [databaseOrigin insertObject:object into:tableNameOrigin];
    }
    {
        //Number
        WCTSampleFTSOrigin *object = [[WCTSampleFTSOrigin alloc] init];
        object.name = @"Number";
        object.content = @"123";
        object.isAutoIncrement = YES;
        [databaseOrigin insertObject:object into:tableNameOrigin];
    }
    {
        //Chinese
        WCTSampleFTSOrigin *object = [[WCTSampleFTSOrigin alloc] init];
        object.name = @"Chinese";
        object.content = @"这是测试内容";
        object.isAutoIncrement = YES;
        [databaseOrigin insertObject:object into:tableNameOrigin];
    }

    NSString *classNameFTS = NSStringFromClass(WCTSampleFTSData.class);
    NSString *pathFTS = [baseDirectory stringByAppendingPathComponent:classNameFTS];
    NSString *tableNameFTS = classNameFTS;
    WCTDatabase *databaseFTS = [[WCTDatabase alloc] initWithPath:pathFTS];
    [databaseFTS close:^{
      [databaseFTS removeFilesWithError:nil];
    }];

    [databaseFTS setTokenizer:WCTTokenizerNameWCDB];

    [databaseFTS createVirtualTableOfName:tableNameFTS withClass:WCTSampleFTSData.class];

    //Build Full-Text-Search Index
    {
        NSArray<WCTSampleFTSOrigin *> *objects = [databaseOrigin getAllObjectsOfClass:WCTSampleFTSOrigin.class fromTable:tableNameOrigin];
        NSMutableArray *ftsDataArray = [[NSMutableArray alloc] init];
        for (WCTSampleFTSOrigin *object in objects) {
            WCTSampleFTSData *ftsData = [[WCTSampleFTSData alloc] init];
            ftsData.name = object.name;
            ftsData.content = object.content;
            [ftsDataArray addObject:ftsData];
        }
        [databaseFTS insertObjects:ftsDataArray into:tableNameFTS];
    }

    //Full-Text-Search by `match`
    NSArray<WCTSampleFTSData *> *ftsDataArray = [databaseFTS getObjectsOfClass:WCTSampleFTSData.class fromTable:tableNameFTS where:WCTSampleFTSData.PropertyNamed(tableNameFTS).match("Eng*")];
    for (WCTSampleFTSData *ftsData in ftsDataArray) {
        NSLog(@"Match name:%@ content:%@", ftsData.name, ftsData.content);
    }

    //Full-Text-Search info by `match`
    //See http://www.sqlite.org/fts3.html#snippet for further information
    WCTOneRow *row = [databaseFTS getOneRowOnResults:{WCTSampleFTSData.PropertyNamed(tableNameFTS).snippet(), WCTSampleFTSData.PropertyNamed(tableNameFTS).offsets()} fromTable:tableNameFTS where:WCTSampleFTSData.PropertyNamed(tableNameFTS).match("12*")];
    NSLog(@"Snippet: %@ Offsets: %@", row[0], row[1]);

    NSLog(@"Sample-fts End");
}
