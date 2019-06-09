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

#import "MigrationBenchmark.h"

@interface MigrationBenchmark ()
@property (nonatomic, retain) WCTDatabase* sourceDatabase;
@property (nonatomic, retain) NSString* sourceTable;
@property (nonatomic, retain) NSString* sourcePath;
@end

@implementation MigrationBenchmark

- (void)doSetUpDatabase
{
    self.sourcePath = self.path;
    self.sourceTable = self.tableName;
    self.sourceDatabase = self.database;

    // pointing migrated
    if (self.isCrossDatabase) {
        self.path = [self.sourcePath stringByAppendingString:@"_migrated"];
    } else {
        self.path = self.sourcePath;
    }
    self.tableName = [self.sourceTable stringByAppendingString:@"_migrated"];

    NSString* sourceTable = self.sourceTable;
    NSString* sourcePath = self.sourcePath;
    [self.database filterMigration:^(WCTMigrationUserInfo* info) {
        info.sourceTable = sourceTable;
        info.sourceDatabase = sourcePath;
    }];

    TestCaseAssertTrue([self.database createTable:self.tableName withClass:BenchmarkObject.class]);

    TestCaseAssertTrue([self.database stepMigration]);
    TestCaseAssertFalse([self.database isMigrated]);
    TestCaseAssertTrue([self.database truncateCheckpoint]);
    TestCaseAssertTrue([self.sourceDatabase truncateCheckpoint]);

    [self.sourceDatabase close];
    TestCaseAssertTrue([self.sourceDatabase canOpen]);
}

- (void)doTearDownDatabase
{
    TestCaseAssertTrue([self.database removeFiles]);
    if (self.sourceDatabase != nil) {
        TestCaseAssertTrue([self.sourceDatabase removeFiles]);
    }
    self.path = self.sourcePath;
    self.tableName = self.sourceTable;
}

- (void)doTestMigrate
{
    self.factory.expectedQuality = 100000;
    [self
    doMeasure:^{
        while ([self.database stepMigration] && ![self.database isMigrated])
            ;
    }
    setUp:^{
        [self setUpDatabase];
    }
    tearDown:^{
        [self tearDownDatabase];
    }
    checkCorrectness:^{
        TestCaseAssertTrue([self.database isMigrated]);
    }];
}

@end
