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

#import "sample_repair_main.h"
#import "WCTSampleRepair+WCTTableCoding.h"
#import "WCTSampleRepair.h"

void sample_repair_main(NSString *baseDirectory)
{
    NSLog(@"Sample-repair Begin");
    NSString *className = NSStringFromClass(WCTSampleRepair.class);
    NSString *path = [baseDirectory stringByAppendingPathComponent:className];
    NSString *recoverPath = [path stringByAppendingString:@"_recover"];
    NSString *tableName = className;
    NSData *databaseCipher = [@"databaseCipher" dataUsingEncoding:NSASCIIStringEncoding];
    NSData *backupCipher = [@"backupCipher" dataUsingEncoding:NSASCIIStringEncoding];

    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:path];
    [database setCipherKey:databaseCipher];
    [database close:^{
      [database removeFilesWithError:nil];
    }];

    //prepare
    {
        [database createTableAndIndexesOfName:tableName
                                    withClass:WCTSampleRepair.class];
        for (int i = 0; i < 10; ++i) {
            WCTSampleRepair *object = [[WCTSampleRepair alloc] init];
            object.identifier = i;
            object.content = [NSString stringWithFormat:@"%d", i];
            [database insertObject:object
                              into:tableName];
        }
        NSLog(@"The count of objects before: %lu", [database getAllObjectsOfClass:WCTSampleRepair.class fromTable:tableName].count);
    }

    //backup
    {
        BOOL ret = [database backupWithCipher:backupCipher];
        if (!ret) {
            abort();
        }
    }

    //get page size
    int pageSize;
    //get page size from origin database.
    //Since page size never change unless you can call "PRAGMA page_size=NewPageSize" to set it. You have no need to get the page size like this. Instead, you can hardcode it.
    {
        @autoreleasepool {
            WCTStatement *statement = [database prepare:WCDB::StatementPragma().pragma(WCDB::Pragma::PageSize)];
            [statement step];
            NSNumber *value = (NSNumber *) [statement getValueAtIndex:0];
            pageSize = value.intValue;
            statement = nil;
        }
    }

    //do something
    //corrupt
    {
        [database close:^{
          FILE *file = fopen(path.UTF8String, "rb+");
          unsigned char *zeroPage = new unsigned char[100];
          memset(zeroPage, 0, 100);
          fwrite(zeroPage, 100, 1, file);
          fclose(file);
        }];

        NSLog(@"The count of objects corrupted: %lu", [database getAllObjectsOfClass:WCTSampleRepair.class fromTable:tableName].count);
    }

    //repair
    {
        //Since recovering is a long time operation, you'd better call it in sub-thread.
        //dispatch_async(DISPATCH_QUEUE_PRIORITY_BACKGROUND, ^{
        WCTDatabase *recover = [[WCTDatabase alloc] initWithPath:recoverPath];
        [recover close:^{
          [recover removeFilesWithError:nil];
        }];
        [database close:^{
          [recover recoverFromPath:path withPageSize:pageSize backupCipher:backupCipher databaseCipher:databaseCipher];
        }];
        NSLog(@"The count of objects repaired: %lu", [recover getAllObjectsOfClass:WCTSampleRepair.class fromTable:tableName].count);
    }
    NSLog(@"Sample-repair End");
}
