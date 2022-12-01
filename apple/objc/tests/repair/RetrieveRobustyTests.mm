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
#import "TestCase.h"

@interface RetrieveRobustyTests : BackupTestCase

@property (nonatomic, readonly) double expectedAttackRadio;
@property (nonatomic, readonly) NSString* tablePrefix;
@property (nonatomic, assign) NSInteger expectedDatabaseSize;
@property (nonatomic, readonly) double deviationForTolerance;

@end

@implementation RetrieveRobustyTests

- (void)setUp
{
    [super setUp];
    self.skipDebugLog = YES;
    [self.database enableAutoCheckpoint:YES];
}

- (double)expectedAttackRadio
{
    return 0.01;
}

- (NSString*)tablePrefix
{
    return @"t_";
}

- (double)deviationForTolerance
{
    return 0.05;
}

- (BOOL)shouldAttack
{
    return Random.shared.float_0_1 < self.expectedAttackRadio;
}

- (BOOL)isToleranceForRetrieveScore:(double)retrieveScore
                    andObjectsScore:(double)objectsScore
{
    return retrieveScore < objectsScore * (1 + self.deviationForTolerance) && retrieveScore > objectsScore * (1 - self.deviationForTolerance);
}

- (double)getObjectsScoreFromRetrievedTableObjects:(NSDictionary<NSString*, NSArray<TestCaseObject*>*>*)retrievedTableObjects
                           andExpectedTableObjects:(NSDictionary<NSString*, NSArray<TestCaseObject*>*>*)expectedTableObjects
{
    int totalCount = 0;
    int matchedCount = 0;
    for (NSString* expectedTableName in expectedTableObjects.allKeys) {
        NSArray<TestCaseObject*>* expectedObjects = [expectedTableObjects objectForKey:expectedTableName];
        NSSet<TestCaseObject*>* expectedObjectsSet = [NSSet setWithArray:expectedObjects];
        TestCaseAssertTrue(expectedObjectsSet.count == expectedObjects.count);
        totalCount += expectedObjectsSet.count;

        NSArray<TestCaseObject*>* retrievedObjects = [retrievedTableObjects objectForKey:expectedTableName];
        if (retrievedObjects.count > 0) {
            for (TestCaseObject* retrievedObject in retrievedObjects) {
                if ([expectedObjectsSet containsObject:retrievedObject]) {
                    ++matchedCount;
                }
            }
        }
    }
    return (double) matchedCount / totalCount;
}

- (BOOL)fillDatabaseUntilMeetExpectedSize
{
    NSString* currentTable = nil;
    BOOL checkpointed = NO; // leave wal exists
    while (checkpointed || [self.database getFilesSize] < self.expectedDatabaseSize) {
        if (currentTable == nil || Random.shared.uint8 % 10 == 0) {
            currentTable = [Random.shared tableNameWithPrefix:self.tablePrefix];
            if (![self.database createTable:currentTable withClass:TestCaseObject.class]) {
                TestCaseFailure();
                return NO;
            }
        }

        int count = 0;
        do {
            count = Random.shared.uint8 * 10;
        } while (count == 0);
        NSArray<TestCaseObject*>* objects = [Random.shared autoIncrementTestCaseObjectsWithCount:count];
        if (![self.database insertObjects:objects intoTable:currentTable]) {
            TestCaseFailure();
            return NO;
        }
        if (Random.shared.uint8 % 100 == 0) {
            if (![self.database truncateCheckpoint]) {
                TestCaseFailure();
                return NO;
            }
            checkpointed = YES;
        } else {
            checkpointed = NO;
        }
    }
    if (![self.fileManager fileExistsAtPath:self.database.walPath]) {
        TestCaseFailure();
        return NO;
    }
    return YES;
}

- (NSDictionary<NSString*, NSArray<TestCaseObject*>*>*)getTableObjects
{
    NSMutableDictionary<NSString*, NSArray<TestCaseObject*>*>* tableObjects = [NSMutableDictionary<NSString*, NSArray<TestCaseObject*>*> dictionary];
    {
        // get all objects
        NSString* likeExpressions = [NSString stringWithFormat:@"%@%%", self.tablePrefix];
        NSArray* tableNames = [self.database getColumnFromStatement:WCDB::StatementSelect().select(WCTMaster.name).from(WCTMaster.tableName).where(WCTMaster.name.like(likeExpressions))];
        if (tableNames.count == 0) {
            TestCaseFailure();
            return nil;
        }
        for (NSString* tableName in tableNames) {
            NSArray<TestCaseObject*>* objects = [self.database getObjectsOfClass:TestCaseObject.class fromTable:tableName];
            if (objects != nil) {
                [tableObjects setObject:objects forKey:tableName];
            }
        }
    }
    return tableObjects;
}

- (double)pageBasedAttackAsExpectedRadio
{
    __block double attackedRatio = 0;

    // make database corrupted
    [self.database close:^{
        int totalNumberOfPages = 0;
        int totalNumberOfAttackedPages = 0;
        {
            // database
            auto optionalNumberOfPages = [self.database getNumberOfPages];
            TestCaseAssertFalse(optionalNumberOfPages.failed());
            int numberOfPages = (int) optionalNumberOfPages.value();
            totalNumberOfPages += numberOfPages;
            for (int i = 0; i < numberOfPages; ++i) {
                if ([self shouldAttack]) {
                    [self.database corruptPage:i + 1];
                    ++totalNumberOfAttackedPages;
                }
            }
        }

        {
            // wal
            WCTOptionalSize optionalNumberOfFrames = [self.database getNumberOfWalFrames];
            TestCaseAssertFalse(optionalNumberOfFrames.failed());
            int numberOfFrames = (int) optionalNumberOfFrames.value();
            totalNumberOfPages += numberOfFrames;
            for (int i = 0; i < numberOfFrames; ++i) {
                if ([self shouldAttack]) {
                    [self.database corruptWalFrame:i + 1];
                    ++totalNumberOfAttackedPages;
                }
            }
        }
        attackedRatio = (float) totalNumberOfAttackedPages / totalNumberOfPages;
    }];
    return attackedRatio;
}

- (void)test_feature_page_based_robusty
{
    NSArray* sizes = @[
        @(10 * 1024 * 1024),
        @(100 * 1024 * 1024),
#ifndef WCDB_QUICK_TESTS
        @(1024 * 1024 * 1024),
#endif
    ];
    for (NSNumber* size in sizes) {
        self.expectedDatabaseSize = size.integerValue;
        [self.database removeFiles];
        [self
        excuteTest:^{
            TestCaseAssertTrue([self fillDatabaseUntilMeetExpectedSize]);

            NSDictionary<NSString*, NSArray<TestCaseObject*>*>* expectedTableObjects = [self getTableObjects];
            TestCaseAssertTrue(expectedTableObjects != nil);

            TestCaseAssertTrue([self.database backup]);

            double attackedRadio = [self pageBasedAttackAsExpectedRadio];

            double retrievedScore = [self.database retrieve:nullptr];

            NSDictionary<NSString*, NSArray<TestCaseObject*>*>* retrievedTableObjects = [self getTableObjects];
            TestCaseAssertTrue(retrievedTableObjects != nil);

            double objectsScore = [self getObjectsScoreFromRetrievedTableObjects:retrievedTableObjects andExpectedTableObjects:expectedTableObjects];

            TestCaseLog(@"Radio: attacked: %.8f, expected %.8f", attackedRadio, self.expectedAttackRadio);
            TestCaseLog(@"Score: retrieve: %.8f, objects: %.8f", retrievedScore, objectsScore);

            TestCaseAssertTrue([self isToleranceForRetrieveScore:retrievedScore andObjectsScore:objectsScore]);
        }];
    }
}

@end
