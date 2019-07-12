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

#import "TestCase.h"

@interface ConvenientInsertTests : CRUDTestCase

@property (nonatomic, readonly) TestCaseObject* renewedObject1;
@property (nonatomic, readonly) TestCaseObject* renewedObject2;

@property (nonatomic, readonly) TestCaseObject* renewedPartialObject1;
@property (nonatomic, readonly) TestCaseObject* renewedPartialObject2;

@property (nonatomic, readonly) TestCaseObject* object3;
@property (nonatomic, readonly) TestCaseObject* object4;

@property (nonatomic, readonly) TestCaseObject* partialObject3;
@property (nonatomic, readonly) TestCaseObject* partialObject4;

@end

@implementation ConvenientInsertTests {
    TestCaseObject* _renewedObject1;
    TestCaseObject* _renewedObject2;

    TestCaseObject* _renewedPartialObject1;
    TestCaseObject* _renewedPartialObject2;

    TestCaseObject* _object3;
    TestCaseObject* _object4;

    TestCaseObject* _partialObject3;
    TestCaseObject* _partialObject4;
}

- (void)setUp
{
    [super setUp];
    [self insertPresetObjects];
}

- (TestCaseObject*)renewedObject1
{
    @synchronized(self) {
        if (_renewedObject1 == nil) {
            _renewedObject1 = [Random.shared testCaseObjectWithIdentifier:1];
        }
        return _renewedObject1;
    }
}

- (TestCaseObject*)renewedObject2
{
    @synchronized(self) {
        if (_renewedObject2 == nil) {
            _renewedObject2 = [Random.shared testCaseObjectWithIdentifier:2];
        }
        return _renewedObject2;
    }
}

- (TestCaseObject*)renewedPartialObject1
{
    @synchronized(self) {
        if (_renewedPartialObject1 == nil) {
            _renewedPartialObject1 = [TestCaseObject partialObjectWithIdentifier:1];
        }
        return _renewedPartialObject1;
    }
}

- (TestCaseObject*)renewedPartialObject2
{
    @synchronized(self) {
        if (_renewedPartialObject2 == nil) {
            _renewedPartialObject2 = [TestCaseObject partialObjectWithIdentifier:2];
        }
        return _renewedPartialObject2;
    }
}

- (TestCaseObject*)object3
{
    @synchronized(self) {
        if (_object3 == nil) {
            _object3 = [Random.shared testCaseObjectWithIdentifier:3];
        }
        return _object3;
    }
}

- (TestCaseObject*)object4
{
    @synchronized(self) {
        if (_object4 == nil) {
            _object4 = [Random.shared testCaseObjectWithIdentifier:4];
        }
        return _object4;
    }
}

- (TestCaseObject*)partialObject3
{
    @synchronized(self) {
        if (_partialObject3 == nil) {
            _partialObject3 = [TestCaseObject partialObjectWithIdentifier:3];
        }
        return _partialObject3;
    }
}

- (TestCaseObject*)partialObject4
{
    @synchronized(self) {
        if (_partialObject4 == nil) {
            _partialObject4 = [TestCaseObject partialObjectWithIdentifier:4];
        }
        return _partialObject4;
    }
}

#pragma mark - Auto Increment
- (void)test_auto_increment
{
    TestCaseObject* autoIncrementObject = [TestCaseObject autoIncrementObjectWithContent:self.object3.content];
    [self doTestObjects:@[ self.object1, self.object2, self.object3 ]
              andNumber:1
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             BOOL result = [self.database insertObject:autoIncrementObject intoTable:self.tableName];
             if (result) {
                 autoIncrementObject.identifier = (int) autoIncrementObject.lastInsertedRowID;
             }
             return result;
         }];
    TestCaseAssertTrue([autoIncrementObject isEqual:self.object3]);
}

- (void)test_auto_increment_with_partial_insert
{
    TestCaseObject* autoIncrementObject = [TestCaseObject autoIncrementObjectWithContent:self.object3.content];
    [self doTestObjects:@[ self.object1, self.object2, self.object3 ]
              andNumber:1
           ofInsertSQLs:@"INSERT INTO main.testTable(content) VALUES(?1)"
         afterInsertion:^BOOL {
             BOOL result = [self.database insertObject:autoIncrementObject onProperties:TestCaseObject.content intoTable:self.tableName];
             if (result) {
                 autoIncrementObject.identifier = (int) autoIncrementObject.lastInsertedRowID;
             }
             return result;
         }];
    TestCaseAssertTrue([autoIncrementObject isEqual:self.object3]);
}

#pragma mark - Database - Insert
- (void)test_database_insert_object
{
    [self doTestObjects:@[ self.object1, self.object2, self.object3 ]
              andNumber:1
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [self.database insertObject:self.object3 intoTable:self.tableName];
         }];
}

- (void)test_database_insert_objects
{
    [self doTestObjects:@[ self.object1, self.object2, self.object3, self.object4 ]
              andNumber:2
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [self.database insertObjects:@[ self.object3, self.object4 ] intoTable:self.tableName];
         }];
}

#pragma mark - Database - Insert or Replace
- (void)test_database_insert_or_replace_object
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [self.database insertOrReplaceObject:self.renewedObject1 intoTable:self.tableName];
         }];
}

- (void)test_database_insert_or_replace_objects
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [self.database insertOrReplaceObjects:@[ self.renewedObject1, self.renewedObject2 ] intoTable:self.tableName];
         }];
}

#pragma mark - Database - Partial Insert
- (void)test_database_insert_object_on_properties
{
    [self doTestObjects:@[ self.object1, self.object2, self.partialObject3 ]
              andNumber:1
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [self.database insertObject:self.object3 onProperties:TestCaseObject.identifier intoTable:self.tableName];
         }];
}

- (void)test_database_insert_objects_on_properties
{
    [self doTestObjects:@[ self.object1, self.object2, self.partialObject3, self.partialObject4 ]
              andNumber:2
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [self.database insertObjects:@[ self.object3, self.object4 ] onProperties:TestCaseObject.identifier intoTable:self.tableName];
         }];
}

#pragma mark - Database - Partial Insert or Replace
- (void)test_database_insert_or_replace_object_on_properties
{
    [self doTestObjects:@[ self.renewedPartialObject1, self.object2 ]
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [self.database insertOrReplaceObject:self.object1 onProperties:TestCaseObject.identifier intoTable:self.tableName];
         }];
}

- (void)test_database_insert_or_replace_objects_on_properties
{
    [self doTestObjects:@[ self.renewedPartialObject1, self.renewedPartialObject2 ]
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [self.database insertOrReplaceObjects:@[ self.object1, self.object2 ] onProperties:TestCaseObject.identifier intoTable:self.tableName];
         }];
}

#pragma mark - Table - Insert
- (void)test_table_insert_object
{
    [self doTestObjects:@[ self.object1, self.object2, self.object3 ]
              andNumber:1
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [self.table insertObject:self.object3];
         }];
}

- (void)test_table_insert_objects
{
    [self doTestObjects:@[ self.object1, self.object2, self.object3, self.object4 ]
              andNumber:2
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [self.table insertObjects:@[ self.object3, self.object4 ]];
         }];
}

#pragma mark - Table - Insert or Replace
- (void)test_table_insert_or_replace_object
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [self.table insertOrReplaceObject:self.renewedObject1];
         }];
}

- (void)test_table_insert_or_replace_objects
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [self.table insertOrReplaceObjects:@[ self.renewedObject1, self.renewedObject2 ]];
         }];
}

#pragma mark - Table - Partial Insert
- (void)test_table_insert_object_on_properties
{
    [self doTestObjects:@[ self.object1, self.object2, self.partialObject3 ]
              andNumber:1
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [self.table insertObject:self.object3 onProperties:TestCaseObject.identifier];
         }];
}

- (void)test_table_insert_objects_on_properties
{
    [self doTestObjects:@[ self.object1, self.object2, self.partialObject3, self.partialObject4 ]
              andNumber:2
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [self.table insertObjects:@[ self.object3, self.object4 ] onProperties:TestCaseObject.identifier];
         }];
}

#pragma mark - Table - Partial Insert or Replace
- (void)test_table_insert_or_replace_object_on_properties
{
    [self doTestObjects:@[ self.renewedPartialObject1, self.object2 ]
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [self.table insertOrReplaceObject:self.object1 onProperties:TestCaseObject.identifier];
         }];
}

- (void)test_table_insert_or_replace_objects_on_properties
{
    [self doTestObjects:@[ self.renewedPartialObject1, self.renewedPartialObject2 ]
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [self.table insertOrReplaceObjects:@[ self.object1, self.object2 ] onProperties:TestCaseObject.identifier];
         }];
}

#pragma mark - Handle - Insert
- (void)test_handle_insert_object
{
    [self doTestObjects:@[ self.object1, self.object2, self.object3 ]
              andNumber:1
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [[self.database getHandle] insertObject:self.object3 intoTable:self.tableName];
         }];
}

- (void)test_handle_insert_objects
{
    [self doTestObjects:@[ self.object1, self.object2, self.object3, self.object4 ]
              andNumber:2
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [[self.database getHandle] insertObjects:@[ self.object3, self.object4 ] intoTable:self.tableName];
         }];
}

#pragma mark - Handle - Insert or Replace
- (void)test_handle_insert_or_replace_object
{
    [self doTestObjects:@[ self.renewedObject1, self.object2 ]
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [[self.database getHandle] insertOrReplaceObject:self.renewedObject1 intoTable:self.tableName];
         }];
}

- (void)test_handle_insert_or_replace_objects
{
    [self doTestObjects:@[ self.renewedObject1, self.renewedObject2 ]
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return [[self.database getHandle] insertOrReplaceObjects:@[ self.renewedObject1, self.renewedObject2 ] intoTable:self.tableName];
         }];
}

#pragma mark - Handle - Partial Insert
- (void)test_handle_insert_object_on_properties
{
    [self doTestObjects:@[ self.object1, self.object2, self.partialObject3 ]
              andNumber:1
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [[self.database getHandle] insertObject:self.object3 onProperties:TestCaseObject.identifier intoTable:self.tableName];
         }];
}

- (void)test_handle_insert_objects_on_properties
{
    [self doTestObjects:@[ self.object1, self.object2, self.partialObject3, self.partialObject4 ]
              andNumber:2
           ofInsertSQLs:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [[self.database getHandle] insertObjects:@[ self.object3, self.object4 ] onProperties:TestCaseObject.identifier intoTable:self.tableName];
         }];
}

#pragma mark - Handle - Partial Insert or Replace
- (void)test_handle_insert_or_replace_object_on_properties
{
    [self doTestObjects:@[ self.renewedPartialObject1, self.object2 ]
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [[self.database getHandle] insertOrReplaceObject:self.object1 onProperties:TestCaseObject.identifier intoTable:self.tableName];
         }];
}

- (void)test_handle_insert_or_replace_objects_on_properties
{
    [self doTestObjects:@[ self.renewedPartialObject1, self.renewedPartialObject2 ]
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return [[self.database getHandle] insertOrReplaceObjects:@[ self.object1, self.object2 ] onProperties:TestCaseObject.identifier intoTable:self.tableName];
         }];
}

@end
