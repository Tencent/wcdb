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

#import "MultipleMigrationTestCase.h"

@interface MultipleMigrationTestCase_Convenient : MultipleMigrationTestCase

@end

@implementation MultipleMigrationTestCase_Convenient {
    WCDB::Expression _greaterThan0Condition;

    WCDB::Expression _removeBothEndCondition;

    WCDB::OrderingTerm _ascendingOrder;

    WCDB::OrderingTerm _descendingOrder;

    WCDB::Expression _limit1;

    WCDB::Expression _offset1;
}

- (void)setUp
{
    [super setUp];
    //Three tables: 1. migrating 2. migrated 3. not started
    BOOL done;
    XCTAssertTrue([_migrated stepMigration:done]);
    NSString *migratingTable = [_migrated getValueOnResult:WCDB::Column("value") fromTable:@"WCDBKV" where:WCDB::Column("key") == "WCDBMigrating"].stringValue;
    XCTAssertTrue([migratingTable isEqualToString:_table1] || [migratingTable isEqualToString:_migratedTable2] || [migratingTable isEqualToString:_migratedTable3]);
    while (YES) {
        WCTError *error;
        if (![_migrated isTableExists:migratingTable withError:&error]) {
            XCTAssertNil(error);
            break;
        }
        XCTAssertTrue([_migrated stepMigration:done]);
        if (done) {
            break;
        }
    }
    XCTAssertTrue([_migrated stepMigration:done]);
    XCTAssertTrue([_migrated stepMigration:done]);

    _greaterThan0Condition = TestCaseObject.variable1 > 0;

    int max = _count - 1;

    _removeBothEndCondition = TestCaseObject.variable1 > 0 && TestCaseObject.variable1 < max;

    _ascendingOrder = TestCaseObject.variable1.asOrder(WCTOrderedAscending);

    _descendingOrder = TestCaseObject.variable1.asOrder(WCTOrderedDescending);

    _limit1 = 1;

    _offset1 = 1;
}

#pragma mark - Drop Table
- (void)drop_table_test:(NSString *)tableName
{
    XCTAssertTrue([_migrated isTableExists:tableName withError:nil]);
    XCTAssertTrue([_migrated dropTable:tableName]);
    WCTError *error = nil;
    XCTAssertFalse([_migrated isTableExists:tableName withError:&error]);
    XCTAssertNil(error);
}
- (void)test_drop_table
{
    [self drop_table_test:_table1];
    [self drop_table_test:_migratedTable2];
    [self drop_table_test:_migratedTable3];
}

#pragma mark - Create Index
- (BOOL)isIndexExists:(NSString *)indexName
{
    WCTValue *rowValue = [_migrated getValueFromStatement:WCDB::StatementSelect().select(WCTMaster.allResults.count()).from(WCTMaster.tableName.UTF8String).where(WCTMaster.name == indexName)];
    XCTAssertNotNil(rowValue);
    return rowValue.boolValue;
}

- (void)create_index_test:(NSString *)tableName
{
    NSString *indexNameSubfix = @"_index_test";
    NSString *indexName = [NSString stringWithFormat:@"%@_%@", tableName, indexNameSubfix];
    XCTAssertFalse([self isIndexExists:indexName]);
    WCDB::StatementCreateIndex statement = WCDB::StatementCreateIndex().createIndex(indexName.UTF8String).ifNotExists().indexedBy(TestCaseObject.variable3.asIndex()).on(tableName.UTF8String);
    XCTAssertTrue([_migrated execute:statement]);
    BOOL done;
    while ([_migrated stepMigration:done] && !done)
        ;
    XCTAssertTrue(done);
    XCTAssertTrue([self isIndexExists:indexName]);
}

- (void)test_create_index
{
    [self create_index_test:_table1];
    [self create_index_test:_migratedTable2];
    [self create_index_test:_migratedTable3];
}

#pragma mark - Get Object
- (void)get_object_ofClass_test:(NSString *)tableName
         withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [_migrated getObjectOfClass:_cls
                                               fromTable:tableName];
    XCTAssertTrue([object isEqualToObject:preInsertedObjects[0]]);
}

- (void)test_get_object_ofClass
{
    [self get_object_ofClass_test:_table1 withPreInsertedObjects:_preInsertObjects1];
    [self get_object_ofClass_test:_migratedTable2 withPreInsertedObjects:_preInsertObjects2];
    [self get_object_ofClass_test:_migratedTable3 withPreInsertedObjects:_preInsertObjects3];
}

- (void)get_object_ofClass_where_test:(NSString *)tableName
               withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [_migrated getObjectOfClass:_cls
                                               fromTable:tableName
                                                   where:_greaterThan0Condition];
    XCTAssertTrue([object isEqualToObject:preInsertedObjects[1]]);
}

- (void)test_get_object_ofClass_where
{
    [self get_object_ofClass_where_test:_table1 withPreInsertedObjects:_preInsertObjects1];
    [self get_object_ofClass_where_test:_migratedTable2 withPreInsertedObjects:_preInsertObjects2];
    [self get_object_ofClass_where_test:_migratedTable3 withPreInsertedObjects:_preInsertObjects3];
}

- (void)get_object_ofClass_orderBy_test:(NSString *)tableName
                 withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [_migrated getObjectOfClass:_cls
                                               fromTable:tableName
                                                 orderBy:_descendingOrder];
    XCTAssertTrue([object isEqualToObject:preInsertedObjects.lastObject]);
}

- (void)test_get_object_ofClass_orderBy
{
    [self get_object_ofClass_orderBy_test:_table1 withPreInsertedObjects:_preInsertObjects1];
    [self get_object_ofClass_orderBy_test:_migratedTable2 withPreInsertedObjects:_preInsertObjects2];
    [self get_object_ofClass_orderBy_test:_migratedTable3 withPreInsertedObjects:_preInsertObjects3];
}

- (void)get_object_ofClass_offset_test:(NSString *)tableName
                withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [_migrated getObjectOfClass:_cls
                                               fromTable:tableName
                                                  offset:_offset1];
    XCTAssertTrue([object isEqualToObject:preInsertedObjects[1]]);
}

- (void)test_get_object_ofClass_offset
{
    [self get_object_ofClass_offset_test:_table1 withPreInsertedObjects:_preInsertObjects1];
    [self get_object_ofClass_offset_test:_migratedTable2 withPreInsertedObjects:_preInsertObjects2];
    [self get_object_ofClass_offset_test:_migratedTable3 withPreInsertedObjects:_preInsertObjects3];
}

- (void)get_object_ofClass_where_orderBy_test:(NSString *)tableName
                       withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [_migrated getObjectOfClass:_cls
                                               fromTable:tableName
                                                   where:_removeBothEndCondition
                                                 orderBy:_descendingOrder];
    XCTAssertTrue([object isEqualToObject:preInsertedObjects[preInsertedObjects.count - 2]]);
}

- (void)test_get_object_ofClass_where_orderBy
{
    [self get_object_ofClass_where_orderBy_test:_table1
                         withPreInsertedObjects:_preInsertObjects1];
    [self get_object_ofClass_where_orderBy_test:_migratedTable2
                         withPreInsertedObjects:_preInsertObjects2];
    [self get_object_ofClass_where_orderBy_test:_migratedTable3
                         withPreInsertedObjects:_preInsertObjects3];
}

- (void)get_object_ofClass_where_offset_test:(NSString *)tableName
                      withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [_migrated getObjectOfClass:_cls
                                               fromTable:tableName
                                                   where:_greaterThan0Condition
                                                  offset:_offset1];
    XCTAssertTrue([object isEqualToObject:preInsertedObjects[2]]);
}

- (void)test_get_object_ofClass_where_offset
{
    [self get_object_ofClass_where_offset_test:_table1
                        withPreInsertedObjects:_preInsertObjects1];
    [self get_object_ofClass_where_offset_test:_migratedTable2
                        withPreInsertedObjects:_preInsertObjects2];
    [self get_object_ofClass_where_offset_test:_migratedTable3
                        withPreInsertedObjects:_preInsertObjects3];
}

- (void)get_object_ofClass_orderBy_offset_test:(NSString *)tableName
                        withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [_migrated getObjectOfClass:_cls
                                               fromTable:tableName
                                                 orderBy:_descendingOrder
                                                  offset:_offset1];
    XCTAssertTrue([object isEqualToObject:preInsertedObjects[preInsertedObjects.count - 2]]);
}

- (void)test_get_object_ofClass_orderBy_offset
{
    [self get_object_ofClass_orderBy_offset_test:_table1
                          withPreInsertedObjects:_preInsertObjects1];
    [self get_object_ofClass_orderBy_offset_test:_migratedTable2
                          withPreInsertedObjects:_preInsertObjects2];
    [self get_object_ofClass_orderBy_offset_test:_migratedTable3
                          withPreInsertedObjects:_preInsertObjects3];
}

- (void)get_object_ofClass_where_orderBy_offset_test:(NSString *)tableName
                              withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [_migrated getObjectOfClass:_cls
                                               fromTable:tableName
                                                   where:_removeBothEndCondition
                                                 orderBy:_descendingOrder
                                                  offset:_offset1];
    XCTAssertTrue([object isEqualToObject:preInsertedObjects[preInsertedObjects.count - 3]]);
}

- (void)test_get_object_ofClass_where_orderBy_offset
{
    [self get_object_ofClass_where_orderBy_offset_test:_table1
                                withPreInsertedObjects:_preInsertObjects1];
    [self get_object_ofClass_where_orderBy_offset_test:_migratedTable2
                                withPreInsertedObjects:_preInsertObjects2];
    [self get_object_ofClass_where_orderBy_offset_test:_migratedTable3
                                withPreInsertedObjects:_preInsertObjects3];
}

#pragma mark - Insert
- (void)insert_object_test:(NSString *)tableName
    withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [TestCaseObject objectWithId:(int) preInsertedObjects.count];
    XCTAssertTrue([_migrated insertObject:object intoTable:tableName]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_migrated getObjectsOfClass:_cls fromTable:tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:preInsertedObjects];
    [expected addObject:object];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_insert_object
{
    [self insert_object_test:_table1 withPreInsertedObjects:_preInsertObjects1];
    [self insert_object_test:_migratedTable2 withPreInsertedObjects:_preInsertObjects2];
    [self insert_object_test:_migratedTable3 withPreInsertedObjects:_preInsertObjects3];
}

- (void)insert_objects_test:(NSString *)tableName
     withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    NSArray<TestCaseObject *> *objects = [TestCaseObject objectsWithCount:3 from:(int) preInsertedObjects.count];
    XCTAssertTrue([_migrated insertObjects:objects intoTable:tableName]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_migrated getObjectsOfClass:_cls fromTable:tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:preInsertedObjects];
    [expected addObjectsFromArray:objects];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_insert_objects
{
    [self insert_objects_test:_table1 withPreInsertedObjects:_preInsertObjects1];
    [self insert_objects_test:_migratedTable2 withPreInsertedObjects:_preInsertObjects2];
    [self insert_objects_test:_migratedTable3 withPreInsertedObjects:_preInsertObjects3];
}

- (void)insert_or_replace_object_test:(NSString *)tableName
               withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    XCTAssertTrue([_migrated insertOrReplaceObject:object intoTable:tableName]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_migrated getObjectsOfClass:_cls fromTable:tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:preInsertedObjects];
    expected[0] = object;
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_insert_or_replace_object
{
    [self insert_or_replace_object_test:_table1 withPreInsertedObjects:_preInsertObjects1];
    [self insert_or_replace_object_test:_migratedTable2 withPreInsertedObjects:_preInsertObjects2];
    [self insert_or_replace_object_test:_migratedTable3 withPreInsertedObjects:_preInsertObjects3];
}

- (void)insert_or_replace_objects_test:(NSString *)tableName
                withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    NSArray<TestCaseObject *> *objects = [TestCaseObject objectsWithCount:3 from:0];
    XCTAssertTrue([_migrated insertOrReplaceObjects:objects intoTable:tableName]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_migrated getObjectsOfClass:_cls fromTable:tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:preInsertedObjects];
    expected[0] = objects[0];
    expected[1] = objects[1];
    expected[2] = objects[2];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_insert_or_replace_objects
{
    [self insert_or_replace_objects_test:_table1 withPreInsertedObjects:_preInsertObjects1];
    [self insert_or_replace_objects_test:_migratedTable2 withPreInsertedObjects:_preInsertObjects2];
    [self insert_or_replace_objects_test:_migratedTable3 withPreInsertedObjects:_preInsertObjects3];
}

#pragma mark - Update Properties With Object
- (void)update_withObject_test:(NSString *)tableName
        withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    XCTAssertTrue(([_migrated updateTable:tableName onProperties:{TestCaseObject.variable2, TestCaseObject.variable3} withObject:object]));
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_migrated getObjectsOfClass:_cls fromTable:tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:preInsertedObjects];
    for (TestCaseObject *element in expected) {
        element.variable2 = object.variable2;
        element.variable3 = object.variable3;
    }
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_update_withObject
{
    [self update_withObject_test:_table1 withPreInsertedObjects:_preInsertObjects1];
    [self update_withObject_test:_migratedTable2 withPreInsertedObjects:_preInsertObjects2];
    [self update_withObject_test:_migratedTable3 withPreInsertedObjects:_preInsertObjects3];
}

- (void)update_withObject_where_test:(NSString *)tableName
              withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    XCTAssertTrue(([_migrated updateTable:tableName onProperties:{TestCaseObject.variable2, TestCaseObject.variable3} withObject:object where:_greaterThan0Condition]));
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_migrated getObjectsOfClass:_cls fromTable:tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:preInsertedObjects];
    for (TestCaseObject *element in expected) {
        if (element.variable1 > 0) {
            element.variable2 = object.variable2;
            element.variable3 = object.variable3;
        }
    }
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_update_withObject_where
{
    [self update_withObject_where_test:_table1 withPreInsertedObjects:_preInsertObjects1];
    [self update_withObject_where_test:_migratedTable2 withPreInsertedObjects:_preInsertObjects2];
    [self update_withObject_where_test:_migratedTable3 withPreInsertedObjects:_preInsertObjects3];
}

#pragma mark - Delete
- (void)delete_test:(NSString *)tableName
{
    XCTAssertTrue([_migrated deleteFromTable:tableName]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_migrated getObjectsOfClass:_cls fromTable:tableName orderBy:TestCaseObject.variable1]];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)test_delete
{
    [self delete_test:_table1];
    [self delete_test:_migratedTable2];
    [self delete_test:_migratedTable3];
}

- (void)delete_where_test:(NSString *)tableName
    withPreInsertedObjects:(NSArray<TestCaseObject *> *)preInsertedObjects
{
    XCTAssertTrue([_migrated deleteFromTable:tableName where:_greaterThan0Condition]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_migrated getObjectsOfClass:_cls fromTable:tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray array];
    for (TestCaseObject *object in preInsertedObjects) {
        if (object.variable1 > 0) {
            continue;
        }
        [expected addObject:object];
    }
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_delete_where:(NSString *)tableName
{
    [self delete_where_test:_table1 withPreInsertedObjects:_preInsertObjects1];
    [self delete_where_test:_migratedTable2 withPreInsertedObjects:_preInsertObjects2];
    [self delete_where_test:_migratedTable3 withPreInsertedObjects:_preInsertObjects3];
}

@end
