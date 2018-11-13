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

#import "CRUDTestCase.h"

@interface ConvenientUpdateTests : CRUDTestCase

@property (nonatomic, readonly) TestCaseObject* renewObject;
@property (nonatomic, readonly) WCTColumnCodingRow* renewRow;
@property (nonatomic, readonly) WCTColumnCodingValue* renewValue;

@property (nonatomic, readonly) TestCaseObject* renewedObject1;
@property (nonatomic, readonly) TestCaseObject* renewedObject2;

@end

@implementation ConvenientUpdateTests

- (void)setUp
{
    [super setUp];

    NSString* renewedContent = [NSString randomString];

    _renewObject = [[TestCaseObject alloc] init];
    _renewObject.content = renewedContent;

    _renewedObject1 = [[TestCaseObject alloc] init];
    _renewedObject1.identifier = 1;
    _renewedObject1.content = _renewObject.content;

    _renewedObject2 = [[TestCaseObject alloc] init];
    _renewedObject2.identifier = 2;
    _renewedObject2.content = _renewObject.content;

    _renewRow = @[ renewedContent ];
    _renewValue = renewedContent;
}

#pragma mark - Database - Update Properties To Object
- (void)test_database_update_table_set_properties_to_object
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_object_where
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_object_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_object_where_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1 limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_object_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_object_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_object_where_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_object_where_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_object_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_object_where_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

#pragma mark - Database - Update Properties To Row
- (void)test_database_update_table_set_properties_to_row
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_row_where
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_row_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_row_where_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1 limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_row_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_row_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_row_where_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_row_where_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_row_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_properties_to_row_where_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

#pragma mark - Database - Update Property To Value
- (void)test_database_update_table_set_property_to_value
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_property_to_value_where
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_property_to_value_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_property_to_value_where_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1 limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_property_to_value_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_property_to_value_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_property_to_value_where_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_property_to_value_where_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_property_to_value_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_update_table_set_property_to_value_where_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.database updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

#pragma mark - Table - Update Properties To Object
- (void)test_table_update_properties_to_object
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_object_where
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_object_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_object_where_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1 limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_object_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_object_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_object_where_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_object_where_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_object_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_object_where_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

#pragma mark - Table - Update Properties To Row
- (void)test_table_update_properties_to_row
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_row_where
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_row_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_row_where_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1 limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_row_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_row_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_row_where_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_row_where_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_row_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_properties_to_row_where_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

#pragma mark - Table - Update Property To Value
- (void)test_table_update_property_to_value
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_property_to_value_where
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_property_to_value_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_property_to_value_where_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1 limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_property_to_value_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_property_to_value_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_property_to_value_where_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_property_to_value_where_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_property_to_value_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_update_property_to_value_where_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

#pragma mark - Handle - Update Properties To Object
- (void)test_handle_update_table_set_properties_to_object
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_object_where
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_object_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_object_where_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier == 1 limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_object_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_object_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_object_where_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_object_where_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_object_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_object_where_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toObject:self.renewObject where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

#pragma mark - Handle - Update Properties To Row
- (void)test_handle_update_table_set_properties_to_row
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_row_where
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_row_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_row_where_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier == 1 limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_row_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_row_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_row_where_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_row_where_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_row_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_properties_to_row_where_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperties:TestCaseObject.content toRow:self.renewRow where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

#pragma mark - Handle - Update Property To Value
- (void)test_handle_update_table_set_property_to_value
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_property_to_value_where
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_property_to_value_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_property_to_value_where_limit
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier == 1 LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier == 1 limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_property_to_value_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_property_to_value_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_property_to_value_where_orders_limit
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_property_to_value_where_limit_offset
{
    BOOL result = [self checkObjects:@[ self.object1, self.renewedObject2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_property_to_value_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_update_table_set_property_to_value_where_orders_limit_offset
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                              andSQL:@"UPDATE main.testTable SET content = ?1 WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] updateTable:self.tableName setProperty:TestCaseObject.content toValue:self.renewValue where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
         }];
    XCTAssertTrue(result);
}

@end
