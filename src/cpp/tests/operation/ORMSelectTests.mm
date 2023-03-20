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

@interface ORMSelectTests : CPPCRUDTestCase

@property (nonatomic, assign) CPPTestCaseObject partialObject1;
@property (nonatomic, assign) CPPTestCaseObject partialObject2;
@property (nonatomic, assign) WCDB::ValueArray<CPPTestCaseObject> partialObjects;

@property (nonatomic, assign) WCDB::OneRowValue partialRow1;
@property (nonatomic, assign) WCDB::OneRowValue partialRow2;
@property (nonatomic, assign) WCDB::MultiRowsValue partialRows;

@property (nonatomic, assign) WCDB::Value value1;
@property (nonatomic, assign) WCDB::Value value2;
@property (nonatomic, assign) WCDB::OneColumnValue column;

@end

@implementation ORMSelectTests

- (void)setUp
{
    [super setUp];
    _partialObject1 = CPPTestCaseObject(1);
    _partialObject2 = CPPTestCaseObject(2);
    _partialObjects = { self.partialObject1, self.partialObject2 };
    [self insertPresetObjects];

    _partialRow1 = { 1 };
    _partialRow2 = { 2 };
    _partialRows = { _partialRow1, _partialRow2 };

    _value1 = 1;
    _value2 = 2;
    _column = { _value1, _value2 };
}

#pragma mark - Database - Get Object
- (void)test_database_get_object
{
    [self doTestObject:self.object1
                andSQL:@"SELECT identifier, content FROM testTable ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getFirstObject<CPPTestCaseObject>(self.tableName.UTF8String);
           }];
}

- (void)test_database_get_object_where
{
    [self doTestObject:self.object2
                andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 2 ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getFirstObject<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 2);
           }];
}

- (void)test_database_get_object_where_orders
{
    [self doTestObject:self.object2
                andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getFirstObject<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
           }];
}

- (void)test_database_get_object_where_orders_offset
{
    [self doTestObject:self.object1
                andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getFirstObject<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
           }];
}

#pragma mark - Database - Get Objects
- (void)test_database_get_objects
{
    [self doTestObjects:self.objects
                 andSQL:@"SELECT identifier, content FROM testTable ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getAllObjects<CPPTestCaseObject>(self.tableName.UTF8String);
            }];
}

- (void)test_database_get_objects_where
{
    [self doTestObjects:self.object1
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 1 ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getAllObjects<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
            }];
}

- (void)test_database_get_objects_where_orders
{
    [self doTestObjects:{ self.object2, self.object1 }
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getAllObjects<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
            }];
}

- (void)test_database_get_objects_where_orders_limit
{
    [self doTestObjects:self.object2
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getAllObjects<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
            }];
}

- (void)test_database_get_objects_where_orders_limit_offset
{
    [self doTestObjects:self.object1
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getAllObjects<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
            }];
}

#pragma mark - Database - Get Part Of Object
- (void)test_database_get_object_on_result_columns
{
    [self doTestObject:self.partialObject1
                andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getFirstObjectWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
           }];
}

- (void)test_database_get_object_on_result_columns_where
{
    [self doTestObject:self.partialObject1
                andSQL:@"SELECT identifier FROM testTable WHERE identifier == 1 ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getFirstObjectWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
           }];
}

- (void)test_database_get_object_on_result_columns_where_orders
{
    [self doTestObject:self.partialObject2
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getFirstObjectWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
           }];
}

- (void)test_database_get_object_on_result_columns_where_orders_offset
{
    [self doTestObject:self.partialObject1
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getFirstObjectWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
           }];
}

#pragma mark - Database - Get Part Of Objects
- (void)test_database_get_objects_on_result_columns
{
    [self doTestObjects:self.partialObjects
                 andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getAllObjectsWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
            }];
}

- (void)test_database_get_objects_on_result_columns_where
{
    [self doTestObjects:self.partialObject1
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier == 1 ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getAllObjectsWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
            }];
}

- (void)test_database_get_objects_on_result_columns_where_orders
{
    [self doTestObjects:{ self.partialObject2, self.partialObject1 }
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getAllObjectsWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
            }];
}

- (void)test_database_get_objects_on_result_columns_where_orders_limit
{
    [self doTestObjects:self.partialObject2
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getAllObjectsWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
            }];
}

- (void)test_database_get_objects_on_result_columns_where_orders_limit_offset
{
    [self doTestObjects:self.partialObject1
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getAllObjectsWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
            }];
}

#pragma mark - Database - Get Value
- (void)test_database_get_value
{
    [self doTestValue:self.object1.identifier
               andSQL:@"SELECT identifier FROM testTable LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.database->selectValue(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String);
          }];
}

- (void)test_database_get_value_where
{
    [self doTestValue:self.object2.identifier
               andSQL:@"SELECT identifier FROM testTable WHERE identifier == 2 LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.database->selectValue(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 2);
          }];
}

- (void)test_database_get_value_where_orders
{
    [self doTestValue:self.object2.identifier
               andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.database->selectValue(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
          }];
}

- (void)test_database_get_value_where_orders_offset
{
    [self doTestValue:self.object1.identifier
               andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
          bySelecting:^WCDB::OptionalValue {
              return self.database->selectValue(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
          }];
}

#pragma mark - Database - Get Row

- (void)test_database_get_row
{
    [self doTestRow:{ self.object1.identifier, self.object1.content }
             andSQL:@"SELECT identifier, content FROM testTable LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.database->selectOneRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String);
        }];
}

- (void)test_database_get_row_where
{
    [self doTestRow:{ self.object2.identifier, self.object2.content }
             andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 2 LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.database->selectOneRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 2);
        }];
}

- (void)test_database_get_row_where_orders
{
    [self doTestRow:{ self.object2.identifier, self.object2.content }
             andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.database->selectOneRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
        }];
}

- (void)test_database_get_row_where_orders_offset
{
    [self doTestRow:{ self.object1.identifier, self.object1.content }
             andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.database->selectOneRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
        }];
}

#pragma mark - Database - Get Column
- (void)test_database_get_column
{
    [self doTestColumn:{ self.object1.identifier, self.object2.identifier }
                andSQL:@"SELECT identifier FROM testTable"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String);
           }];
}

- (void)test_database_get_column_where
{
    [self doTestColumn:{ self.object1.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier == 1"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
           }];
}

- (void)test_database_get_column_where_orders
{
    [self doTestColumn:{ self.object2.identifier, self.object1.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
           }];
}

- (void)test_database_get_column_where_orders_limit
{
    [self doTestColumn:{ self.object2.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
           }];
}

- (void)test_database_get_column_where_orders_limit_offset
{
    [self doTestColumn:{ self.object1.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
           }];
}

#pragma mark - Database - Get Rows
- (void)test_database_get_rows
{
    [self doTestRows:{ {self.object1.identifier, self.object1.content },
                        { self.object2.identifier, self.object2.content } }
    andSQL:@"SELECT identifier, content FROM testTable"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.database->selectAllRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String);
    }];
}

- (void)test_database_get_rows_where
{
    [self doTestRows:{ {self.object1.identifier, self.object1.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 1"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.database->selectAllRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
    }];
}

- (void)test_database_get_rows_where_orders
{
    [self doTestRows:{ {self.object2.identifier, self.object2.content },
                        { self.object1.identifier, self.object1.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.database->selectAllRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
    }];
}

- (void)test_database_get_rows_where_orders_limit
{
    [self doTestRows:{ {self.object2.identifier, self.object2.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.database->selectAllRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
    }];
}

- (void)test_database_get_rows_where_orders_limit_offset
{
    [self doTestRows:{ {self.object1.identifier, self.object1.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.database->selectAllRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
    }];
}

#pragma mark - Database - Get From Statement
- (void)test_database_get_row_from_statement
{
    [self doTestRow:self.partialRow1
             andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.database->getOneRowFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()).limit(1));
        }];
}

- (void)test_database_get_column_from_statement
{
    [self doTestColumn:self.column
                andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->getOneColumnFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()));
           }];
}

- (void)test_database_get_value_from_statement
{
    [self doTestValue:self.value1
               andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()).limit(1));
          }];
}

- (void)test_database_get_rows_from_statement
{
    [self doTestRows:self.partialRows
              andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return self.database->getAllRowsFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()));
         }];
}

#pragma mark - Table - Get Object
- (void)test_table_get_object
{
    [self doTestObject:self.object1
                andSQL:@"SELECT identifier, content FROM testTable ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.table.getFirstObject();
           }];
}

- (void)test_table_get_object_where
{
    [self doTestObject:self.object2
                andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 2 ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.table.getFirstObject(WCDB_FIELD(CPPTestCaseObject::identifier) == 2);
           }];
}

- (void)test_table_get_object_where_orders
{
    [self doTestObject:self.object2
                andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.table.getFirstObject(WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
           }];
}

- (void)test_table_get_object_where_orders_offset
{
    [self doTestObject:self.object1
                andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.table.getFirstObject(WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
           }];
}

#pragma mark - Table - Get Objects
- (void)test_table_get_objects
{
    [self doTestObjects:self.objects
                 andSQL:@"SELECT identifier, content FROM testTable ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.table.getAllObjects();
            }];
}

- (void)test_table_get_objects_where
{
    [self doTestObjects:self.object1
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 1 ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.table.getAllObjects(WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
            }];
}

- (void)test_table_get_objects_where_orders
{
    [self doTestObjects:{ self.object2, self.object1 }
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.table.getAllObjects(WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
            }];
}

- (void)test_table_get_objects_where_orders_limit
{
    [self doTestObjects:self.object2
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.table.getAllObjects(WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
            }];
}

- (void)test_table_get_objects_where_orders_limit_offset
{
    [self doTestObjects:self.object1
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.table.getAllObjects(WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
            }];
}

#pragma mark - Table - Get Part Of Object
- (void)test_table_get_object_on_result_columns
{
    [self doTestObject:self.partialObject1
                andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.table.getFirstObjectWithFields(WCDB_FIELD(CPPTestCaseObject::identifier));
           }];
}

- (void)test_table_get_object_on_result_columns_where
{
    [self doTestObject:self.partialObject1
                andSQL:@"SELECT identifier FROM testTable WHERE identifier == 1 ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.table.getFirstObjectWithFields(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
           }];
}

- (void)test_table_get_object_on_result_columns_where_orders
{
    [self doTestObject:self.partialObject2
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.table.getFirstObjectWithFields(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
           }];
}

- (void)test_table_get_object_on_result_columns_where_orders_offset
{
    [self doTestObject:self.partialObject1
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.table.getFirstObjectWithFields(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
           }];
}

#pragma mark - Table - Get Part Of Objects
- (void)test_table_get_objects_on_result_columns
{
    [self doTestObjects:self.partialObjects
                 andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.table.getAllObjectsWithFields(WCDB_FIELD(CPPTestCaseObject::identifier));
            }];
}

- (void)test_table_get_objects_on_result_columns_where
{
    [self doTestObjects:self.partialObject1
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier == 1 ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.table.getAllObjectsWithFields(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
            }];
}

- (void)test_table_get_objects_on_result_columns_where_orders
{
    [self doTestObjects:{ self.partialObject2, self.partialObject1 }
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.table.getAllObjectsWithFields(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
            }];
}

- (void)test_table_get_objects_on_result_columns_where_orders_limit
{
    [self doTestObjects:self.partialObject2
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.table.getAllObjectsWithFields(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
            }];
}

- (void)test_table_get_objects_on_result_columns_where_orders_limit_offset
{
    [self doTestObjects:self.partialObject1
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.table.getAllObjectsWithFields(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
            }];
}

#pragma mark - Table - Get Value
- (void)test_table_get_value
{
    [self doTestValue:self.object1.identifier
               andSQL:@"SELECT identifier FROM testTable LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.table.selectValue(WCDB_FIELD(CPPTestCaseObject::identifier));
          }];
}

- (void)test_table_get_value_where
{
    [self doTestValue:self.object2.identifier
               andSQL:@"SELECT identifier FROM testTable WHERE identifier == 2 LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.table.selectValue(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) == 2);
          }];
}

- (void)test_table_get_value_where_orders
{
    [self doTestValue:self.object2.identifier
               andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.table.selectValue(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
          }];
}

- (void)test_table_get_value_where_orders_offset
{
    [self doTestValue:self.object1.identifier
               andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
          bySelecting:^WCDB::OptionalValue {
              return self.table.selectValue(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
          }];
}

#pragma mark - Table - Get Row

- (void)test_table_get_row
{
    [self doTestRow:{ self.object1.identifier, self.object1.content }
             andSQL:@"SELECT identifier, content FROM testTable LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.table.selectOneRow(CPPTestCaseObject::allFields());
        }];
}

- (void)test_table_get_row_where
{
    [self doTestRow:{ self.object2.identifier, self.object2.content }
             andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 2 LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.table.selectOneRow(CPPTestCaseObject::allFields(), WCDB_FIELD(CPPTestCaseObject::identifier) == 2);
        }];
}

- (void)test_table_get_row_where_orders
{
    [self doTestRow:{ self.object2.identifier, self.object2.content }
             andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.table.selectOneRow(CPPTestCaseObject::allFields(), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
        }];
}

- (void)test_table_get_row_where_orders_offset
{
    [self doTestRow:{ self.object1.identifier, self.object1.content }
             andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.table.selectOneRow(CPPTestCaseObject::allFields(), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
        }];
}

#pragma mark - Table - Get Column
- (void)test_table_get_column
{
    [self doTestColumn:{ self.object1.identifier, self.object2.identifier }
                andSQL:@"SELECT identifier FROM testTable"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.table.selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier));
           }];
}

- (void)test_table_get_column_where
{
    [self doTestColumn:{ self.object1.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier == 1"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.table.selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
           }];
}

- (void)test_table_get_column_where_orders
{
    [self doTestColumn:{ self.object2.identifier, self.object1.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.table.selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
           }];
}

- (void)test_table_get_column_where_orders_limit
{
    [self doTestColumn:{ self.object2.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.table.selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
           }];
}

- (void)test_table_get_column_where_orders_limit_offset
{
    [self doTestColumn:{ self.object1.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.table.selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
           }];
}

#pragma mark - Table - Get Rows
- (void)test_table_get_rows
{
    [self doTestRows:{ {self.object1.identifier, self.object1.content },
                        { self.object2.identifier, self.object2.content } }
    andSQL:@"SELECT identifier, content FROM testTable"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.table.selectAllRow(CPPTestCaseObject::allFields());
    }];
}

- (void)test_table_get_rows_where
{
    [self doTestRows:{ {self.object1.identifier, self.object1.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 1"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.table.selectAllRow(CPPTestCaseObject::allFields(), WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
    }];
}

- (void)test_table_get_rows_where_orders
{
    [self doTestRows:{ {self.object2.identifier, self.object2.content },
                        { self.object1.identifier, self.object1.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.table.selectAllRow(CPPTestCaseObject::allFields(), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
    }];
}

- (void)test_table_get_rows_where_orders_limit
{
    [self doTestRows:{ {self.object2.identifier, self.object2.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.table.selectAllRow(CPPTestCaseObject::allFields(), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
    }];
}

- (void)test_table_get_rows_where_orders_limit_offset
{
    [self doTestRows:{ {self.object1.identifier, self.object1.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.table.selectAllRow(CPPTestCaseObject::allFields(), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
    }];
}

#pragma mark - Table - Get From Statement
- (void)test_table_get_row_from_statement
{
    [self doTestRow:self.partialRow1
             andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.table.getOneRowFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()).limit(1));
        }];
}

- (void)test_table_get_column_from_statement
{
    [self doTestColumn:self.column
                andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.table.getOneColumnFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()));
           }];
}

- (void)test_table_get_value_from_statement
{
    [self doTestValue:self.value1
               andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.table.getValueFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()).limit(1));
          }];
}

- (void)test_table_get_rows_from_statement
{
    [self doTestRows:self.partialRows
              andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return self.table.getAllRowsFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()));
         }];
}

#pragma mark - Handle - Get Object
- (void)test_handle_get_object
{
    [self doTestObject:self.object1
                andSQL:@"SELECT identifier, content FROM testTable ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getHandle().getFirstObject<CPPTestCaseObject>(self.tableName.UTF8String);
           }];
}

- (void)test_handle_get_object_where
{
    [self doTestObject:self.object2
                andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 2 ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getHandle().getFirstObject<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 2);
           }];
}

- (void)test_handle_get_object_where_orders
{
    [self doTestObject:self.object2
                andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getHandle().getFirstObject<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
           }];
}

- (void)test_handle_get_object_where_orders_offset
{
    [self doTestObject:self.object1
                andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getHandle().getFirstObject<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
           }];
}

#pragma mark - Handle - Get Objects
- (void)test_handle_get_objects
{
    [self doTestObjects:self.objects
                 andSQL:@"SELECT identifier, content FROM testTable ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getHandle().getAllObjects<CPPTestCaseObject>(self.tableName.UTF8String);
            }];
}

- (void)test_handle_get_objects_where
{
    [self doTestObjects:self.object1
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 1 ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getHandle().getAllObjects<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
            }];
}

- (void)test_handle_get_objects_where_orders
{
    [self doTestObjects:{ self.object2, self.object1 }
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getHandle().getAllObjects<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
            }];
}

- (void)test_handle_get_objects_where_orders_limit
{
    [self doTestObjects:self.object2
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getHandle().getAllObjects<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
            }];
}

- (void)test_handle_get_objects_where_orders_limit_offset
{
    [self doTestObjects:self.object1
                 andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getHandle().getAllObjects<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
            }];
}

#pragma mark - Handle - Get Part Of Object
- (void)test_handle_get_object_on_result_columns
{
    [self doTestObject:self.partialObject1
                andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getHandle().getFirstObjectWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
           }];
}

- (void)test_handle_get_object_on_result_columns_where
{
    [self doTestObject:self.partialObject1
                andSQL:@"SELECT identifier FROM testTable WHERE identifier == 1 ORDER BY rowid ASC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getHandle().getFirstObjectWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
           }];
}

- (void)test_handle_get_object_on_result_columns_where_orders
{
    [self doTestObject:self.partialObject2
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getHandle().getFirstObjectWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
           }];
}

- (void)test_handle_get_object_on_result_columns_where_orders_offset
{
    [self doTestObject:self.partialObject1
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           bySelecting:^WCDB::Optional<CPPTestCaseObject> {
               return self.database->getHandle().getFirstObjectWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
           }];
}

#pragma mark - Handle - Get Part Of Objects
- (void)test_handle_get_objects_on_result_columns
{
    [self doTestObjects:self.partialObjects
                 andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getHandle().getAllObjectsWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
            }];
}

- (void)test_handle_get_objects_on_result_columns_where
{
    [self doTestObjects:self.partialObject1
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier == 1 ORDER BY rowid ASC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getHandle().getAllObjectsWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
            }];
}

- (void)test_handle_get_objects_on_result_columns_where_orders
{
    [self doTestObjects:{ self.partialObject2, self.partialObject1 }
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getHandle().getAllObjectsWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
            }];
}

- (void)test_handle_get_objects_on_result_columns_where_orders_limit
{
    [self doTestObjects:self.partialObject2
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getHandle().getAllObjectsWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
            }];
}

- (void)test_handle_get_objects_on_result_columns_where_orders_limit_offset
{
    [self doTestObjects:self.partialObject1
                 andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                return self.database->getHandle().getAllObjectsWithFields<CPPTestCaseObject>(self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier), WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
            }];
}

#pragma mark - Handle - Get Value
- (void)test_handle_get_value
{
    [self doTestValue:self.object1.identifier
               andSQL:@"SELECT identifier FROM testTable LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.database->getHandle().selectValue(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String);
          }];
}

- (void)test_handle_get_value_where
{
    [self doTestValue:self.object2.identifier
               andSQL:@"SELECT identifier FROM testTable WHERE identifier == 2 LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.database->getHandle().selectValue(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 2);
          }];
}

- (void)test_handle_get_value_where_orders
{
    [self doTestValue:self.object2.identifier
               andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.database->getHandle().selectValue(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
          }];
}

- (void)test_handle_get_value_where_orders_offset
{
    [self doTestValue:self.object1.identifier
               andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
          bySelecting:^WCDB::OptionalValue {
              return self.database->getHandle().selectValue(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
          }];
}

#pragma mark - Handle - Get Row

- (void)test_handle_get_row
{
    [self doTestRow:{ self.object1.identifier, self.object1.content }
             andSQL:@"SELECT identifier, content FROM testTable LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.database->getHandle().selectOneRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String);
        }];
}

- (void)test_handle_get_row_where
{
    [self doTestRow:{ self.object2.identifier, self.object2.content }
             andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 2 LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.database->getHandle().selectOneRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 2);
        }];
}

- (void)test_handle_get_row_where_orders
{
    [self doTestRow:{ self.object2.identifier, self.object2.content }
             andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.database->getHandle().selectOneRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
        }];
}

- (void)test_handle_get_row_where_orders_offset
{
    [self doTestRow:{ self.object1.identifier, self.object1.content }
             andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.database->getHandle().selectOneRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
        }];
}

#pragma mark - Handle - Get Column
- (void)test_handle_get_column
{
    [self doTestColumn:{ self.object1.identifier, self.object2.identifier }
                andSQL:@"SELECT identifier FROM testTable"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->getHandle().selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String);
           }];
}

- (void)test_handle_get_column_where
{
    [self doTestColumn:{ self.object1.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier == 1"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->getHandle().selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
           }];
}

- (void)test_handle_get_column_where_orders
{
    [self doTestColumn:{ self.object2.identifier, self.object1.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->getHandle().selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
           }];
}

- (void)test_handle_get_column_where_orders_limit
{
    [self doTestColumn:{ self.object2.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->getHandle().selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
           }];
}

- (void)test_handle_get_column_where_orders_limit_offset
{
    [self doTestColumn:{ self.object1.identifier }
                andSQL:@"SELECT identifier FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->getHandle().selectOneColumn(WCDB_FIELD(CPPTestCaseObject::identifier), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
           }];
}

#pragma mark - Handle - Get Rows
- (void)test_handle_get_rows
{
    [self doTestRows:{ {self.object1.identifier, self.object1.content },
                        { self.object2.identifier, self.object2.content } }
    andSQL:@"SELECT identifier, content FROM testTable"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.database->getHandle().selectAllRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String);
    }];
}

- (void)test_handle_get_rows_where
{
    [self doTestRows:{ {self.object1.identifier, self.object1.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier == 1"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.database->getHandle().selectAllRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
    }];
}

- (void)test_handle_get_rows_where_orders
{
    [self doTestRows:{ {self.object2.identifier, self.object2.content },
                        { self.object1.identifier, self.object1.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.database->getHandle().selectAllRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC));
    }];
}

- (void)test_handle_get_rows_where_orders_limit
{
    [self doTestRows:{ {self.object2.identifier, self.object2.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.database->getHandle().selectAllRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1);
    }];
}

- (void)test_handle_get_rows_where_orders_limit_offset
{
    [self doTestRows:{ {self.object1.identifier, self.object1.content } }
    andSQL:@"SELECT identifier, content FROM testTable WHERE identifier > 0 ORDER BY identifier DESC LIMIT 1 OFFSET 1"
    bySelecting:^WCDB::OptionalMultiRows {
        return self.database->getHandle().selectAllRow(CPPTestCaseObject::allFields(), self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier) > 0, WCDB_FIELD(CPPTestCaseObject::identifier).asOrder(WCDB::Order::DESC), 1, 1);
    }];
}

#pragma mark - Handle - Get From Statement
- (void)test_handle_get_row_from_statement
{
    [self doTestRow:self.partialRow1
             andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC LIMIT 1"
        bySelecting:^WCDB::OptionalOneRow {
            return self.database->getHandle().getOneRowFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()).limit(1));
        }];
}

- (void)test_handle_get_column_from_statement
{
    [self doTestColumn:self.column
                andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.database->getHandle().getOneColumnFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()));
           }];
}

- (void)test_handle_get_value_from_statement
{
    [self doTestValue:self.value1
               andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC LIMIT 1"
          bySelecting:^WCDB::OptionalValue {
              return self.database->getHandle().getValueFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()).limit(1));
          }];
}

- (void)test_handle_get_rows_from_statement
{
    [self doTestRows:self.partialRows
              andSQL:@"SELECT identifier FROM testTable ORDER BY rowid ASC"
         bySelecting:^WCDB::OptionalMultiRows {
             return self.database->getHandle().getAllRowsFromStatement(WCDB::StatementSelect().select(WCDB_FIELD(CPPTestCaseObject::identifier)).from(self.tableName.UTF8String).order(WCDB::OrderingTerm::ascendingRowid()));
         }];
}

@end
