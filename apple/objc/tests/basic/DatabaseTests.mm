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

- (void)test_existing
{
    NSString* path = [self.path stringByAppendingString:@"_existing"];
    WCTTag tag = self.database.tag;

    @autoreleasepool {
        // non-exists
        {
            WCTDatabase* existing = [[WCTDatabase alloc] initWithPathOfAlivingDatabase:path];
            TestCaseAssertTrue(existing == nil);
        }

        WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];
        database.tag = tag;
        TestCaseAssertTrue(database != nil);

        // exists
        {
            WCTDatabase* existing = [[WCTDatabase alloc] initWithPathOfAlivingDatabase:path];
            TestCaseAssertTrue(existing != nil);
            TestCaseAssertEqual(tag, existing.tag);
        }
    }

    // non-exists
    {
        WCTDatabase* existing = [[WCTDatabase alloc] initWithPathOfAlivingDatabase:path];
        TestCaseAssertTrue(existing == nil);
    }
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

@end
