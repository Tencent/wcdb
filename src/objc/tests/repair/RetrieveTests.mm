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

#import "BackupTestCase.h"
#import "Random+RepairTestObject.h"
#import "SizeBasedFactory.h"

@interface RetrieveTests : BackupTestCase

@end

@implementation RetrieveTests

- (void)test_empty
{
    [self
    executeTest:^{
        self.path = [self.directory stringByAppendingPathComponent:@"empty"];
        TestCaseAssertEqual([self.database retrieve:nil], -1);
        self.path = nil;
    }];
}

- (void)doTestObjectsRetrieved
{
    XCTAssertTrue([[self.table getObjects] isEqualToArray:self.objects]);
}

- (void)doTestObjectsNotRetrieved
{
    XCTAssertFalse([self.database tableExists:self.tableName]);
}

- (void)doBackupWithIncrementalMaterial
{
    XCTAssertTrue([self.database backup]);
    NSArray* objects = [[Random shared] repairObjectsWithClass:self.testClass andCount:100 startingFromIdentifier:self.objects.lastObject.identifier + 1];
    [self.objects addObjectsFromArray:objects];
    XCTAssertTrue([self.table insertObjects:objects]);

    XCTAssertTrue([self.database passiveCheckpoint]);
    usleep(10000);
    XCTAssertTrue([self.database backup]);
}

- (void)doTestRetrieve
{
    __block double lastPercentage = 0;
    __block BOOL sanity = YES;
    double score = [self.database retrieve:^(double percentage, double increment) {
        if (percentage - lastPercentage != increment
            || increment <= 0) {
            TestCaseFailure();
            sanity = NO;
        }
        lastPercentage = percentage;
        return true;
    }];
    TestCaseAssertTrue(sanity);
    TestCaseAssertEqual(score, 1.0);
    TestCaseAssertEqual(lastPercentage, 1.0);
}

- (void)doTestRetrieveFailed
{
    __block double lastPercentage = 0;
    __block BOOL sanity = YES;
    double score = [self.database retrieve:^(double percentage, double increment) {
        if (percentage - lastPercentage != increment
            || increment <= 0) {
            TestCaseFailure();
            sanity = NO;
        }
        lastPercentage = percentage;
        return true;
    }];
    TestCaseAssertTrue(sanity);
    TestCaseAssertNotEqual(score, 1.0);
    TestCaseAssertEqual(lastPercentage, 1.0);
}

- (void)doTestRetrieveInterrupt
{
    __block double lastPercentage = 0;
    __block BOOL sanity = YES;
    float interruptPoint = Random.shared.float_0_1;
    double score = [self.database retrieve:^(double percentage, double increment) {
        if (percentage - lastPercentage != increment
            || increment <= 0) {
            TestCaseFailure();
            sanity = NO;
        }
        lastPercentage = percentage;
        return percentage < interruptPoint;
    }];
    TestCaseAssertTrue(sanity);
    TestCaseAssertEqual(score, -1);
    TestCaseAssertTrue(lastPercentage >= interruptPoint && lastPercentage < 1 && lastPercentage < interruptPoint + [self.testClass hasRowid] ? 0.1 : 0.5);
}

#pragma mark - Non-Corrupted
- (void)test_retrieve_with_backup_and_deposit
{
    [self
    executeTest:^{
        [self doBackupWithIncrementalMaterial];
        TestCaseAssertTrue([self.database deposit]);

        [self doTestRetrieve];
        [self doTestObjectsRetrieved];
    }];
}

- (void)test_interrupt_retrieve_with_backup_and_deposit
{
    [self
    executeTest:^{
        [self doBackupWithIncrementalMaterial];
        [self doTestRetrieveInterrupt];
        [self doTestObjectsRetrieved];
    }];
}

- (void)test_retrieve_with_backup_and_without_deposit
{
    __block double mechanicScore = 0;
    __block double crawlerScore = 0;
    __block double summaryScore = 0;

    __block double mechanicCost = 0;
    __block double crawlerCost = 0;
    __block double summaryCost = 0;

    __block double mechanicSpeed = 0;
    __block double crawlerSpeed = 0;

    __block double mechanicSize = 0;
    __block double crawlerSize = 0;

    __block double mechanicWeight = 0;
    __block double crawlerWeight = 0;
    [self.database traceError:^(WCTError* error) {
        if (error.level == WCTErrorLevelNotice
            && error.code == WCTErrorCodeNotice) {
            NSDictionary* userInfo = error.userInfo;
            if ([error.message isEqualToString:@"Mechanic Retrieve Report."]) {
                mechanicCost = ((NSString*) [userInfo objectForKey:@"Cost"]).doubleValue;
                mechanicSize = ((NSString*) [userInfo objectForKey:@"Size"]).doubleValue;
                mechanicSpeed = ((NSString*) [userInfo objectForKey:@"Speed"]).doubleValue;
                mechanicWeight = ((NSString*) [userInfo objectForKey:@"Weight"]).doubleValue;
                mechanicScore = ((NSString*) [userInfo objectForKey:@"Score"]).doubleValue;
            } else if ([error.message isEqualToString:@"Crawler Retrieve Report."]) {
                crawlerCost = ((NSString*) [userInfo objectForKey:@"Cost"]).doubleValue;
                crawlerSize = ((NSString*) [userInfo objectForKey:@"Size"]).doubleValue;
                crawlerSpeed = ((NSString*) [userInfo objectForKey:@"Speed"]).doubleValue;
                crawlerWeight = ((NSString*) [userInfo objectForKey:@"Weight"]).doubleValue;
                crawlerScore = ((NSString*) [userInfo objectForKey:@"Score"]).doubleValue;
            } else if ([error.message isEqualToString:@"Summary Retrieve Report."]) {
                summaryCost = ((NSString*) [userInfo objectForKey:@"Cost"]).doubleValue;
                summaryScore = ((NSString*) [userInfo objectForKey:@"Score"]).doubleValue;
            }
        }
    }];
    [self
    executeTest:^{
        [self doBackupWithIncrementalMaterial];

        [self doTestRetrieve];
        [self doTestObjectsRetrieved];

        TestCaseAssertTrue(mechanicCost > 0);
        TestCaseAssertTrue(mechanicSize > 0);
        TestCaseAssertEqual(mechanicScore, 1);
        TestCaseAssertTrue(mechanicSpeed > 0);
        TestCaseAssertEqual(mechanicWeight, 100); // for 100 percent

        TestCaseAssertTrue(crawlerCost > 0);
        TestCaseAssertTrue(crawlerSize > 0);
        TestCaseAssertEqual(crawlerScore, 1);
        TestCaseAssertTrue(crawlerSpeed > 0);
        TestCaseAssertEqual(crawlerWeight, 100); // for 100 percent

        TestCaseAssertTrue(summaryCost > 0);
        TestCaseAssertEqual(summaryScore, 1);
    }];
}

- (void)test_retrieve_without_backup_and_with_deposit
{
    [self
    executeTest:^{
        TestCaseAssertTrue([self.database deposit]);

        [self doTestRetrieve];
        [self doTestObjectsRetrieved];
    }];
}

- (void)test_retrieve_without_backup_and_deposite
{
    [self
    executeTest:^{
        [self doTestRetrieve];
        [self doTestObjectsRetrieved];
    }];
}

#pragma mark - Corrupted
- (void)test_retrieve_corrupted_with_backup_and_deposit
{
    [self
    executeTest:^{
        [self doBackupWithIncrementalMaterial];

        TestCaseAssertTrue([self.database corruptHeaderWithinCloseAfterTruncatedCheckpoint]);

        TestCaseAssertTrue([self.database deposit]);

        [self doTestRetrieve];

        [self doTestObjectsRetrieved];
    }];
}

- (void)test_retrieve_corrupted_with_backup_and_without_deposit
{
    [self
    executeTest:^{
        [self doBackupWithIncrementalMaterial];

        TestCaseAssertTrue([self.database corruptHeaderWithinCloseAfterTruncatedCheckpoint]);

        [self doTestRetrieve];

        [self doTestObjectsRetrieved];
    }];
}

- (void)test_retrieve_corrupted_without_backup_and_with_deposit
{
    [self
    executeTest:^{
        TestCaseAssertTrue([self.database corruptHeaderWithinCloseAfterTruncatedCheckpoint]);

        TestCaseAssertTrue([self.database deposit]);

        [self doTestRetrieveFailed];

        [self doTestObjectsNotRetrieved];
    }];
}

#pragma mark - Corrupted Schema
- (void)doCorruptSchema
{
    TestCaseAssertTrue([self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::writableSchema()).to(true)]);
    WCTMaster* indexMaster = [[WCTMaster alloc] init];
    indexMaster.type = @"index";
    indexMaster.name = @"unexist_index";
    indexMaster.tblName = @"unexist_table";
    indexMaster.sql = @"CREATE INDEX unexist_index on unexist_table(unexist_column)";
    TestCaseAssertTrue([self.database insertObject:indexMaster intoTable:WCTMaster.tableName]);
    [self.database close];
    TestCaseAssertFalse([self.database canOpen]);
}

- (void)test_retrieve_corrupted_schema
{
    [self executeTest:^{
        TestCaseAssertTrue([self.database backup]);
        [self doCorruptSchema];
        TestCaseAssertTrue([self.database retrieve:nil] > 0);
        [self doTestObjectsRetrieved];
    }];
}

- (void)test_auto_detect_corrupted_schema
{
    [self executeTest:^{
        [self doCorruptSchema];
        __block BOOL detectCorrupted = false;
        [self.database setNotificationWhenCorrupted:^(WCTDatabase* _Nonnull) {
            detectCorrupted = true;
        }];
        TestCaseAssertFalse([self.table insertObject:[[Random shared] repairObjectWithClass:self.testClass andIdentifier:10000]]);
        [NSThread sleepForTimeInterval:1.0];
        TestCaseAssertTrue(detectCorrupted);
        TestCaseAssertTrue([self.database isAlreadyCorrupted]);
        TestCaseAssertTrue([self.database retrieve:nil] > 0);
        [self doTestObjectsRetrieved];
    }];
}

#ifndef WCDB_QUICK_TESTS
- (void)test_backup_huge_database
{
    self.skipDebugLog = YES;
    SizeBasedFactory* factory = [[SizeBasedFactory alloc] initWithDirectory:self.class.cacheRoot];
    factory.quality = 6LL * 1024 * 1024 * 1024; // 6GB > 4GB
    factory.tolerance = 0.02;
    [factory.database enableAutoBackup:YES];

    [factory produce:self.path];

    TestCaseAssertTrue([self.database backup]);

    TestCaseAssertTrue([self.database corruptHeaderWithinCloseAfterTruncatedCheckpoint]);

    __block double percentage = 0;
    TestCaseAssertEqual([self.database retrieve:^(double progress, double increment) {
                            WCDB_UNUSED(increment);
                            double newPercentage = progress * 100.0;
                            if (newPercentage - percentage >= 1.0) {
                                TestCaseLog(@"Retrieving %.2f%%", newPercentage);
                                percentage = newPercentage;
                            }
                            return true;
                        }],
                        1.0);
}

- (void)test_retrieve_huge_database
{
    self.skipDebugLog = YES;
    SizeBasedFactory* factory = [[SizeBasedFactory alloc] initWithDirectory:self.class.cacheRoot];
    factory.quality = 6LL * 1024 * 1024 * 1024; // 6GB > 4GB
    [factory.database enableAutoBackup:YES];
    factory.tolerance = 0.02;

    [factory produce:self.path];

    __block double percentage = 0;
    TestCaseAssertEqual([self.database retrieve:^(double progress, double increment) {
                            WCDB_UNUSED(increment);
                            double newPercentage = progress * 100.0;
                            if (newPercentage - percentage >= 1.0) {
                                TestCaseLog(@"Retrieving %.2f%%", newPercentage);
                                percentage = newPercentage;
                            }
                            return true;
                        }],
                        1.0);
}
#endif

@end
