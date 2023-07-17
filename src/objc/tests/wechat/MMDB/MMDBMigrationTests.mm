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

@interface MMDBMigrationTests : MMDBTests
@property (nonatomic, retain) WCTDatabase* sourceDatabase;
@property (nonatomic, retain) NSString* sourcePath;
@end

@implementation MMDBMigrationTests {
    NSString* _sourcePath;
    WCTDatabase* _sourceDatabase;
}

- (NSString*)sourcePath
{
    if (_sourcePath == nil) {
        _sourcePath = [self.path stringByAppendingString:@"_source"];
    }
    return _sourcePath;
}

- (WCTDatabase*)sourceDatabase
{
    if (_sourceDatabase == nil) {
        _sourceDatabase = [[WCTDatabase alloc] initWithPath:self.sourcePath];
    }
    return _sourceDatabase;
}

- (void)setUpDatabase
{
    [self.factory produce:self.sourcePath];
    NSString* tableName = self.factory.tableName;
    NSString* path = self.path;
    NSString* sourceTableName = tableName;
    NSString* sourcePath = self.sourcePath;
    self.tableName = tableName;
    [self.database addMigration:sourcePath
                     withFilter:^(WCTMigrationUserInfo* info) {
                         if ([info.table isEqualToString:tableName]) {
                             info.sourceTable = sourceTableName;
                         }
                     }];

    TestCaseAssertTrue([self.database createTable:self.tableName withClass:DBMessage.class]);
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
    [self.database removeFiles];
}

- (void)test_get_max_time
{
    [self doTestGetMaxTime];
}

@end
