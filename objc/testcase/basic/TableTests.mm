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

#import "TableTestCase.h"

@interface TableTests : TableTestCase

@end

@implementation TableTests

- (void)setUp
{
    [super setUp];
    self.tableClass = TestCaseObject.class;
}

#pragma mark - Database
- (void)test_database_table_exists
{
    TestCaseAssertTrue([self createTable]);

    TestCaseAssertTrue([self.database tableExists:self.tableName]);

    auto exists = [self.database tableExists:@"nonexists"];
    TestCaseAssertFalse(exists.value());
    TestCaseAssertFalse(exists.failed());
}

- (void)test_database_drop_table
{
    BOOL result = [self checkAllSQLs:@[ @"DROP TABLE IF EXISTS main.testTable" ]
               asExpectedInOperation:^BOOL {
                   return [self.database dropTable:self.tableName];
               }];
    TestCaseAssertTrue(result);

    TestCaseAssertFalse([self.database tableExists:self.tableName]);
}

- (void)test_database_drop_index
{
    BOOL result = [self checkAllSQLs:@[ @"DROP INDEX IF EXISTS main.testTable_index" ]
               asExpectedInOperation:^BOOL {
                   return [self.database dropIndex:@"testTable_index"];
               }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle
- (void)test_handle_table_exists
{
    TestCaseAssertTrue([self createTable]);

    WCTHandle* handle = [self.database getHandle];

    TestCaseAssertTrue([handle tableExists:self.tableName]);

    auto exists = [handle tableExists:@"nonexists"];
    TestCaseAssertFalse(exists.value());
    TestCaseAssertFalse(exists.failed());
}

- (void)test_handle_drop_table
{
    WCTHandle* handle = [self.database getHandle];
    BOOL result = [self checkAllSQLs:@[ @"DROP TABLE IF EXISTS main.testTable" ]
               asExpectedInOperation:^BOOL {
                   return [handle dropTable:self.tableName];
               }];
    TestCaseAssertTrue(result);

    TestCaseAssertFalse([handle tableExists:self.tableName]);
}

- (void)test_handle_drop_index
{
    WCTHandle* handle = [self.database getHandle];
    BOOL result = [self checkAllSQLs:@[ @"DROP INDEX IF EXISTS main.testTable_index" ]
               asExpectedInOperation:^BOOL {
                   return [handle dropIndex:@"testTable_index"];
               }];
    TestCaseAssertTrue(result);
}

@end
