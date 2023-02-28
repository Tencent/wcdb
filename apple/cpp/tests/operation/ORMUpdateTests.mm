//
// Created by qiuwenchen on 2022/9/8.
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

@interface ORMUpdateTests : CPPCRUDTestCase

@property (nonatomic, assign) std::string renewedContent;

@property (nonatomic, assign) CPPTestCaseObject renewObject;
@property (nonatomic, assign) WCDB::OneRowValue renewRow;
@property (nonatomic, assign) WCDB::OneColumnValue renewValue;

@property (nonatomic, assign) CPPTestCaseObject renewedObject1;
@property (nonatomic, assign) CPPTestCaseObject renewedObject2;

@end

@implementation ORMUpdateTests

- (void)setUp
{
    [super setUp];
    [self insertPresetObjects];
    _renewedContent = Random.shared.string.UTF8String;
    _renewObject = CPPTestCaseObject(0, _renewedContent);
    _renewRow = { _renewedContent };
    _renewValue = _renewedContent;
    _renewedObject1 = CPPTestCaseObject(1, _renewedContent);
    _renewedObject2 = CPPTestCaseObject(2, _renewedContent);
}

#pragma mark - Database - Update Properties To Object
- (void)test_database_update_table_set_properties_to_object
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1"
      afterModification:^BOOL {
          return self.database->updateObject<CPPTestCaseObject>(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String);
      }];
}

- (void)test_database_update_table_set_properties_to_object_where
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return self.database->updateObject<CPPTestCaseObject>(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
      }];
}

- (void)test_database_update_table_set_properties_to_object_where_orders_limit
{
    [self doTestObjects:{ self.object1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return self.database->updateObject<CPPTestCaseObject>(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
      }];
}

- (void)test_database_update_table_set_properties_to_object_where_orders_limit_offset
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return self.database->updateObject<CPPTestCaseObject>(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
      }];
}

#pragma mark - Database - Update Properties To Row
- (void)test_database_update_table_set_properties_to_row
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1"
      afterModification:^BOOL {
          return self.database->updateRow(self.renewRow, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String);
      }];
}

- (void)test_database_update_table_set_properties_to_row_where
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return self.database->updateRow(self.renewRow, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
      }];
}

- (void)test_database_update_table_set_properties_to_row_where_orders_limit_offset
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return self.database->updateRow(self.renewRow, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
      }];
}

#pragma mark - Database - Update Property To Value
- (void)test_database_update_table_set_property_to_value
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1"
      afterModification:^BOOL {
          return self.database->updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String);
      }];
}

- (void)test_database_update_table_set_property_to_value_where
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return self.database->updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
      }];
}

- (void)test_database_update_table_set_property_to_value_where_orders_limit
{
    [self doTestObjects:{ self.object1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return self.database->updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
      }];
}

- (void)test_database_update_table_set_property_to_value_where_orders_limit_offset
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return self.database->updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
      }];
}

#pragma mark - Table - Update Properties To Object
- (void)test_table_update_table_set_properties_to_object
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1"
      afterModification:^BOOL {
          return self.table.updateObject(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content));
      }];
}

- (void)test_table_update_table_set_properties_to_object_where
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return self.table.updateObject(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
      }];
}

- (void)test_table_update_table_set_properties_to_object_where_orders_limit
{
    [self doTestObjects:{ self.object1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return self.table.updateObject(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
      }];
}

- (void)test_table_update_table_set_properties_to_object_where_orders_limit_offset
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return self.table.updateObject(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
      }];
}

#pragma mark - Table - Update Properties To Row
- (void)test_table_update_table_set_properties_to_row
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1"
      afterModification:^BOOL {
          return self.table.updateRow(self.renewRow, WCDB_FIELD(CPPTestCaseObject::content));
      }];
}

- (void)test_table_update_table_set_properties_to_row_where
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return self.table.updateRow(self.renewRow, WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
      }];
}

- (void)test_table_update_table_set_properties_to_row_where_orders_limit_offset
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return self.table.updateRow(self.renewRow, WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
      }];
}

#pragma mark - Table - Update Property To Value
- (void)test_table_update_table_set_property_to_value
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1"
      afterModification:^BOOL {
          return self.table.updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content));
      }];
}

- (void)test_table_update_table_set_property_to_value_where
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return self.table.updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
      }];
}

- (void)test_table_update_table_set_property_to_value_where_orders_limit
{
    [self doTestObjects:{ self.object1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return self.table.updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
      }];
}

- (void)test_table_update_table_set_property_to_value_where_orders_limit_offset
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return self.table.updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
      }];
}

#pragma mark - Handle - Update Properties To Object
- (void)test_handle_update_table_set_properties_to_object
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1"
      afterModification:^BOOL {
          return self.database->getHandle().updateObject<CPPTestCaseObject>(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String);
      }];
}

- (void)test_handle_update_table_set_properties_to_object_where
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return self.database->getHandle().updateObject<CPPTestCaseObject>(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
      }];
}

- (void)test_handle_update_table_set_properties_to_object_where_orders_limit
{
    [self doTestObjects:{ self.object1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return self.database->getHandle().updateObject<CPPTestCaseObject>(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
      }];
}

- (void)test_handle_update_table_set_properties_to_object_where_orders_limit_offset
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return self.database->getHandle().updateObject<CPPTestCaseObject>(self.renewObject, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
      }];
}

#pragma mark - Handle - Update Properties To Row
- (void)test_handle_update_table_set_properties_to_row
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1"
      afterModification:^BOOL {
          return self.database->getHandle().updateRow(self.renewRow, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String);
      }];
}

- (void)test_handle_update_table_set_properties_to_row_where
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return self.database->getHandle().updateRow(self.renewRow, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
      }];
}

- (void)test_handle_update_table_set_properties_to_row_where_orders_limit_offset
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return self.database->getHandle().updateRow(self.renewRow, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
      }];
}

#pragma mark - Handle - Update Property To Value
- (void)test_handle_update_table_set_property_to_value
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1"
      afterModification:^BOOL {
          return self.database->getHandle().updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String);
      }];
}

- (void)test_handle_update_table_set_property_to_value_where
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return self.database->getHandle().updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
      }];
}

- (void)test_handle_update_table_set_property_to_value_where_orders_limit
{
    [self doTestObjects:{ self.object1, self.renewedObject2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return self.database->getHandle().updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
      }];
}

- (void)test_handle_update_table_set_property_to_value_where_orders_limit_offset
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
                 andSQL:@"UPDATE testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return self.database->getHandle().updateRow(self.renewValue, WCDB_FIELD(CPPTestCaseObject::content), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
      }];
}

@end
