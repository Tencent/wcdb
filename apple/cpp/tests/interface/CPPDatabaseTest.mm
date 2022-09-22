//
// Created by 陈秋文 on 2022/8/14.
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

#import "CPPTestCase.h"

@interface CPPDatabaseTests : CPPTableTestCase

@end

@implementation CPPDatabaseTests

- (void)test_tag
{
    TestCaseAssertNotEqual(self.database->getTag(), WCDB::Tag::invalid());
    WCDB::Database another = WCDB::Database(self.path.UTF8String);
    TestCaseAssertEqual(self.database->getTag(), another.getTag());
}

- (void)test_path
{
    TestCaseAssertCPPStringEqual(self.database->getPath().data(), self.path.UTF8String);
}

- (void)test_open_and_close
{
    TestCaseAssertFalse(self.database->isOpened());
    TestCaseAssertTrue(self.database->canOpen());
    TestCaseAssertTrue(self.database->isOpened());
    self.database->close();
    TestCaseAssertFalse(self.database->isOpened());
}

- (void)test_blockade
{
    self.database->blockade();
    __block NSDate* subthread;
    [self.dispatch async:^{
        TestCaseAssertTrue(self.database->canOpen());
        subthread = [NSDate date];
    }];
    [NSThread sleepForTimeInterval:1];
    NSDate* main = [NSDate date];
    self.database->unblockade();
    [self.dispatch waitUntilDone];
    TestCaseAssertTrue([main compare:subthread] == NSOrderedAscending);
}

- (void)test_blockaded_close
{
    NSDate* main = [NSDate date];
    NSDate* subthread = [NSDate date];
    self.database->close([&]() {
        [self.dispatch async:^{
            TestCaseAssertTrue(self.database->canOpen());
            subthread = [NSDate date];
        }];
        [NSThread sleepForTimeInterval:1];
        main = [NSDate date];
    });
    [self.dispatch waitUntilDone];
    TestCaseAssertTrue([main compare:subthread] == NSOrderedAscending);
}

- (void)test_readonly
{
    WCDB::OneRowValue row = [Random.shared autoIncrementTestCaseValuesWithCount:1][0];
    TestCaseAssertTrue([self createValueTable]);
    TestCaseAssertTrue(self.database->insertRows(row, self.columns, self.tableName.UTF8String));

    self.database->close([=]() {
        for (const WCDB::UnsafeStringView path : self.database->getPaths()) {
            NSString* nsPath = [NSString stringWithUTF8String:path.data()];
            if ([self.fileManager fileExistsAtPath:nsPath]) {
                TestCaseAssertTrue([self.fileManager setAttributes:@{ NSFileImmutable : @(YES) } ofItemAtPath:nsPath error:nil]);
            }
        }
    });

    TestCaseAssertTrue(self.database->canOpen());
    auto values = self.database->selectAllRow(self.resultColumns, self.tableName.UTF8String);
    TestCaseAssertTrue(values.has_value() && values.value().size() == 1)
    TestCaseAssertFalse(self.database->insertRows(row, self.columns, self.tableName.UTF8String));

    // reset attribute
    for (const WCDB::UnsafeStringView path : self.database->getPaths()) {
        NSString* nsPath = [NSString stringWithUTF8String:path.data()];
        if ([self.fileManager fileExistsAtPath:nsPath]) {
            TestCaseAssertTrue([self.fileManager setAttributes:@{ NSFileImmutable : @(NO) } ofItemAtPath:nsPath error:nil]);
        }
    }
}

- (void)test_feature_run_while_closing
{
    TestCaseAssertTrue(self.database->canOpen());
    TestCaseAssertTrue(self.database->isOpened());
    self.database->close([=]() {
        TestCaseAssertTrue(self.database->execute(WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion()).to(123)));
    });
    TestCaseAssertFalse(self.database->isOpened());
}

- (void)test_purge
{
    // acquire handle
    TestCaseAssertTrue(self.database->execute(WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())));

    TestCaseAssertTrue(self.database->isOpened());
    self.database->purge();
    TestCaseAssertFalse(self.database->isOpened());
}

- (void)test_purge_all
{
    // acquire handle
    TestCaseAssertTrue(self.database->execute(WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())));

    TestCaseAssertTrue(self.database->isOpened());
    WCDB::Database::purgeAll();
    TestCaseAssertFalse(self.database->isOpened());
}

- (void)testCheckpoint
{
    WCDB::MultiRowsValue rows = [Random.shared autoIncrementTestCaseValuesWithCount:2000];

    TestCaseAssertTrue([self createValueTable]);

    unsigned long walFrameNum = 0;
    for (int i = 0; i < 2000; i++) {
        auto value = self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB::Expression::function("count").invokeAll()).from(self.tableName.UTF8String));
        TestCaseAssertTrue(value.has_value() && value.value().intValue() == i);
        TestCaseAssertTrue(self.database->insertRows(rows[i], self.columns, self.tableName.UTF8String));
        self.database->passiveCheckpoint();
        self.database->close([&]() {
            if (walFrameNum != 0) {
                TestCaseAssertTrue(walFrameNum == [self getNumberOfWalFrames]);
            } else {
                walFrameNum = [self getNumberOfWalFrames];
            }
            TestCaseAssertTrue(walFrameNum > 0);
            for (const WCDB::UnsafeStringView& path : self.database->getPaths()) {
                NSString* nsPath = [NSString stringWithUTF8String:path.data()];
                if (path.hasSuffix("wal")) {
                    TestCaseAssertTrue([self.fileManager fileExistsAtPath:nsPath]);
                    TestCaseAssertTrue([self.fileManager getFileSizeIfExists:nsPath] > 32);
                } else if (path.hasSuffix("shm")) {
                    TestCaseAssertTrue([self.fileManager fileExistsAtPath:nsPath]);
                    TestCaseAssertTrue([self.fileManager getFileSizeIfExists:nsPath] >= 32 * 1024);
                }
            }
        });
    }
}

@end
