//
//  sample_fts_main.m
//  WCDB
//
//  Created by sanhuazhang on 15/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

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

    [databaseFTS setTokenizer:WCTTokenizerNameApple];
    [databaseFTS createVirtualTableOfName:tableNameFTS usingModule:@"fts3" withClass:WCTSampleFTSData.class];

    //Build Full-Text-Search Index
    {
        NSArray<WCTSampleFTSOrigin *> *objects = [databaseOrigin getAllObjectsOfClass:WCTSampleFTSOrigin.class fromTable:tableNameOrigin];
        NSMutableArray *ftsDatas = [[NSMutableArray alloc] init];
        for (WCTSampleFTSOrigin *object in objects) {
            WCTSampleFTSData *ftsData = [[WCTSampleFTSData alloc] init];
            ftsData.name = object.name;
            ftsData.content = object.content;
            [ftsDatas addObject:ftsData];
        }
        [databaseFTS insertObjects:ftsDatas into:tableNameFTS];
    }

    //Full-Text-Search by `match`
    NSArray<WCTSampleFTSData *> *ftsDatas = [databaseFTS getObjectsOfClass:WCTSampleFTSData.class fromTable:tableNameFTS where:WCTSampleFTSData.content.match("123")];
    for (WCTSampleFTSData *ftsData in ftsDatas) {
        NSLog(@"Match name:%@ content:%@", ftsData.name, ftsData.content);
    }

    NSLog(@"Sample-fts End");
}
