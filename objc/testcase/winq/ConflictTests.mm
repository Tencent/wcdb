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

#import "WINQAssertion.h"

@interface ConflictTests : BaseTestCase

@end

@implementation ConflictTests {
    NSString* name;
}

- (void)setUp
{
    [super setUp];
    name = @"testColumnConstraint";
}

- (void)test_default_constructible
{
    WCDB::Conflict constructible __attribute__((unused));
}

- (void)test_conflict_abort
{
    auto testingSQL = WCDB::ColumnConstraint(name).primaryKey().conflict(WCDB::Conflict::Abort);
    
    TestCaseAssertSQLEqual(testingSQL, @"CONSTRAINT testColumnConstraint PRIMARY KEY ON CONFLICT ABORT");
}

- (void)test_conflict_fail
{
    auto testingSQL = WCDB::ColumnConstraint(name).primaryKey().conflict(WCDB::Conflict::Fail);
    
    TestCaseAssertSQLEqual(testingSQL, @"CONSTRAINT testColumnConstraint PRIMARY KEY ON CONFLICT FAIL");
}

- (void)test_conflict_rollback
{
    auto testingSQL = WCDB::ColumnConstraint(name).primaryKey().conflict(WCDB::Conflict::Rollback);
    
    TestCaseAssertSQLEqual(testingSQL, @"CONSTRAINT testColumnConstraint PRIMARY KEY ON CONFLICT ROLLBACK");
}

- (void)test_conflict_ignore
{
    auto testingSQL = WCDB::ColumnConstraint(name).primaryKey().conflict(WCDB::Conflict::Ignore);
    
    TestCaseAssertSQLEqual(testingSQL, @"CONSTRAINT testColumnConstraint PRIMARY KEY ON CONFLICT IGNORE");
}

- (void)test_conflict_replace
{
    auto testingSQL = WCDB::ColumnConstraint(name).primaryKey().conflict(WCDB::Conflict::Replace);
    
    TestCaseAssertSQLEqual(testingSQL, @"CONSTRAINT testColumnConstraint PRIMARY KEY ON CONFLICT REPLACE");
}

@end
