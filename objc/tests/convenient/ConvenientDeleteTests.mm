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

#import "ConvenientTestCase.h"

@interface ConvenientDeleteTests : ConvenientTestCase

@end

@implementation ConvenientDeleteTests

#pragma mark - Database - Delete
- (void)test_database_delete
{
    BOOL result = [self checkObjects:@[]
                              andSQL:@"DELETE FROM main.testTable"
         asExpectedAfterModification:^BOOL {
             return [self.database deleteFromTable:self.tableName];
         }];
    XCTAssertTrue(result);
}

- (void)test_database_delete_where
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable WHERE identifier == 2"
        asExpectedAfterModification:^BOOL {
            return [self.database deleteFromTable:self.tableName where:ConvenientObject.identifier == 2];
        }];
    XCTAssertTrue(result);
}

- (void)test_database_delete_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [self.database deleteFromTable:self.tableName limit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_database_delete_where_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [self.database deleteFromTable:self.tableName where:ConvenientObject.identifier > 0 limit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_database_delete_orders_limit
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [self.database deleteFromTable:self.tableName orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_database_delete_limit_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [self.database deleteFromTable:self.tableName limit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_database_delete_where_orders_limit
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [self.database deleteFromTable:self.tableName where:ConvenientObject.identifier > 0 orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_database_delete_where_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [self.database deleteFromTable:self.tableName where:ConvenientObject.identifier > 0 orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_database_delete_orders_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [self.database deleteFromTable:self.tableName orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_database_delete_where_orders_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [self.database deleteFromTable:self.tableName where:ConvenientObject.identifier > 0 orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

#pragma mark - Table - Delete
- (void)test_table_delete
{
    BOOL result = [self checkObjects:@[]
                              andSQL:@"DELETE FROM main.testTable"
         asExpectedAfterModification:^BOOL {
             return [self.table deleteObjects];
         }];
    XCTAssertTrue(result);
}

- (void)test_table_delete_where
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable WHERE identifier == 2"
        asExpectedAfterModification:^BOOL {
            return [self.table deleteObjectsWhere:ConvenientObject.identifier == 2];
        }];
    XCTAssertTrue(result);
}

- (void)test_table_delete_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [self.table deleteObjectsLimit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_table_delete_where_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [self.table deleteObjectsWhere:ConvenientObject.identifier > 0 limit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_table_delete_orders_limit
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [self.table deleteObjectsOrders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_table_delete_limit_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [self.table deleteObjectsLimit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_table_delete_where_orders_limit
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [self.table deleteObjectsWhere:ConvenientObject.identifier > 0 orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_table_delete_where_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [self.table deleteObjectsWhere:ConvenientObject.identifier > 0 orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_table_delete_orders_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [self.table deleteObjectsOrders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_table_delete_where_orders_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [self.table deleteObjectsWhere:ConvenientObject.identifier > 0 orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

#pragma mark - Handle - Delete
- (void)test_handle_delete
{
    BOOL result = [self checkObjects:@[]
                              andSQL:@"DELETE FROM main.testTable"
         asExpectedAfterModification:^BOOL {
             return [[self.database getHandle] deleteFromTable:self.tableName];
         }];
    XCTAssertTrue(result);
}

- (void)test_handle_delete_where
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable WHERE identifier == 2"
        asExpectedAfterModification:^BOOL {
            return [[self.database getHandle] deleteFromTable:self.tableName where:ConvenientObject.identifier == 2];
        }];
    XCTAssertTrue(result);
}

- (void)test_handle_delete_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [[self.database getHandle] deleteFromTable:self.tableName limit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_handle_delete_where_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [[self.database getHandle] deleteFromTable:self.tableName where:ConvenientObject.identifier > 0 limit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_handle_delete_orders_limit
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [[self.database getHandle] deleteFromTable:self.tableName orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_handle_delete_limit_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [[self.database getHandle] deleteFromTable:self.tableName limit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_handle_delete_where_orders_limit
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
        asExpectedAfterModification:^BOOL {
            return [[self.database getHandle] deleteFromTable:self.tableName where:ConvenientObject.identifier > 0 orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_handle_delete_where_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [[self.database getHandle] deleteFromTable:self.tableName where:ConvenientObject.identifier > 0 orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_handle_delete_orders_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [[self.database getHandle] deleteFromTable:self.tableName orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

- (void)test_handle_delete_where_orders_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"DELETE FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        asExpectedAfterModification:^BOOL {
            return [[self.database getHandle] deleteFromTable:self.tableName where:ConvenientObject.identifier > 0 orders:ConvenientObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
        }];
    XCTAssertTrue(result);
}

@end
