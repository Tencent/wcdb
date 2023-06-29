//
// Created by sanhuazhang on 2019/12/9.
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

#import "MMDBTests.h"

@implementation MMDBTests {
    MMDBFactory* _factory;
}

- (MMDBFactory*)factory
{
    @synchronized(self) {
        if (_factory == nil) {
            _factory = [[MMDBFactory alloc] initWithDirectory:self.class.cacheRoot];
        }
        return _factory;
    }
}

- (void)setUp
{
    [super setUp];

    self.factory.tolerance = 0.0f;
    self.factory.quality = 1000000;
}

- (void)setUpDatabase
{
    [self.factory produce:self.path];
    self.tableName = self.factory.tableName;

    TestCaseAssertOptionalEqual([self.database getNumberOfWalFrames], 0);
    TestCaseAssertFalse([self.database isOpened]);
}

- (void)tearDownDatabase
{
    [self.database removeFiles];
}

- (void)doTestGetMaxTime
{
    [self
    doMeasure:^{
        [self.database getValueOnResultColumn:DBMessage.createTime.max() fromTable:self.tableName];
    }
    setUp:^{
        [self setUpDatabase];
    }
    tearDown:^{
        [self tearDownDatabase];
    }
    checkCorrectness:nil];
}

#pragma mark - Tests
- (void)test_get_max_time
{
    [self doTestGetMaxTime];
}

- (void)testCreateTable
{
    NSDate* start = [NSDate date];
    TestCaseLog(@"Database path: %@", self.database.path);
    for (int i = 0; i < 10000; i++) {
        if (i % 100 == 0 && i > 0) {
            TestCaseLog(@"CreateTableTime: %lld, %d", (UInt64) [[NSDate date] timeIntervalSinceDate:start], i);
            start = [NSDate date];
        }

        if (i % 1000 == 0 && i > 0) {
            [self.database close:^{
                [self.database truncateCheckpoint];
            }];
        }

        NSString* userName = [Random.shared stringWithLength:30];

        TestCaseAssertTrue([self.database createTable:[@"Chat_" stringByAppendingString:userName] withClass:DBMessage.class]);

        TestCaseAssertTrue([self.database createTable:[@"ChatExt_" stringByAppendingString:userName] withClass:DBMessageExt.class]);
    }
    [self.database close:^{
        [self.database truncateCheckpoint];
    }];
    sleep(100000);
}

- (void)testCreateCipherTable
{
    NSDate* start = [NSDate date];
    TestCaseLog(@"Database path: %@", self.database.path);
    NSData* cipher = [@"abc" dataUsingEncoding:NSUTF8StringEncoding];
    [self.database setCipherKey:cipher andCipherPageSize:4096 andCipherViersion:WCTCipherVersion2];
    for (int i = 0; i < 10000; i++) {
        if (i % 100 == 0 && i > 0) {
            TestCaseLog(@"CreateTableTime: %lld, %d", (UInt64) [[NSDate date] timeIntervalSinceDate:start], i);
            start = [NSDate date];
        }

        if (i % 1000 == 0 && i > 0) {
            [self.database close:^{
                [self.database truncateCheckpoint];
            }];
        }

        NSString* userName = [Random.shared stringWithLength:30];

        TestCaseAssertTrue([self.database createTable:[@"Chat_" stringByAppendingString:userName] withClass:DBMessage.class]);

        TestCaseAssertTrue([self.database createTable:[@"ChatExt_" stringByAppendingString:userName] withClass:DBMessageExt.class]);
    }
    [self.database close:^{
        [self.database truncateCheckpoint];
    }];
    sleep(100000);
}

- (void)testOpenDatabase
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:@"/Users/qiuwenchen/Desktop/AndroidDBTest/testDatabase"];
    [self
    doMeasure:^{
        [database canOpen];
    }
    setUp:nil
    tearDown:^{
        [database close];
    }
    checkCorrectness:nil];
}

- (void)testMemory
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:@"/Users/qiuwenchen/Desktop/iOSDBTest/testDatabase"];
    for (int i = 0; i < 32; i++) {
        [self.dispatch async:^{
            WCTHandle* handle = [database getHandle];
            [handle tableExists:@"abc"];
            sleep(10);
        }];
    }
    [self.dispatch waitUntilDone];
    sleep(100000);
}

@end
