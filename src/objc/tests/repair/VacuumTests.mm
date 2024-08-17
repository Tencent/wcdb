//
// Created by qiuwenchen on 2023/12/17.
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

#import "BackupTestCase.h"
#import "CoreConst.h"
#import "FTS3Object+WCTTableCoding.h"
#import "FTS3Object.h"
#import "FTS5Object.h"
#import "Random+RepairTestObject.h"
#import "SizeBasedFactory.h"
#import <Foundation/Foundation.h>

@interface VacuumTests : BackupTestCase

@end

@implementation VacuumTests

- (void)setUp
{
    [super setUp];
    self.objectCount = 10000;
}

- (void)test_empty
{
    [self
    executeTest:^{
        self.path = [self.directory stringByAppendingPathComponent:@"empty"];
        TestCaseAssertTrue([self.database vacuum:nil]);
        self.path = nil;
    }];
}

- (void)doTestObjectsExist
{
    XCTAssertTrue([[self.table getObjects] isEqualToArray:self.objects]);
}

- (void)doTestObjectsNoExist
{
    XCTAssertFalse([self.database tableExists:self.tableName]);
}

- (void)doTestFactoryNotExist
{
    XCTAssertFalse([self.fileManager fileExistsAtPath:[self.database.path stringByAppendingString:@".factory"]]);
}

- (void)doTestVacuum
{
    __block double lastPercentage = 0;
    __block BOOL sanity = YES;
    bool succeed = [self.database vacuum:^(double percentage, double increment) {
        if (percentage - lastPercentage != increment
            || increment <= 0) {
            TestCaseFailure();
            sanity = NO;
        }
        lastPercentage = percentage;
        return true;
    }];
    TestCaseAssertTrue(sanity);
    TestCaseAssertTrue(succeed);
    TestCaseAssertEqual(lastPercentage, 1.0);
}

- (void)doTestInterruptVacuum
{
    __block double lastPercentage = 0;
    __block BOOL sanity = YES;
    float interruptPoint = Random.shared.float_0_1 * 0.6f;
    bool succeed = [self.database vacuum:^(double percentage, double increment) {
        if (percentage - lastPercentage != increment
            || increment <= 0) {
            TestCaseFailure();
            sanity = NO;
        }
        lastPercentage = percentage;
        return percentage < interruptPoint;
    }];
    TestCaseAssertTrue(sanity);
    TestCaseAssertTrue(!succeed || ![self.testClass hasRowid]);
    TestCaseAssertTrue(![self.testClass hasRowid] || (lastPercentage >= interruptPoint && lastPercentage < 1 && lastPercentage < interruptPoint + 0.5));
}

- (void)doTestVacuumFailed
{
    __block double lastPercentage = 0;
    __block BOOL sanity = YES;
    bool succeed = [self.database vacuum:^(double percentage, double increment) {
        if (percentage - lastPercentage != increment
            || increment <= 0) {
            TestCaseFailure();
            sanity = NO;
        }
        lastPercentage = percentage;
        return true;
    }];
    TestCaseAssertTrue(sanity);
    TestCaseAssertFalse(succeed);
    TestCaseAssertEqual(lastPercentage, 0);
}

#pragma mark - Non-Corrupted
- (void)test_vacuum
{
    [self
    executeTest:^{
        [self doTestVacuum];
        [self doTestObjectsExist];
        [self doTestFactoryNotExist];
    }];
}

- (void)test_interrupt_vacuum
{
    [self
    executeTest:^{
        [self doTestInterruptVacuum];
        [self doTestObjectsExist];
        [self doTestFactoryNotExist];
    }];
}

#pragma mark - Corrupted
- (void)test_vacuum_corrupted
{
    [self
    executeTest:^{
        TestCaseAssertTrue([self.database corruptHeaderWithinCloseAfterTruncatedCheckpoint]);
        [self doTestVacuumFailed];
        [self doTestObjectsNoExist];
        [self doTestFactoryNotExist];
    }];
}

- (void)test_vacuum_corrupt_opened
{
    [self
    executeTest:^{
        TestCaseAssertTrue([self.database canOpen]);
        [self.database truncateCheckpoint];
        [self.database corruptPage:1];

        [self doTestVacuumFailed];

        [self.database close];
        [self doTestObjectsNoExist];
        [self doTestFactoryNotExist];
    }];
}

#pragma mark - auto vacuum
- (void)test_auto_vacuum
{
    [self.database enableAutoVacuum:NO];
    WCTValue* autoVaccum = [self.database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::autoVacuum())];
    TestCaseAssertTrue(autoVaccum.numberValue.intValue == 1);

    [self.database enableAutoVacuum:YES];
    autoVaccum = [self.database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::autoVacuum())];
    TestCaseAssertTrue(autoVaccum.numberValue.intValue == 2);
}

- (void)test_incremental_vacuum
{
    [self.database enableAutoVacuum:YES];
    TestCaseAssertTrue([self createTable]);
    TestCaseAssertTrue([self.table insertObjects:[[Random shared] autoIncrementTestCaseObjectsWithCount:1000]]);
    TestCaseAssertTrue([self.database truncateCheckpoint]);
    TestCaseAssertTrue([self.database getNumberOfPages] > 3);

    TestCaseAssertTrue([self.database dropTable:self.tableName]);
    TestCaseAssertTrue([self.database truncateCheckpoint]);
    TestCaseAssertTrue([self.database getNumberOfPages] > 3);

    TestCaseAssertTrue([self.database incrementalVacuum:0]);
    TestCaseAssertTrue([self.database truncateCheckpoint]);
    WCTValue* freelist = [self.database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::freelistCount())];
    TestCaseAssertTrue(freelist != nil && freelist.numberValue.intValue == 0);
    TestCaseAssertTrue([self.database getNumberOfPages] == 3);
}

- (void)test_backup_and_recover_with_incremental_vacuum
{
    [self.database enableAutoVacuum:NO];
    [self.database enableAutoCheckpoint:NO];
    [self.database enableAutoBackup:YES];
    TestCaseObject* object = [Random.shared autoIncrementTestCaseObject];
    XCTAssertTrue([self createTable]);
    TestCaseAssertTrue([self.table insertObject:object]);

    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.incrementalMaterialPath]);
    TestCaseAssertTrue([self.database passiveCheckpoint]);
    usleep(10000);

    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    TestCaseAssertFalse([self.fileManager fileExistsAtPath:self.database.incrementalMaterialPath]);

    [NSThread sleepForTimeInterval:WCDB::OperationQueueTimeIntervalForBackup + self.delayForTolerance];
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.firstMaterialPath]);
    TestCaseAssertTrue([self.fileManager fileExistsAtPath:self.database.incrementalMaterialPath]);

    TestCaseAssertTrue([self.database retrieve:nil] == 1);

    WCTValue* autoVaccum = [self.database getValueFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::autoVacuum())];
    TestCaseAssertTrue(autoVaccum.numberValue.intValue == 1);
}

#pragma mark - FTS
- (void)test_fts3
{
    self.tableClass = FTS3Object.class;
    self.isVirtualTable = YES;
    [self.database addTokenizer:WCTTokenizerOneOrBinary];
    [WCTDatabase configTraditionalChineseDict:@{
        @"們" : @"们",
        @"員" : @"员",
    }];
    NSMutableArray<FTS3Object*>* objects = [[NSMutableArray alloc] init];
    for (int i = 0; i < self.objectCount; i++) {
        FTS3Object* object = [[FTS3Object alloc] init];
        object.content = Random.shared.chineseString;
        object.extension = Random.shared.englishString;
        [objects addObject:object];
    }
    self.objects = objects;
    TestCaseAssertTrue([self createTable]);
    TestCaseAssertTrue([self.table insertObjects:objects]);
    [self doTestVacuum];
    [self doTestObjectsExist];
    self.tableClass = nil;
    self.isVirtualTable = NO;
}

- (void)test_fts5
{
    self.tableClass = FTS5Object.class;
    self.isVirtualTable = YES;
    [self.database addTokenizer:WCTTokenizerVerbatim];
    [WCTDatabase configTraditionalChineseDict:@{
        @"們" : @"们",
        @"員" : @"员",
    }];
    NSMutableArray<FTS5Object*>* objects = [[NSMutableArray alloc] init];
    for (int i = 0; i < self.objectCount; i++) {
        FTS5Object* object = [[FTS5Object alloc] init];
        object.content = Random.shared.chineseString;
        object.extension = Random.shared.englishString;
        [objects addObject:object];
    }
    self.objects = objects;
    TestCaseAssertTrue([self createTable]);
    TestCaseAssertTrue([self.table insertObjects:objects]);
    [self doTestVacuum];
    [self doTestObjectsExist];
    self.tableClass = nil;
    self.isVirtualTable = NO;
}

#ifndef WCDB_QUICK_TESTS
- (void)test_vacuum_huge_database
{
    self.skipDebugLog = YES;
    SizeBasedFactory* factory = [[SizeBasedFactory alloc] initWithDirectory:self.class.cacheRoot];
    factory.quality = 6LL * 1024 * 1024 * 1024; // 6GB > 4GB
    factory.tolerance = 0.02;

    [factory produce:self.path];

    __block double percentage = 0;
    TestCaseAssertTrue([self.database vacuum:^(double progress, double increment) {
        WCDB_UNUSED(increment);
        double newPercentage = progress * 100.0;
        if (newPercentage - percentage >= 1.0) {
            TestCaseLog(@"Retrieving %.2f%%", newPercentage);
            percentage = newPercentage;
        }
        return true;
    }]);
}
#endif

@end
