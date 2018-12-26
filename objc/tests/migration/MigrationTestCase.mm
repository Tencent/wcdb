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

@implementation MigrationTestCase

- (void)setUp
{
    [super setUp];

    self.tableClass = TestCaseObject.class;

    if (self.isCrossDatabaseMigration) {
        _sourcePath = [self.path stringByAppendingString:@"_source"];
    } else {
        _sourcePath = self.path;
    }
    _sourceDatabase = [[WCTDatabase alloc] initWithPath:_sourcePath];
    _sourceTable = @"testSourceTable";

    TestCaseAssertTrue([_sourceDatabase createTableAndIndexes:_sourceTable withClass:TestCaseObject.class]);
    NSMutableArray<TestCaseObject*>* objects = [NSMutableArray array];
    for (int i = 0; i < 100; ++i) {
        TestCaseObject* object = [[TestCaseObject alloc] init];
        object.identifier = i + 1;
        object.content = [NSString randomString];
        [objects addObject:object];
    }
    _objects = objects;
    TestCaseAssertTrue([_sourceDatabase insertObjects:_objects intoTable:_sourceTable]);

    [_sourceDatabase close];

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
