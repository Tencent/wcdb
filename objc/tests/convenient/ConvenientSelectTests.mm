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

@interface ConvenientSelectTests : CRUDTestCase

@property (nonatomic, readonly) TestCaseObject *partialObject1;
@property (nonatomic, readonly) TestCaseObject *partialObject2;
@property (nonatomic, readonly) NSArray<TestCaseObject *> *partialObjects;

@property (nonatomic, readonly) WCTOneRow *partialRow1;
@property (nonatomic, readonly) WCTOneRow *partialRow2;
@property (nonatomic, readonly) WCTColumnsXRows *partialRows;

@property (nonatomic, readonly) WCTValue *value1;
@property (nonatomic, readonly) WCTValue *value2;
@property (nonatomic, readonly) WCTOneColumn *column;

@end

@implementation ConvenientSelectTests

- (void)setUp
{
    [super setUp];

    _partialObject1 = [[TestCaseObject alloc] init];
    _partialObject1.identifier = 1;
    _partialObject1.content = nil;

    _partialObject2 = [[TestCaseObject alloc] init];
    _partialObject2.identifier = 2;
    _partialObject2.content = nil;

    _partialObjects = @[ _partialObject1, _partialObject2 ];

    _partialRow1 = @[ @(1) ];
    _partialRow2 = @[ @(2) ];
    _partialRows = @[ _partialRow1, _partialRow2 ];

    _value1 = @(1);
    _value2 = @(2);
    _column = @[ _value1, _value2 ];
}

#pragma mark - Database - Get Object
- (void)test_database_get_object
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOfClass:self.tableClass fromTable:self.tableName] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_where
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier == 2 LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier == 2] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_orders
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOfClass:self.tableClass fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOfClass:self.tableClass fromTable:self.tableName offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_where_orders
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_where_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_orders_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOfClass:self.tableClass fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_where_orders_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Database - Get Objects
- (void)test_database_get_objects
{
    BOOL result = [self checkObjects:self.objects
                              andSQL:@"SELECT identifier, content FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_where
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier == 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier == 1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_orders
{
    BOOL result = [self checkObjects:self.objects.reversedArray
                              andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_limit
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName limit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_where_orders
{
    BOOL result = [self checkObjects:self.objects.reversedArray
                              andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_where_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 1 LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 1 limit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_orders_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName limit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_where_orders_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_where_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 limit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_orders_limit_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_where_orders_limit_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.database getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Database - Get Part Of Object
- (void)test_database_get_object_on_result_columns
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_on_result_columns_where
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1 LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_on_result_columns_orders
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_on_result_columns_offset
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_on_result_columns_where_orders
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_on_result_columns_where_offset
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_on_result_columns_orders_offset
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_object_on_result_columns_where_orders_offset
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.database getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Database - Get Part Of Objects
- (void)test_database_get_objects_on_result_columns
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_on_result_columns_where
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_on_result_columns_orders
{
    BOOL result = [self checkObjects:self.partialObjects.reversedArray
                              andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_on_result_columns_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_on_result_columns_where_orders
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_on_result_columns_where_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_on_result_columns_orders_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_on_result_columns_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_on_result_columns_where_orders_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_on_result_columns_where_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_on_result_columns_orders_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_objects_on_result_columns_where_orders_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.database getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

#pragma mark - Database - Get One Row
- (void)test_database_get_row_on_result_columns
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.database getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_row_on_result_columns_where
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1 LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.database getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_row_on_result_columns_orders
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.database getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_row_on_result_columns_offset
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.database getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName offset:1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_row_on_result_columns_where_orders
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.database getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_row_on_result_columns_where_offset
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.database getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 offset:1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_row_on_result_columns_orders_offset
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.database getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_row_on_result_columns_where_orders_offset
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.database getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
           }];
    TestCaseAssertTrue(result);
}

#pragma mark - Database - Get One Column
- (void)test_database_get_column_on_result
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_column_on_result_where
{
    BOOL result = [self checkColumn:@[ self.value1 ]
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_column_on_result_orders
{
    BOOL result = [self checkColumn:self.column.reversedArray
                             andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_column_on_result_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_column_on_result_where_orders
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_column_on_result_where_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_column_on_result_orders_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_column_on_result_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_column_on_result_where_orders_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_column_on_result_where_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_column_on_result_orders_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_column_on_result_where_orders_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.database getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Database - Get One Value
#pragma mark - Database - Get One Value
- (void)test_database_get_value_on_result_column
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.database getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_value_on_result_column_where
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1 LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.database getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_value_on_result_column_orders
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.database getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_value_on_result_column_offset
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.database getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName offset:1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_value_on_result_column_where_orders
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.database getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_value_on_result_column_where_offset
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.database getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 offset:1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_value_on_result_column_orders_offset
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.database getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_value_on_result_column_where_orders_offset
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.database getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
             }];
    TestCaseAssertTrue(result);
}

#pragma mark - Database - Get Rows
- (void)test_database_get_rows_on_result_columns
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_rows_on_result_columns_where
{
    BOOL result = [self checkRows:@[ self.partialRow1 ]
                           andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_rows_on_result_columns_orders
{
    BOOL result = [self checkRows:self.partialRows.reversedArray
                           andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_rows_on_result_columns_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_rows_on_result_columns_where_orders
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_rows_on_result_columns_where_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_rows_on_result_columns_orders_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_rows_on_result_columns_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_rows_on_result_columns_where_orders_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_rows_on_result_columns_where_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_rows_on_result_columns_orders_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_database_get_rows_on_result_columns_where_orders_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.database getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

#pragma mark - Table - Get Object
- (void)test_table_get_object
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObject] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_where
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier == 2 LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectWhere:TestCaseObject.identifier == 2] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_orders
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectOrders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectOffset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_where_orders
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectWhere:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_where_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectWhere:TestCaseObject.identifier > 0 offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_orders_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectOrders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_where_orders_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectWhere:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Table - Get Objects
- (void)test_table_get_objects
{
    BOOL result = [self checkObjects:self.objects
                              andSQL:@"SELECT identifier, content FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjects];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_where
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier == 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.table getObjectsWhere:TestCaseObject.identifier == 1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_orders
{
    BOOL result = [self checkObjects:self.objects.reversedArray
                              andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOrders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_limit
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.table getObjectsLimit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_where_orders
{
    BOOL result = [self checkObjects:self.objects.reversedArray
                              andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsWhere:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_where_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 1 LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.table getObjectsWhere:TestCaseObject.identifier > 1 limit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_orders_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.table getObjectsOrders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.table getObjectsLimit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_where_orders_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.table getObjectsWhere:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_where_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.table getObjectsWhere:TestCaseObject.identifier > 0 limit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_orders_limit_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.table getObjectsOrders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_where_orders_limit_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.table getObjectsWhere:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Table - Get Part Of Object
- (void)test_table_get_object_on_result_columns
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectOnResultColumns:TestCaseObject.identifier] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_on_result_columns_where
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1 LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectOnResultColumns:TestCaseObject.identifier where:TestCaseObject.identifier == 1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_on_result_columns_orders
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectOnResultColumns:TestCaseObject.identifier orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_on_result_columns_offset
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectOnResultColumns:TestCaseObject.identifier offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_on_result_columns_where_orders
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectOnResultColumns:TestCaseObject.identifier where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_on_result_columns_where_offset
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectOnResultColumns:TestCaseObject.identifier where:TestCaseObject.identifier > 0 offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_on_result_columns_orders_offset
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectOnResultColumns:TestCaseObject.identifier orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_object_on_result_columns_where_orders_offset
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [self.table getObjectOnResultColumns:TestCaseObject.identifier where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Table - Get Part Of Objects
- (void)test_table_get_objects_on_result_columns
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOnResultColumns:TestCaseObject.identifier];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_on_result_columns_where
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [self.table getObjectsOnResultColumns:TestCaseObject.identifier where:TestCaseObject.identifier == 1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_on_result_columns_orders
{
    BOOL result = [self checkObjects:self.partialObjects.reversedArray
                              andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOnResultColumns:TestCaseObject.identifier orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_on_result_columns_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOnResultColumns:TestCaseObject.identifier];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_on_result_columns_where_orders
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOnResultColumns:TestCaseObject.identifier];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_on_result_columns_where_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOnResultColumns:TestCaseObject.identifier];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_on_result_columns_orders_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOnResultColumns:TestCaseObject.identifier];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_on_result_columns_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOnResultColumns:TestCaseObject.identifier];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_on_result_columns_where_orders_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOnResultColumns:TestCaseObject.identifier];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_on_result_columns_where_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOnResultColumns:TestCaseObject.identifier];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_on_result_columns_orders_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOnResultColumns:TestCaseObject.identifier];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_objects_on_result_columns_where_orders_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [self.table getObjectsOnResultColumns:TestCaseObject.identifier];
               }];
    TestCaseAssertTrue(result);
}

#pragma mark - Table - Get One Row
- (void)test_table_get_row_on_result_columns
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.table getRowOnResultColumns:TestCaseObject.identifier];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_row_on_result_columns_where
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1 LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.table getRowOnResultColumns:TestCaseObject.identifier where:TestCaseObject.identifier == 1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_row_on_result_columns_orders
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.table getRowOnResultColumns:TestCaseObject.identifier orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_row_on_result_columns_offset
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.table getRowOnResultColumns:TestCaseObject.identifier offset:1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_row_on_result_columns_where_orders
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.table getRowOnResultColumns:TestCaseObject.identifier where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_row_on_result_columns_where_offset
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.table getRowOnResultColumns:TestCaseObject.identifier where:TestCaseObject.identifier > 0 offset:1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_row_on_result_columns_orders_offset
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.table getRowOnResultColumns:TestCaseObject.identifier orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_row_on_result_columns_where_orders_offset
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [self.table getRowOnResultColumns:TestCaseObject.identifier where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
           }];
    TestCaseAssertTrue(result);
}

#pragma mark - Table - Get One Column
- (void)test_table_get_column_on_result
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_column_on_result_where
{
    BOOL result = [self checkColumn:@[ self.value1 ]
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier where:TestCaseObject.identifier == 1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_column_on_result_orders
{
    BOOL result = [self checkColumn:self.column.reversedArray
                             andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_column_on_result_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_column_on_result_where_orders
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_column_on_result_where_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_column_on_result_orders_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_column_on_result_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_column_on_result_where_orders_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_column_on_result_where_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_column_on_result_orders_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_column_on_result_where_orders_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [self.table getColumnOnResult:TestCaseObject.identifier];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Table - Get One Value
#pragma mark - Table - Get One Value
- (void)test_table_get_value_on_result_column
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.table getValueOnResultColumn:TestCaseObject.identifier];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_value_on_result_column_where
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1 LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.table getValueOnResultColumn:TestCaseObject.identifier where:TestCaseObject.identifier == 1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_value_on_result_column_orders
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.table getValueOnResultColumn:TestCaseObject.identifier orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_value_on_result_column_offset
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.table getValueOnResultColumn:TestCaseObject.identifier offset:1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_value_on_result_column_where_orders
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.table getValueOnResultColumn:TestCaseObject.identifier where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_value_on_result_column_where_offset
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.table getValueOnResultColumn:TestCaseObject.identifier where:TestCaseObject.identifier > 0 offset:1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_value_on_result_column_orders_offset
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.table getValueOnResultColumn:TestCaseObject.identifier orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_value_on_result_column_where_orders_offset
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [self.table getValueOnResultColumn:TestCaseObject.identifier where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
             }];
    TestCaseAssertTrue(result);
}

#pragma mark - Table - Get Rows
- (void)test_table_get_rows_on_result_columns
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_rows_on_result_columns_where
{
    BOOL result = [self checkRows:@[ self.partialRow1 ]
                           andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier where:TestCaseObject.identifier == 1];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_rows_on_result_columns_orders
{
    BOOL result = [self checkRows:self.partialRows.reversedArray
                           andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_rows_on_result_columns_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_rows_on_result_columns_where_orders
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_rows_on_result_columns_where_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_rows_on_result_columns_orders_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_rows_on_result_columns_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_rows_on_result_columns_where_orders_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_rows_on_result_columns_where_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_rows_on_result_columns_orders_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_table_get_rows_on_result_columns_where_orders_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [self.table getRowsOnResultColumns:TestCaseObject.identifier];
            }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Get Object
- (void)test_handle_get_object
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOfClass:self.tableClass fromTable:self.tableName] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_where
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier == 2 LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier == 2] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_orders
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOfClass:self.tableClass fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOfClass:self.tableClass fromTable:self.tableName offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_where_orders
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_where_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_orders_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOfClass:self.tableClass fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_where_orders_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Get Objects
- (void)test_handle_get_objects
{
    BOOL result = [self checkObjects:self.objects
                              andSQL:@"SELECT identifier, content FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_where
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier == 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier == 1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_orders
{
    BOOL result = [self checkObjects:self.objects.reversedArray
                              andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_limit
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName limit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_where_orders
{
    BOOL result = [self checkObjects:self.objects.reversedArray
                              andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_where_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 1 LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 1 limit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_orders_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName limit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_where_orders_limit
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_where_limit_offset
{
    BOOL result = [self checkObject:self.object2
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 limit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_orders_limit_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_where_orders_limit_offset
{
    BOOL result = [self checkObject:self.object1
                             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [[self.database getHandle] getObjectsOfClass:self.tableClass fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) limit:1 offset:1];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Get Part Of Object
- (void)test_handle_get_object_on_result_columns
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_on_result_columns_where
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1 LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_on_result_columns_orders
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_on_result_columns_offset
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_on_result_columns_where_orders
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_on_result_columns_where_offset
{
    BOOL result = [self checkObject:self.partialObject2
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_on_result_columns_orders_offset
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_object_on_result_columns_where_orders_offset
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return @[ [[self.database getHandle] getObjectOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1] ];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Get Part Of Objects
- (void)test_handle_get_objects_on_result_columns
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_on_result_columns_where
{
    BOOL result = [self checkObject:self.partialObject1
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1"
              asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                  return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_on_result_columns_orders
{
    BOOL result = [self checkObjects:self.partialObjects.reversedArray
                              andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_on_result_columns_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_on_result_columns_where_orders
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_on_result_columns_where_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_on_result_columns_orders_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_on_result_columns_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_on_result_columns_where_orders_limit
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_on_result_columns_where_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_on_result_columns_orders_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_objects_on_result_columns_where_orders_limit_offset
{
    BOOL result = [self checkObjects:self.partialObjects
                              andSQL:@"SELECT identifier FROM main.testTable"
               asExpectedBySelecting:^NSArray<TestCaseObject *> * {
                   return [[self.database getHandle] getObjectsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Get One Row
- (void)test_handle_get_row_on_result_columns
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [[self.database getHandle] getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_row_on_result_columns_where
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1 LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [[self.database getHandle] getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_row_on_result_columns_orders
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [[self.database getHandle] getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_row_on_result_columns_offset
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [[self.database getHandle] getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName offset:1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_row_on_result_columns_where_orders
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [[self.database getHandle] getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_row_on_result_columns_where_offset
{
    BOOL result = [self checkRow:self.partialRow2
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [[self.database getHandle] getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 offset:1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_row_on_result_columns_orders_offset
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [[self.database getHandle] getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
           }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_row_on_result_columns_where_orders_offset
{
    BOOL result = [self checkRow:self.partialRow1
                          andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           asExpectedBySelecting:^WCTOneRow * {
               return [[self.database getHandle] getRowOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
           }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Get One Column
- (void)test_handle_get_column_on_result
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_column_on_result_where
{
    BOOL result = [self checkColumn:@[ self.value1 ]
                             andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_column_on_result_orders
{
    BOOL result = [self checkColumn:self.column.reversedArray
                             andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_column_on_result_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_column_on_result_where_orders
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_column_on_result_where_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_column_on_result_orders_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_column_on_result_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_column_on_result_where_orders_limit
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_column_on_result_where_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_column_on_result_orders_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_column_on_result_where_orders_limit_offset
{
    BOOL result = [self checkColumn:self.column
                             andSQL:@"SELECT identifier FROM main.testTable"
              asExpectedBySelecting:^WCTOneColumn * {
                  return [[self.database getHandle] getColumnOnResult:TestCaseObject.identifier fromTable:self.tableName];
              }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Get One Value
#pragma mark - Handle - Get One Value
- (void)test_handle_get_value_on_result_column
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [[self.database getHandle] getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_value_on_result_column_where
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1 LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [[self.database getHandle] getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_value_on_result_column_orders
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [[self.database getHandle] getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_value_on_result_column_offset
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [[self.database getHandle] getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName offset:1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_value_on_result_column_where_orders
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
             asExpectedBySelecting:^WCTValue * {
                 return [[self.database getHandle] getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_value_on_result_column_where_offset
{
    BOOL result = [self checkValue:self.value2
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [[self.database getHandle] getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 offset:1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_value_on_result_column_orders_offset
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [[self.database getHandle] getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
             }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_value_on_result_column_where_orders_offset
{
    BOOL result = [self checkValue:self.value1
                            andSQL:@"SELECT identifier FROM main.testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
             asExpectedBySelecting:^WCTValue * {
                 return [[self.database getHandle] getValueOnResultColumn:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier > 0 orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending) offset:1];
             }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle - Get Rows
- (void)test_handle_get_rows_on_result_columns
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_rows_on_result_columns_where
{
    BOOL result = [self checkRows:@[ self.partialRow1 ]
                           andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName where:TestCaseObject.identifier == 1];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_rows_on_result_columns_orders
{
    BOOL result = [self checkRows:self.partialRows.reversedArray
                           andSQL:@"SELECT identifier FROM main.testTable ORDER BY identifier DESC"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_rows_on_result_columns_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_rows_on_result_columns_where_orders
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_rows_on_result_columns_where_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_rows_on_result_columns_orders_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_rows_on_result_columns_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_rows_on_result_columns_where_orders_limit
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_rows_on_result_columns_where_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_rows_on_result_columns_orders_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

- (void)test_handle_get_rows_on_result_columns_where_orders_limit_offset
{
    BOOL result = [self checkRows:self.partialRows
                           andSQL:@"SELECT identifier FROM main.testTable"
            asExpectedBySelecting:^WCTColumnsXRows * {
                return [[self.database getHandle] getRowsOnResultColumns:TestCaseObject.identifier fromTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

@end
