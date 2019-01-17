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

@interface ConvenientUpdateTests : CRUDTestCase

@property (nonatomic, readonly) NSString* renewedContent;

@property (nonatomic, readonly) TestCaseObject* renewObject;
@property (nonatomic, readonly) WCTColumnCodingRow* renewRow;
@property (nonatomic, readonly) WCTColumnCodingValue* renewValue;

@property (nonatomic, readonly) TestCaseObject* renewedObject1;
@property (nonatomic, readonly) TestCaseObject* renewedObject2;

@end

@implementation ConvenientUpdateTests {
    NSString* _renewedContent;

    TestCaseObject* _renewObject;
    WCTColumnCodingRow* _renewRow;
    WCTColumnCodingValue* _renewValue;

    TestCaseObject* _renewedObject1;
    TestCaseObject* _renewedObject2;
}

- (void)setUp
{
    [super setUp];

    [self insertPresetObjects];
}

- (NSString*)renewedContent
{
    @synchronized(self) {
        if (_renewedContent == nil) {
            _renewedContent = self.random.string;
        }
        return _renewedContent;
    }
}

- (TestCaseObject*)renewObject
{
    @synchronized(self) {
        if (_renewObject == nil) {
            TestCaseObject* object = [[TestCaseObject alloc] init];
            object.content = self.renewedContent;
            _renewObject = object;
        }
        return _renewObject;
    }
}

- (WCTColumnCodingRow*)renewRow
{
    @synchronized(self) {
        if (_renewRow == nil) {
            _renewRow = @[ self.renewedContent ];
        }
        return _renewRow;
    }
}

- (WCTColumnCodingValue*)renewValue
{
    @synchronized(self) {
        if (_renewValue == nil) {
            _renewValue = self.renewedContent;
        }
        return _renewValue;
    }
}

- (TestCaseObject*)renewedObject1
{
    @synchronized(self) {
        if (_renewedObject1 == nil) {
            TestCaseObject* object = [[TestCaseObject alloc] init];
            object.identifier = 1;
            object.content = self.renewedContent;
            _renewedObject1 = object;
        }
        return _renewedObject1;
    }
}

- (TestCaseObject*)renewedObject2
{
    @synchronized(self) {
        if (_renewedObject2 == nil) {
            TestCaseObject* object = [[TestCaseObject alloc] init];
            object.identifier = 2;
            object.content = self.renewedContent;
            _renewedObject2 = object;
        }
        return _renewedObject2;
    }
}

#pragma mark - Database - Update Properties To Object
- (void)test_database_update_table_set_properties_to_object
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject];
      }];
}

- (void)test_database_update_table_set_properties_to_object_where
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1];
      }];
}

- (void)test_database_update_table_set_properties_to_object_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject limit:1];
      }];
}

- (void)test_database_update_table_set_properties_to_object_where_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1 limit:1];
      }];
}

- (void)test_database_update_table_set_properties_to_object_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_database_update_table_set_properties_to_object_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject limit:1 offset:1];
      }];
}

- (void)test_database_update_table_set_properties_to_object_where_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_database_update_table_set_properties_to_object_where_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 limit:1 offset:1];
      }];
}

- (void)test_database_update_table_set_properties_to_object_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

- (void)test_database_update_table_set_properties_to_object_where_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

#pragma mark - Database - Update Properties To Row
- (void)test_database_update_table_set_properties_to_row
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow];
      }];
}

- (void)test_database_update_table_set_properties_to_row_where
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1];
      }];
}

- (void)test_database_update_table_set_properties_to_row_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow limit:1];
      }];
}

- (void)test_database_update_table_set_properties_to_row_where_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1 limit:1];
      }];
}

- (void)test_database_update_table_set_properties_to_row_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_database_update_table_set_properties_to_row_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow limit:1 offset:1];
      }];
}

- (void)test_database_update_table_set_properties_to_row_where_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_database_update_table_set_properties_to_row_where_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 limit:1 offset:1];
      }];
}

- (void)test_database_update_table_set_properties_to_row_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

- (void)test_database_update_table_set_properties_to_row_where_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

#pragma mark - Database - Update Property To Value
- (void)test_database_update_table_set_property_to_value
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue];
      }];
}

- (void)test_database_update_table_set_property_to_value_where
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1];
      }];
}

- (void)test_database_update_table_set_property_to_value_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue limit:1];
      }];
}

- (void)test_database_update_table_set_property_to_value_where_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1 limit:1];
      }];
}

- (void)test_database_update_table_set_property_to_value_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_database_update_table_set_property_to_value_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue limit:1 offset:1];
      }];
}

- (void)test_database_update_table_set_property_to_value_where_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_database_update_table_set_property_to_value_where_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 limit:1 offset:1];
      }];
}

- (void)test_database_update_table_set_property_to_value_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

- (void)test_database_update_table_set_property_to_value_where_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

#pragma mark - Table - Update Properties To Object
- (void)test_table_update_properties_to_object
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject];
      }];
}

- (void)test_table_update_properties_to_object_where
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1];
      }];
}

- (void)test_table_update_properties_to_object_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject limit:1];
      }];
}

- (void)test_table_update_properties_to_object_where_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1 limit:1];
      }];
}

- (void)test_table_update_properties_to_object_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_table_update_properties_to_object_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject limit:1 offset:1];
      }];
}

- (void)test_table_update_properties_to_object_where_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_table_update_properties_to_object_where_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 limit:1 offset:1];
      }];
}

- (void)test_table_update_properties_to_object_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

- (void)test_table_update_properties_to_object_where_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

#pragma mark - Table - Update Properties To Row
- (void)test_table_update_properties_to_row
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow];
      }];
}

- (void)test_table_update_properties_to_row_where
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1];
      }];
}

- (void)test_table_update_properties_to_row_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow limit:1];
      }];
}

- (void)test_table_update_properties_to_row_where_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1 limit:1];
      }];
}

- (void)test_table_update_properties_to_row_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_table_update_properties_to_row_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow limit:1 offset:1];
      }];
}

- (void)test_table_update_properties_to_row_where_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_table_update_properties_to_row_where_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 limit:1 offset:1];
      }];
}

- (void)test_table_update_properties_to_row_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

- (void)test_table_update_properties_to_row_where_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

#pragma mark - Table - Update Property To Value
- (void)test_table_update_property_to_value
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1"
      afterModification:^BOOL {
          return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue];
      }];
}

- (void)test_table_update_property_to_value_where
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1];
      }];
}

- (void)test_table_update_property_to_value_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue limit:1];
      }];
}

- (void)test_table_update_property_to_value_where_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1 limit:1];
      }];
}

- (void)test_table_update_property_to_value_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_table_update_property_to_value_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue limit:1 offset:1];
      }];
}

- (void)test_table_update_property_to_value_where_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_table_update_property_to_value_where_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 limit:1 offset:1];
      }];
}

- (void)test_table_update_property_to_value_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

- (void)test_table_update_property_to_value_where_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

#pragma mark - Handle - Update Properties To Object
- (void)test_handle_update_table_set_properties_to_object
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject];
      }];
}

- (void)test_handle_update_table_set_properties_to_object_where
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1];
      }];
}

- (void)test_handle_update_table_set_properties_to_object_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject limit:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_object_where_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1 limit:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_object_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_object_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject limit:1 offset:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_object_where_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_object_where_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 limit:1 offset:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_object_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_object_where_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

#pragma mark - Handle - Update Properties To Row
- (void)test_handle_update_table_set_properties_to_row
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow];
      }];
}

- (void)test_handle_update_table_set_properties_to_row_where
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1];
      }];
}

- (void)test_handle_update_table_set_properties_to_row_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow limit:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_row_where_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1 limit:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_row_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_row_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow limit:1 offset:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_row_where_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_row_where_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 limit:1 offset:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_row_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

- (void)test_handle_update_table_set_properties_to_row_where_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

#pragma mark - Handle - Update Property To Value
- (void)test_handle_update_table_set_property_to_value
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue];
      }];
}

- (void)test_handle_update_table_set_property_to_value_where
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1];
      }];
}

- (void)test_handle_update_table_set_property_to_value_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue limit:1];
      }];
}

- (void)test_handle_update_table_set_property_to_value_where_limit
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1 limit:1];
      }];
}

- (void)test_handle_update_table_set_property_to_value_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_handle_update_table_set_property_to_value_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue limit:1 offset:1];
      }];
}

- (void)test_handle_update_table_set_property_to_value_where_orders_limit
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
      }];
}

- (void)test_handle_update_table_set_property_to_value_where_limit_offset
{
    [self doTestObjects:@[ self.object1, self.renewedObject2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 limit:1 offset:1];
      }];
}

- (void)test_handle_update_table_set_property_to_value_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

- (void)test_handle_update_table_set_property_to_value_where_orders_limit_offset
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
                 andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
      afterModification:^BOOL {
          return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
      }];
}

@end
