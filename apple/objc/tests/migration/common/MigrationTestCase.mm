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

#import "MigrationTestCase.h"

@implementation MigrationTestCase {
    NSString* _sourcePath;
    NSString* _sourceTable;
    WCTDatabase* _sourceDatabase;
    NSArray<MigrationObject*>* _objects;
}

- (void)setUp
{
    [super setUp];

    _toMigrate = [NSMutableDictionary<NSString*, NSString*> dictionaryWithObject:self.sourceTable forKey:self.tableName];

    TestCaseAssertTrue([self.sourceDatabase execute:[MigrationObject statementForCreatingTable:self.sourceTable withMode:self.mode]]);
    TestCaseAssertTrue([self.sourceDatabase insertObjects:self.objects intoTable:self.sourceTable]);

    [self.sourceDatabase close];

    TestCaseAssertTrue(self.database.isMigrated);

    // It's not a good practice to retain self in this escapable block.
    {
        weakify(self);
        [self.database filterMigration:^(WCTMigrationUserInfo* userInfo) {
            strongify_or_return(self);
            NSString* sourceTable = [self.toMigrate objectForKey:userInfo.table];
            if (sourceTable != nil) {
                userInfo.sourceTable = sourceTable;
                userInfo.sourceDatabase = self.sourcePath;
            }
        }];
    }

    TestCaseAssertFalse(self.database.isMigrated);
    self.tableClass = MigrationObject.class;
    TestCaseAssertTrue([self.database execute:[MigrationObject statementForCreatingTable:self.tableName withMode:self.mode]]);
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
            if (self.isCrossDatabaseMigration) {
                _sourceTable = self.tableName;
            } else {
                _sourceTable = @"testSourceTable";
            }
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

- (NSArray<MigrationObject*>*)objects
{
    @synchronized(self) {
        if (_objects == nil) {
            _objects = [Random.shared migrationObjectsWithCount:10000 startingFromIdentifier:1];
        }
        return _objects;
    }
}

- (void)tearDown
{
    [_sourceDatabase close];
    _sourceDatabase = nil;
    [super tearDown];
}

- (BOOL)isMigrating
{
    return !self.database.isMigrated;
}

@end
