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

@property (nonatomic, readonly) NSString* sourceTable;
@property (nonatomic, readonly) NSString* sourcePath;

@property (nonatomic, readonly) NSString* migratedTable;
@property (nonatomic, readonly) NSString* migratedPath;

@end

@implementation MigrationBenchmark {
    NSString* _sourceTable;
    NSString* _sourcePath;
    NSString* _migratedTable;
    NSString* _migratedPath;
}

- (NSString*)sourceTable
{
    @synchronized(self) {
        if (_sourceTable == nil) {
            _sourceTable = self.tableName;
        }
        return _sourceTable;
    }
}

- (NSString*)sourcePath
{
    @synchronized(self) {
        if (_sourcePath == nil) {
            _sourcePath = self.path;
        }
        return _sourcePath;
    }
}

- (NSString*)migratedTable
{
    @synchronized(self) {
        if (_migratedTable == nil) {
            _migratedTable = [NSString stringWithFormat:@"%@_migrated", self.sourceTable];
            ;
        }
        return _migratedTable;
    }
}

- (NSString*)migratedPath
{
    @synchronized(self) {
        if (_migratedPath == nil) {
            if (self.isCrossDatabase) {
                _migratedPath = [NSString stringWithFormat:@"%@_migrated", self.sourcePath];
            } else {
                _migratedPath = self.sourcePath;
            }
        }
        return _migratedPath;
    }
}

- (void)doSetUpDatabase
{
    NSString* sourceTable = self.sourceTable;
    NSString* sourcePath = self.sourcePath;
    NSString* migratedTable = self.migratedTable;

    self.tableName = self.migratedTable;
    self.path = self.migratedPath;
    [self.database filterMigration:^(WCTMigrationUserInfo* info) {
        if ([info.table isEqualToString:migratedTable]) {
            info.sourceTable = sourceTable;
            info.sourceDatabase = sourcePath;
        }
    }];

    TestCaseAssertTrue([self.database createTable:self.tableName withClass:BenchmarkObject.class]);

    TestCaseAssertTrue([self.database stepMigration]);
    TestCaseAssertFalse([self.database isMigrated]);

    TestCaseAssertTrue([self.database truncateCheckpoint]);

    if (self.isCrossDatabase) {
        WCTDatabase* database = [[WCTDatabase alloc] initWithPath:self.migratedPath];
        TestCaseAssertTrue([database canOpen]);
        TestCaseAssertTrue([database truncateCheckpoint]);
    }
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
