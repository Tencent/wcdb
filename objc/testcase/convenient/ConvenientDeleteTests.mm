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

#import <TestCase/TestCase.h>

@interface ConvenientDeleteTests : CRUDTestCase

@end

@implementation ConvenientDeleteTests

- (void)setUp
{
    [super setUp];
    [self insertPresetObjects];
}

#pragma mark - Database - Delete
- (void)test_database_delete
{
    [self doTestObjects:@[]
                 andSQL:@"DELETE FROM main.testTable"
      afterModification:^BOOL {
          return [self.database deleteFromTable:self.tableName];
      }];
}

- (void)test_database_delete_where
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable WHERE identifier == 2"
     afterModification:^BOOL {
         return [self.database deleteFromTable:self.tableName where:TestCaseObject.identifier == 2];
     }];
}

- (void)test_database_delete_limit
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable LIMIT 1"
     afterModification:^BOOL {
         return [self.database deleteFromTable:self.tableName limit:1];
     }];
}

- (void)test_database_delete_where_limit
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 LIMIT 1"
     afterModification:^BOOL {
         return [self.database deleteFromTable:self.tableName where:TestCaseObject.identifier > 0 limit:1];
     }];
}

- (void)test_database_delete_orders_limit
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1"
     afterModification:^BOOL {
         return [self.database deleteFromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
     }];
}

- (void)test_database_delete_limit_offset
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [self.database deleteFromTable:self.tableName limit:1 offset:1];
     }];
}

- (void)test_database_delete_where_orders_limit
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
     afterModification:^BOOL {
         return [self.database deleteFromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
     }];
}

- (void)test_database_delete_where_limit_offset
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [self.database deleteFromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
     }];
}

- (void)test_database_delete_orders_limit_offset
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [self.database deleteFromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
     }];
}

- (void)test_database_delete_where_orders_limit_offset
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [self.database deleteFromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
     }];
}

#pragma mark - Table - Delete
- (void)test_table_delete
{
    [self doTestObjects:@[]
                 andSQL:@"DELETE FROM main.testTable"
      afterModification:^BOOL {
          return [self.table deleteObjects];
      }];
}

- (void)test_table_delete_where
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable WHERE identifier == 2"
     afterModification:^BOOL {
         return [self.table deleteObjectsWhere:TestCaseObject.identifier == 2];
     }];
}

- (void)test_table_delete_limit
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable LIMIT 1"
     afterModification:^BOOL {
         return [self.table deleteObjectsLimit:1];
     }];
}

- (void)test_table_delete_where_limit
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 LIMIT 1"
     afterModification:^BOOL {
         return [self.table deleteObjectsWhere:TestCaseObject.identifier > 0 limit:1];
     }];
}

- (void)test_table_delete_orders_limit
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1"
     afterModification:^BOOL {
         return [self.table deleteObjectsOrders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
     }];
}

- (void)test_table_delete_limit_offset
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [self.table deleteObjectsLimit:1 offset:1];
     }];
}

- (void)test_table_delete_where_orders_limit
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
     afterModification:^BOOL {
         return [self.table deleteObjectsWhere:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
     }];
}

- (void)test_table_delete_where_limit_offset
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [self.table deleteObjectsWhere:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
     }];
}

- (void)test_table_delete_orders_limit_offset
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [self.table deleteObjectsOrders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
     }];
}

- (void)test_table_delete_where_orders_limit_offset
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [self.table deleteObjectsWhere:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
     }];
}

#pragma mark - Handle - Delete
- (void)test_handle_delete
{
    [self doTestObjects:@[]
                 andSQL:@"DELETE FROM main.testTable"
      afterModification:^BOOL {
          return [[self.database getHandle] deleteFromTable:self.tableName];
      }];
}

- (void)test_handle_delete_where
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable WHERE identifier == 2"
     afterModification:^BOOL {
         return [[self.database getHandle] deleteFromTable:self.tableName where:TestCaseObject.identifier == 2];
     }];
}

- (void)test_handle_delete_limit
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable LIMIT 1"
     afterModification:^BOOL {
         return [[self.database getHandle] deleteFromTable:self.tableName limit:1];
     }];
}

- (void)test_handle_delete_where_limit
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 LIMIT 1"
     afterModification:^BOOL {
         return [[self.database getHandle] deleteFromTable:self.tableName where:TestCaseObject.identifier > 0 limit:1];
     }];
}

- (void)test_handle_delete_orders_limit
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1"
     afterModification:^BOOL {
         return [[self.database getHandle] deleteFromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
     }];
}

- (void)test_handle_delete_limit_offset
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [[self.database getHandle] deleteFromTable:self.tableName limit:1 offset:1];
     }];
}

- (void)test_handle_delete_where_orders_limit
{
    [self doTestObject:self.object1
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
     afterModification:^BOOL {
         return [[self.database getHandle] deleteFromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
     }];
}

- (void)test_handle_delete_where_limit_offset
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [[self.database getHandle] deleteFromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
     }];
}

- (void)test_handle_delete_orders_limit_offset
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [[self.database getHandle] deleteFromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
     }];
}

- (void)test_handle_delete_where_orders_limit_offset
{
    [self doTestObject:self.object2
                andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
     afterModification:^BOOL {
         return [[self.database getHandle] deleteFromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
     }];
}

@end
