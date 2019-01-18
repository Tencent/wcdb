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

#import "MigrationTestCase.h"

@implementation MigrationTestCase {
    NSString* _sourcePath;
    NSString* _sourceTable;
    WCTDatabase* _sourceDatabase;
    NSArray<TestCaseObject*>* _objects;
}

- (void)setUp
{
    [super setUp];

    TestCaseAssertTrue([self.sourceDatabase createTableAndIndexes:self.sourceTable withClass:TestCaseObject.class]);
    TestCaseAssertTrue([self.sourceDatabase insertObjects:self.objects intoTable:self.sourceTable]);

    [self.sourceDatabase close];

    NSString* sourceTable = self.sourceTable;
    NSString* sourcePath = self.sourcePath;
    NSString* table = self.tableName;
    [self.database removeConfigForName:WCTConfigNameCheckpoint];
    [self.database filterMigration:^(WCTMigrationUserInfo* userInfo) {
        if ([userInfo.table isEqualToString:table]) {
            userInfo.sourceTable = sourceTable;
            userInfo.sourceDatabase = sourcePath;
        }
    }];
    TestCaseAssertTrue([self createTable]);
    [self.database close];
}

- (NSString*)sourcePath
{
    @synchronized(self) {
        if (_sourcePath == nil) {
            if (self.isCrossDatabaseMigration) {
                _sourcePath = [self.path stringByAppendingString:@"_source"];
            } else {
                _sourcePath = self.path;
            }
        }
        return _sourcePath;
    }
}

- (NSString*)sourceTable
{
    @synchronized(self) {
        if (_sourceTable == nil) {
            _sourceTable = @"testSourceTable";
        }
        return _sourceTable;
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

- (NSArray<TestCaseObject*>*)objects
{
    @synchronized(self) {
        if (_objects == nil) {
            _objects = [self.random testCaseObjectsWithCount:100 startingFromIdentifier:1];
        }
        return _objects;
    }
}

- (void)tearDown
{
    if (_sourceDatabase.isValidated) {
        [_sourceDatabase close];
        [_sourceDatabase invalidate];
    }
    _sourceDatabase = nil;
    [super tearDown];
}

- (BOOL)isMigrating
{
    int count = [self.database getValueFromStatement:WCDB::StatementSelect().select(TestCaseObject.allProperties.count()).from(self.tableName)].numberValue.intValue;
    return count > 0 && count < self.objects.count;
}

@end
