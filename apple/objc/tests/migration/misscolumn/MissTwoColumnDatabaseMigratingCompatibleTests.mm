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

#import "MigrationCompatibleTestCase.h"

@interface MissTwoColumnDatabaseMigratingCompatibleTests : MigrationCompatibleTestCase

@end

@implementation MissTwoColumnDatabaseMigratingCompatibleTests

- (void)setUp
{
    self.mode = MigrationObjectORMModeMissTwoColumn;
    self.isCrossDatabaseMigration = YES;
    [super setUp];

    TestCaseAssertTrue([self.database stepMigration]);
    TestCaseAssertFalse([self.database isMigrated]);
}

- (void)test_insert
{
    [self doTestInsert];
}

- (void)test_limited_delete
{
    [self doTestLimitedDelete];
}

- (void)test_limited_update
{
    [self doTestLimitedUpdate];
}

- (void)test_select
{
    [self doTestSelect];
}

- (void)test_drop_table
{
    [self doTestDropTable];
}

- (void)test_subquery_within_delete
{
    [self doTestSubqueryWithinDelete];
}

- (void)test_subquery_within_update
{
    [self doTestSubqueryWithinUpdate];
}

- (NSArray<NSObject<WCTTableCoding>*>*)getAllObjects
{
    return [self.table getObjectsOrders:MigrationObject.identifier.asOrder(WCTOrderedAscending)];
}

@end
