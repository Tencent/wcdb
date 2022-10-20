//
// Created by sanhuazhang on 2019/05/02
//

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

#import "TestCase.h"
#import <WCDB/WCTDatabase+Monitor.h>

@interface DatabaseTests : DatabaseTestCase

@end

@implementation DatabaseTests

- (void)test_tag
{
    TestCaseAssertNotEqual(self.database.tag, WCTInvalidTag);
    WCTDatabase* another = [[WCTDatabase alloc] initWithPath:self.path];
    TestCaseAssertEqual(self.database.tag, another.tag);
}

- (void)test_path
{
    TestCaseAssertTrue([self.database.path isEqualToString:self.path]);
}

- (void)test_open_and_close
{
    TestCaseAssertFalse([self.database isOpened]);
    TestCaseAssertTrue([self.database canOpen]);
    TestCaseAssertTrue([self.database isOpened]);
    [self.database close];
    TestCaseAssertFalse([self.database isOpened]);
}

- (void)test_blockade
{
    [self.database blockade];
    __block NSDate* subthread;
    [self.dispatch async:^{
        TestCaseAssertTrue([self.database canOpen]);
        subthread = [NSDate date];
    }];
    [NSThread sleepForTimeInterval:1];
    NSDate* main = [NSDate date];
    [self.database unblockade];
    [self.dispatch waitUntilDone];
    TestCaseAssertTrue([main compare:subthread] == NSOrderedAscending);
}

- (void)test_blockaded_close
{
    __block NSDate* main;
    __block NSDate* subthread;
    [self.database close:^{
        [self.dispatch async:^{
            TestCaseAssertTrue([self.database canOpen]);
            subthread = [NSDate date];
        }];
        [NSThread sleepForTimeInterval:1];
        main = [NSDate date];
    }];
    [self.dispatch waitUntilDone];
    TestCaseAssertTrue([main compare:subthread] == NSOrderedAscending);
}

- (void)test_readonly
{
    NSString* tableName = @"testTable";

    TestCaseObject* object = [Random.shared autoIncrementTestCaseObject];

    TestCaseAssertTrue([self.database createTable:tableName withClass:TestCaseObject.class]);
    TestCaseAssertTrue([self.database insertObject:object intoTable:tableName]);

    [self.database close:^{
        for (NSString* path in self.database.paths) {
            if ([self.fileManager fileExistsAtPath:path]) {
                TestCaseAssertTrue([self.fileManager setAttributes:@{NSFileImmutable : @(YES)} ofItemAtPath:path error:nil]);
            }
        }
    }];

    TestCaseAssertTrue([self.database canOpen]);
    TestCaseAssertTrue([self.database getObjectOfClass:TestCaseObject.class fromTable:tableName] != nil);
    TestCaseAssertFalse([self.database insertObject:object intoTable:tableName]);

    // reset attribute
    for (NSString* path in self.database.paths) {
        if ([self.fileManager fileExistsAtPath:path]) {
            TestCaseAssertTrue([self.fileManager setAttributes:@{ NSFileImmutable : @(NO) } ofItemAtPath:path error:nil]);
        }
    }
}

- (void)test_feature_run_while_closing
{
    TestCaseAssertTrue([self.database canOpen]);
    TestCaseAssertTrue([self.database isOpened]);
    [self.database close:^{
        TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion()).to(123)]);
    }];
    TestCaseAssertFalse([self.database isOpened]);
}

- (void)test_purge
{
    // acquire handle
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);

    TestCaseAssertTrue([self.database isOpened]);
    [self.database purge];
    TestCaseAssertFalse([self.database isOpened]);
}

- (void)test_purge_all
{
    // acquire handle
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);

    TestCaseAssertTrue([self.database isOpened]);
    [WCTDatabase purgeAll];
    TestCaseAssertFalse([self.database isOpened]);
}

- (void)testCheckpoint
{
    NSString* tableName = @"testTable";

    TestCaseObject* object = [Random.shared autoIncrementTestCaseObject];

    TestCaseAssertTrue([self.database createTable:tableName withClass:TestCaseObject.class]);
    __block unsigned long walFrameNum = 0;
    for (int i = 0; i < 2000; i++) {
        TestCaseAssertTrue([[self.database getRowFromStatement:WCDB::StatementSelect().select(WCDB::Expression::function("count").invokeAll()).from(tableName)].firstObject numberValue].unsignedIntValue == i);
        TestCaseAssertTrue([self.database insertObject:object intoTable:tableName]);
        [self.database passiveCheckpoint];
        [self.database close:^{
            if (walFrameNum != 0) {
                TestCaseAssertTrue(walFrameNum == [self.database getNumberOfWalFrames].value());
            } else {
                walFrameNum = [self.database getNumberOfWalFrames].value();
            }
            TestCaseAssertTrue(walFrameNum > 0);
            for (NSString* path in self.database.paths) {
                if ([path hasSuffix:@"wal"]) {
                    TestCaseAssertTrue([self.fileManager fileExistsAtPath:path]);
                    TestCaseAssertTrue([self.fileManager getFileSizeIfExists:path] > 32);
                } else if ([path hasSuffix:@"shm"]) {
                    TestCaseAssertTrue([self.fileManager fileExistsAtPath:path]);
                    TestCaseAssertTrue([self.fileManager getFileSizeIfExists:path] >= 32 * 1024);
                }
            }
        }];
    }
}

- (void)test_long_write_with_checkpoint
{
    NSString* tableName = @"testTable";
    TestCaseAssertTrue([self.database createTable:tableName withClass:TestCaseObject.class]);

    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 5; j++) {
            NSArray<TestCaseObject*>* objs = [[Random shared] autoIncrementTestCaseObjectsWithCount:2];
            TestCaseAssertTrue([self.database insertObjects:objs intoTable:tableName]);
        }
        [self.dispatch async:^{
            [self.database passiveCheckpoint];
        }];
        if ([Random shared].boolean) {
            usleep(10000);
        }
        TestCaseAssertTrue([self.database getValueOnResultColumn:TestCaseObject.allProperties.count() fromTable:tableName].numberValue.intValue == 10 * (1 + i));

        if ([Random shared].boolean) {
            [self.database close];
        }
    }
}

- (void)test_in_memory_db
{
    WCTDatabase* db = [[WCTDatabase alloc] initInMemoryDatabase];
    TestCaseObject* object = [Random.shared autoIncrementTestCaseObject];
    NSString* tableName = @"testTable";
    TestCaseAssertTrue([db createTable:tableName withClass:TestCaseObject.class]);
    for (int i = 0; i < 100; i++) {
        TestCaseAssertTrue([db insertObject:object intoTable:tableName]);
    }
    TestCaseAssertTrue([[db getRowFromStatement:WCDB::StatementSelect().select(WCDB::Expression::function("count").invokeAll()).from(tableName)].firstObject numberValue].unsignedIntValue == 100);
    TestCaseAssertTrue([db passiveCheckpoint]);
    [db close];
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:db.path]);
    db = [[WCTDatabase alloc] initInMemoryDatabase];
    TestCaseAssertTrue([[db getRowFromStatement:WCDB::StatementSelect().select(WCDB::Expression::function("count").invokeAll()).from(tableName)].firstObject numberValue].unsignedIntValue == 0);
}

@end
