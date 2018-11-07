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

#import "WINQTestCase.h"
#import <WCDB/WCDB.h>

@interface StatementRollbackTests : WINQTestCase

@end

@implementation StatementRollbackTests {
    NSString* savepoint;
}

- (void)setUp
{
    [super setUp];
    savepoint = @"testSavepoint";
}

- (void)test_default_constructible
{
    WCDB::StatementRollback constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::StatementRollback().getType(), WCDB::SQL::Type::RollbackSTMT);
    XCTAssertEqual(WCDB::StatementRollback::type, WCDB::SQL::Type::RollbackSTMT);
}

- (void)test_rollback
{
    auto testingSQL = WCDB::StatementRollback().rollback();

    auto testingTypes = { WCDB::SQL::Type::RollbackSTMT };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"ROLLBACK");
}

- (void)test_rollback_savepoint
{
    auto testingSQL = WCDB::StatementRollback().rollbackToSavepoint(savepoint);

    auto testingTypes = { WCDB::SQL::Type::RollbackSTMT };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"ROLLBACK TO testSavepoint");
}

@end
