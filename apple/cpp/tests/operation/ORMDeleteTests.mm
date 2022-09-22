//
// Created by 陈秋文 on 2022/9/7.
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

#import "CPPTestCase.h"

@interface ORMDeleteTests : CPPCRUDTestCase

@end

@implementation ORMDeleteTests

- (void)setUp
{
    [super setUp];
    [self insertPresetObjects];
}

#pragma mark - Database - Delete

- (void)test_database_delete
{
    [self doTestObjects:{}
    andSQL:@"DELETE FROM main.testTable"
    afterModification:^BOOL {
        return self.database->deleteObjects(self.tableName.UTF8String);
    }];
}

- (void)test_database_delete_where
{
    [self doTestObjects:self.object1
                 andSQL:@"DELETE FROM main.testTable WHERE identifier == 2"
      afterModification:^BOOL {
          return self.database->deleteObjects(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 2);
      }];
}

- (void)test_database_delete_where_order_limit
{
    [self doTestObjects:self.object1
                 andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return self.database->deleteObjects(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
      }];
}

#pragma mark - Table - Delete
- (void)test_table_delete
{
    [self doTestObjects:{}
    andSQL:@"DELETE FROM main.testTable"
    afterModification:^BOOL {
        return self.table.deleteObjects();
    }];
}

- (void)test_table_delete_where
{
    [self doTestObjects:self.object1
                 andSQL:@"DELETE FROM main.testTable WHERE identifier == 2"
      afterModification:^BOOL {
          return self.table.deleteObjects(WCDB_FIELD(CPPTestCaseObject::identifier) == 2);
      }];
}

- (void)test_table_delete_where_order_limit
{
    [self doTestObjects:self.object1
                 andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return self.table.deleteObjects(WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
      }];
}

@end
