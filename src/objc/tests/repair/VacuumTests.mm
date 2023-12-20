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
#import "Random+RepairTestObject.h"
#import "SizeBasedFactory.h"
#import <Foundation/Foundation.h>

@interface VacuumTests : BackupTestCase

@end

@implementation VacuumTests

- (void)test_empty
{
    [self
    executeTest:^{
        self.path = [self.directory stringByAppendingPathComponent:@"empty"];
        TestCaseAssertTrue([self.database vaccum:nil]);
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
    bool succeed = [self.database vaccum:^(double percentage, double increment) {
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
    bool succeed = [self.database vaccum:^(double percentage, double increment) {
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
    TestCaseAssertTrue(![self.testClass hasRowid] || (lastPercentage >= interruptPoint && lastPercentage < 1 && lastPercentage < interruptPoint + 0.2));
}

- (void)doTestVacuumFailed
{
    __block double lastPercentage = 0;
    __block BOOL sanity = YES;
    bool succeed = [self.database vaccum:^(double percentage, double increment) {
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
- (void)test_vaccum
{
    [self
    executeTest:^{
        [self doTestVacuum];
        [self doTestObjectsExist];
        [self doTestFactoryNotExist];
    }];
}

#pragma mark - Non-Corrupted
- (void)test_interrupt_vaccum
{
    [self
    executeTest:^{
        [self doTestInterruptVacuum];
        [self doTestObjectsExist];
        [self doTestFactoryNotExist];
    }];
}

#pragma mark - Corrupted
- (void)test_vaccum_corrupted
{
    [self
    executeTest:^{
        TestCaseAssertTrue([self.database corruptHeaderWithinCloseAfterTruncatedCheckpoint]);
        [self doTestVacuumFailed];
        [self doTestObjectsNoExist];
        [self doTestFactoryNotExist];
    }];
}

- (void)test_vaccum_corrupt_opened
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

#ifndef WCDB_QUICK_TESTS
- (void)test_vaccum_huge_database
{
    self.skipDebugLog = YES;
    SizeBasedFactory* factory = [[SizeBasedFactory alloc] initWithDirectory:self.class.cacheRoot];
    factory.quality = 6LL * 1024 * 1024 * 1024; // 6GB > 4GB
    factory.tolerance = 0.02;

    [factory produce:self.path];

    __block double percentage = 0;
    TestCaseAssertTrue([self.database vaccum:^(double progress, double increment) {
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
