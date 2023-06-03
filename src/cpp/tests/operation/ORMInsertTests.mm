//
// Created by qiuwenchen on 2022/9/7.
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

@interface ORMInsertTests : CPPCRUDTestCase

@property (nonatomic, assign) CPPTestCaseObject renewedObject1;
@property (nonatomic, assign) CPPTestCaseObject renewedObject2;

@property (nonatomic, assign) CPPTestCaseObject renewedPartialObject1;
@property (nonatomic, assign) CPPTestCaseObject renewedPartialObject2;

@property (nonatomic, assign) CPPTestCaseObject object3;
@property (nonatomic, assign) CPPTestCaseObject object4;

@property (nonatomic, assign) CPPTestCaseObject partialObject3;
@property (nonatomic, assign) CPPTestCaseObject partialObject4;

@end

@implementation ORMInsertTests

- (void)setUp
{
    [super setUp];
    [self insertPresetObjects];
    self.renewedObject1 = [Random.shared testCaseObjectWithIdentifier:1];
    self.renewedObject2 = [Random.shared testCaseObjectWithIdentifier:2];
    self.renewedPartialObject1 = CPPTestCaseObject(1);
    self.renewedPartialObject2 = CPPTestCaseObject(2);
    self.object3 = [Random.shared testCaseObjectWithIdentifier:3];
    self.object4 = [Random.shared testCaseObjectWithIdentifier:4];
    self.partialObject3 = CPPTestCaseObject(3);
    self.partialObject4 = CPPTestCaseObject(4);
}

#pragma mark - Auto Increment
- (void)test_auto_increment
{
    __block CPPTestCaseObject autoIncrementObject = CPPTestCaseObject::autoIncrementObject(self.object3.content);
    [self doTestObjects:{ self.object1, self.object2, self.object3 }
              andNumber:1
           ofInsertSQLs:@"INSERT INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             BOOL result = self.database->insertObjects<CPPTestCaseObject>(autoIncrementObject, self.tableName.UTF8String);
             if (result) {
                 autoIncrementObject.identifier = (int) *autoIncrementObject.lastInsertedRowID;
             }
             return result;
         }];
    TestCaseAssertTrue(autoIncrementObject == self.object3);
}

- (void)test_auto_increment_with_partial_insert
{
    __block CPPTestCaseObject autoIncrementObject = CPPTestCaseObject::autoIncrementObject(self.object3.content);
    [self doTestObjects:{ self.object1, self.object2, self.object3 }
              andNumber:1
           ofInsertSQLs:@"INSERT INTO testTable(content) VALUES(?1)"
         afterInsertion:^BOOL {
             BOOL result = self.database->insertObjects<CPPTestCaseObject>(autoIncrementObject, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::content));
             if (result) {
                 autoIncrementObject.identifier = (int) *autoIncrementObject.lastInsertedRowID;
             }
             return result;
         }];
    TestCaseAssertTrue(autoIncrementObject == self.object3);
}

#pragma mark - Database - Insert
- (void)test_database_insert_object
{
    [self doTestObjects:{ self.object1, self.object2, self.object3 }
              andNumber:1
           ofInsertSQLs:@"INSERT INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->insertObjects<CPPTestCaseObject>(self.object3, self.tableName.UTF8String);
         }];
}

- (void)test_database_insert_objects
{
    [self doTestObjects:{ self.object1, self.object2, self.object3, self.object4 }
              andNumber:2
           ofInsertSQLs:@"INSERT INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->insertObjects<CPPTestCaseObject>({ self.object3, self.object4 }, self.tableName.UTF8String);
         }];
}

#pragma mark - Database - Insert or Replace
- (void)test_database_insert_or_replace_object
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->insertOrReplaceObjects<CPPTestCaseObject>(self.renewedObject1, self.tableName.UTF8String);
         }];
}

- (void)test_database_insert_or_replace_objects
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->insertOrReplaceObjects<CPPTestCaseObject>({ self.renewedObject1, self.renewedObject2 }, self.tableName.UTF8String);
         }];
}

#pragma mark - Database - Insert or Ignore
- (void)test_database_insert_or_ignore_object
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->insertOrIgnoreObjects<CPPTestCaseObject>(self.renewedObject1, self.tableName.UTF8String);
         }];
}

- (void)test_database_insert_or_ignore_objects
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->insertOrIgnoreObjects<CPPTestCaseObject>({ self.renewedObject1, self.renewedObject2 }, self.tableName.UTF8String);
         }];
}

#pragma mark - Database - Partial Insert
- (void)test_database_insert_object_on_properties
{
    [self doTestObjects:{ self.object1, self.object2, self.partialObject3 }
              andNumber:1
           ofInsertSQLs:@"INSERT INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->insertObjects<CPPTestCaseObject>(self.object3, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

- (void)test_database_insert_objects_on_properties
{
    [self doTestObjects:{ self.object1, self.object2, self.partialObject3, self.partialObject4 }
              andNumber:2
           ofInsertSQLs:@"INSERT INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->insertObjects<CPPTestCaseObject>({ self.object3, self.object4 }, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

#pragma mark - Database - Partial Insert or Replace
- (void)test_database_insert_or_replace_object_on_properties
{
    [self doTestObjects:{ self.renewedPartialObject1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->insertOrReplaceObjects<CPPTestCaseObject>(self.object1, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

- (void)test_database_insert_or_replace_objects_on_properties
{
    [self doTestObjects:{ self.renewedPartialObject1, self.renewedPartialObject2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->insertOrReplaceObjects<CPPTestCaseObject>({ self.object1, self.object2 }, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

#pragma mark - Database - Partial Insert or Ignore
- (void)test_database_insert_or_ignore_object_on_properties
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->insertOrIgnoreObjects<CPPTestCaseObject>(self.object1, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

- (void)test_database_insert_or_ignore_objects_on_properties
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->insertOrIgnoreObjects<CPPTestCaseObject>({ self.object1, self.object2 }, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

#pragma mark - Table - Insert
- (void)test_table_insert_object
{
    [self doTestObjects:{ self.object1, self.object2, self.object3 }
              andNumber:1
           ofInsertSQLs:@"INSERT INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.table.insertObjects(self.object3);
         }];
}

- (void)test_table_insert_objects
{
    [self doTestObjects:{ self.object1, self.object2, self.object3, self.object4 }
              andNumber:2
           ofInsertSQLs:@"INSERT INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.table.insertObjects({ self.object3, self.object4 });
         }];
}

#pragma mark - Table - Insert or Replace
- (void)test_table_insert_or_replace_object
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.table.insertOrReplaceObjects(self.renewedObject1);
         }];
}

- (void)test_table_insert_or_replace_objects
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.table.insertOrReplaceObjects({ self.renewedObject1, self.renewedObject2 });
         }];
}

#pragma mark - Table - Insert or Ignore
- (void)test_table_insert_or_ignore_object
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.table.insertOrIgnoreObjects(self.renewedObject1);
         }];
}

- (void)test_table_insert_or_ignore_objects
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.table.insertOrIgnoreObjects({ self.renewedObject1, self.renewedObject2 });
         }];
}

#pragma mark - Table - Partial Insert
- (void)test_table_insert_object_on_properties
{
    [self doTestObjects:{ self.object1, self.object2, self.partialObject3 }
              andNumber:1
           ofInsertSQLs:@"INSERT INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.table.insertObjects(self.object3, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

- (void)test_table_insert_objects_on_properties
{
    [self doTestObjects:{ self.object1, self.object2, self.partialObject3, self.partialObject4 }
              andNumber:2
           ofInsertSQLs:@"INSERT INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.table.insertObjects({ self.object3, self.object4 }, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

#pragma mark - Table - Partial Insert or Replace
- (void)test_table_insert_or_replace_object_on_properties
{
    [self doTestObjects:{ self.renewedPartialObject1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.table.insertOrReplaceObjects(self.object1, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

- (void)test_table_insert_or_replace_objects_on_properties
{
    [self doTestObjects:{ self.renewedPartialObject1, self.renewedPartialObject2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.table.insertOrReplaceObjects({ self.object1, self.object2 }, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

#pragma mark - Table - Partial Insert or Ignore
- (void)test_table_insert_or_ignore_object_on_properties
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.table.insertOrIgnoreObjects(self.object1, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

- (void)test_table_insert_or_ignore_objects_on_properties
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.table.insertOrIgnoreObjects({ self.object1, self.object2 }, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

#pragma mark - Handle - Insert
- (void)test_handle_insert_object
{
    [self doTestObjects:{ self.object1, self.object2, self.object3 }
              andNumber:1
           ofInsertSQLs:@"INSERT INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertObjects<CPPTestCaseObject>(self.object3, self.tableName.UTF8String);
         }];
}

- (void)test_handle_insert_objects
{
    [self doTestObjects:{ self.object1, self.object2, self.object3, self.object4 }
              andNumber:2
           ofInsertSQLs:@"INSERT INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertObjects<CPPTestCaseObject>({ self.object3, self.object4 }, self.tableName.UTF8String);
         }];
}

#pragma mark - Handle - Insert or Replace
- (void)test_handle_insert_or_replace_object
{
    [self doTestObjects:{ self.renewedObject1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertOrReplaceObjects<CPPTestCaseObject>(self.renewedObject1, self.tableName.UTF8String);
         }];
}

- (void)test_handle_insert_or_replace_objects
{
    [self doTestObjects:{ self.renewedObject1, self.renewedObject2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertOrReplaceObjects<CPPTestCaseObject>({ self.renewedObject1, self.renewedObject2 }, self.tableName.UTF8String);
         }];
}

#pragma mark - Handle - Insert or Ignore
- (void)test_handle_insert_or_ignore_object
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertOrIgnoreObjects<CPPTestCaseObject>(self.renewedObject1, self.tableName.UTF8String);
         }];
}

- (void)test_handle_insert_or_ignore_objects
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier, content) VALUES(?1, ?2)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertOrIgnoreObjects<CPPTestCaseObject>({ self.renewedObject1, self.renewedObject2 }, self.tableName.UTF8String);
         }];
}

#pragma mark - Handle - Partial Insert
- (void)test_handle_insert_object_on_properties
{
    [self doTestObjects:{ self.object1, self.object2, self.partialObject3 }
              andNumber:1
           ofInsertSQLs:@"INSERT INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertObjects<CPPTestCaseObject>(self.object3, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

- (void)test_handle_insert_objects_on_properties
{
    [self doTestObjects:{ self.object1, self.object2, self.partialObject3, self.partialObject4 }
              andNumber:2
           ofInsertSQLs:@"INSERT INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertObjects<CPPTestCaseObject>({ self.object3, self.object4 }, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

#pragma mark - Handle - Partial Insert or Replace
- (void)test_handle_insert_or_replace_object_on_properties
{
    [self doTestObjects:{ self.renewedPartialObject1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertOrReplaceObjects<CPPTestCaseObject>(self.object1, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

- (void)test_handle_insert_or_replace_objects_on_properties
{
    [self doTestObjects:{ self.renewedPartialObject1, self.renewedPartialObject2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR REPLACE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertOrReplaceObjects<CPPTestCaseObject>({ self.object1, self.object2 }, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

#pragma mark - Handle - Partial Insert or Ignore
- (void)test_handle_insert_or_ignore_object_on_properties
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:1
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertOrIgnoreObjects<CPPTestCaseObject>(self.object1, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

- (void)test_handle_insert_or_ignore_objects_on_properties
{
    [self doTestObjects:{ self.object1, self.object2 }
              andNumber:2
           ofInsertSQLs:@"INSERT OR IGNORE INTO testTable(identifier) VALUES(?1)"
         afterInsertion:^BOOL {
             return self.database->getHandle().insertOrIgnoreObjects<CPPTestCaseObject>({ self.object1, self.object2 }, self.tableName.UTF8String, WCDB_FIELD(CPPTestCaseObject::identifier));
         }];
}

@end
