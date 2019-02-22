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

@property (nonatomic, retain) NSString* sourceTable;
@property (nonatomic, retain) NSString* sourcePath;

@property (nonatomic, retain) NSString* migratedTable;
@property (nonatomic, retain) NSString* migratedPath;

@end

@implementation MigrationBenchmark

- (void)doSetUpDatabase
{
    self.sourcePath = self.path;
    self.sourceTable = self.tableName;
    self.migratedTable = [NSString stringWithFormat:@"%@_migrated", self.sourceTable];
    if (self.isCrossDatabase) {
        self.migratedPath = [NSString stringWithFormat:@"%@_migrated", self.sourcePath];
    } else {
        self.migratedPath = self.sourcePath;
    }

    self.tableName = self.migratedTable;
    self.path = self.migratedPath;

    NSString* sourceTable = self.sourceTable;
    NSString* sourcePath = self.sourcePath;
    NSString* migratedTable = self.migratedTable;
    [self.database filterMigration:^(WCTMigrationUserInfo* info) {
        if ([info.table isEqualToString:migratedTable]) {
            info.sourceTable = sourceTable;
            info.sourceDatabase = sourcePath;
        }
    }];

    TestCaseAssertTrue([self.database createTable:self.tableName withClass:BenchmarkObject.class]);

    BOOL done;
    TestCaseAssertTrue([self.database stepMigrationOrDone:done]);
    TestCaseAssertFalse(done);
}

- (void)doTearDownDatabase
{
    self.tableName = self.sourceTable;
    self.path = self.sourcePath;

    if (self.isCrossDatabase) {
        TestCaseAssertTrue([[[WCTDatabase alloc] initWithPath:self.migratedPath] removeFiles]);
    }
}

- (void)doTestMigrate
{
    self.factory.expectedQuality = 100000;
    __block BOOL done;
    [self
    doMeasure:^{
        while ([self.database stepMigrationOrDone:done] && !done)
            ;
    }
    setUp:^{
        [self setUpDatabase];
    }
    tearDown:^{
        [self tearDownDatabase];
        done = NO;
    }
    checkCorrectness:^{
        TestCaseAssertTrue(done);
    }];
}

@end
