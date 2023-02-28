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

#import "MigrationBenchmark.h"

@interface MigrationBenchmark ()
@property (nonatomic, retain) WCTDatabase* sourceDatabase;
@property (nonatomic, retain) NSString* sourcePath;
@end

@implementation MigrationBenchmark {
    WCTDatabase* _sourceDatabase;
    NSString* _sourcePath;
}

- (void)setSourcePath:(NSString*)sourcePath
{
    _sourcePath = sourcePath;
    _sourceDatabase = nil;
}

- (NSString*)sourcePath
{
    @synchronized(self) {
        if (_sourcePath == nil) {
            if (self.isCrossDatabase) {
                _sourcePath = [self.path stringByAppendingString:@"_source"];
            } else {
                _sourcePath = self.path;
            }
        }
        return _sourcePath;
    }
}

- (WCTDatabase*)sourceDatabase
{
    @synchronized(self) {
        if (_sourceDatabase == nil) {
            _sourceDatabase = [[WCTDatabase alloc] initWithPath:self.sourcePath];
        }
        return _sourceDatabase;
    }
}

- (void)setUpDatabase
{
    [self.factory produce:self.sourcePath];
    self.tableName = [self.factory.tableName stringByAppendingString:@"_migrated"];

    NSString* tableName = self.tableName;
    NSString* path = self.path;
    NSString* sourceTableName = self.factory.tableName;
    NSString* sourcePath = self.sourcePath;
    [self.database filterMigration:^(WCTMigrationUserInfo* info) {
        if ([info.table isEqualToString:tableName]
            && [info.database isEqualToString:path]) {
            info.sourceTable = sourceTableName;
            info.sourceDatabase = sourcePath;
        }
    }];
    TestCaseAssertTrue([self.database createTable:self.tableName withClass:TestCaseObject.class]);
    TestCaseAssertTrue([self.database stepMigration]);
    TestCaseAssertFalse([self.database isMigrated]);
    TestCaseAssertTrue([self.database truncateCheckpoint]);
    TestCaseAssertTrue([self.sourceDatabase truncateCheckpoint]);
    [self.database close];

    TestCaseAssertOptionalEqual([self.database getNumberOfWalFrames], 0);
    TestCaseAssertOptionalEqual([self.sourceDatabase getNumberOfWalFrames], 0);
    TestCaseAssertFalse([self.database isOpened]);
}

- (void)tearDownDatabase
{
    TestCaseAssertTrue([self.database removeFiles]);
    TestCaseAssertTrue([self.sourceDatabase removeFiles]);
}

- (void)doTestMigrate
{
    self.factory.quality = 100000;
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
