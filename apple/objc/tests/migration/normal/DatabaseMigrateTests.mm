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

#import "MigrateTestCase.h"

@interface DatabaseMigrateTests : MigrateTestCase

@end

@implementation DatabaseMigrateTests

- (void)setUp
{
    self.mode = MigrationObjectORMModeNormal;
    self.isCrossDatabaseMigration = YES;
    [super setUp];
}

- (void)test_filter
{
    [self doTestFilter];
}

- (void)test_step_migrate
{
    [self doTestStepMigrate];
}

- (void)test_migrate
{
    [self doTestMigrate];
}

- (void)test_notification
{
    [self doTestNotification];
}

- (void)test_auto_migrate
{
    [self doTestAutoMigrate];
}

- (void)test_feature_auto_migrate_will_stop_due_to_error
{
    [self doTestFeatureAutoMigrateWillStopDueToError];
}

- (NSArray<NSObject<WCTTableCoding>*>*)getAllObjects
{
    return [self.table getObjectsOrders:MigrationObject.identifier.asOrder(WCTOrderedAscending)];
}

- (void)test_feature_migrate_newly_created_table_after_migrated
{
    [self doTestFeatureMigrateNewlyCreatedTableAfterMigrated];
}

@end
