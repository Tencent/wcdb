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

@implementation ConvenientInsertTests

- (void)setUp
{
    [super setUp];

    _renewedObject1 = [[TestCaseObject alloc] init];
    _renewedObject1.identifier = 1;
    _renewedObject1.content = [NSString randomString];

    _renewedObject2 = [[TestCaseObject alloc] init];
    _renewedObject2.identifier = 2;
    _renewedObject2.content = [NSString randomString];

    _renewedPartialObject1 = [[TestCaseObject alloc] init];
    _renewedPartialObject1.identifier = 1;
    _renewedPartialObject1.content = nil;

    _renewedPartialObject2 = [[TestCaseObject alloc] init];
    _renewedPartialObject2.identifier = 2;
    _renewedPartialObject2.content = nil;

    _object3 = [[TestCaseObject alloc] init];
    _object3.identifier = 3;
    _object3.content = [NSString randomString];

    _object4 = [[TestCaseObject alloc] init];
    _object4.identifier = 4;
    _object4.content = [NSString randomString];

    _partialObject3 = [[TestCaseObject alloc] init];
    _partialObject3.identifier = 3;
    _partialObject3.content = nil;

    _partialObject4 = [[TestCaseObject alloc] init];
    _partialObject4.identifier = 4;
    _partialObject4.content = nil;
}

- (BOOL)checkObjects:(NSArray<TestCaseObject*>*)objects
            andInsertSQL:(NSString*)insertSQL
               withCount:(int)count
asExpectedAfterInsertion:(BOOL (^)())block
{
    NSMutableArray<NSString*>* sqls = [NSMutableArray array];
    [sqls addObject:@"BEGIN IMMEDIATE"];
    for (int i = 0; i < count; ++i) {
        [sqls addObject:insertSQL];
    }
    [sqls addObject:@"COMMIT"];
    return [self checkObjects:objects andSQLs:sqls asExpectedAfterModification:block];
}

#pragma mark - Auto Increment
- (void)test_auto_increment
{
    TestCaseObject* autoIncrementObject = [[TestCaseObject alloc] init];
    autoIncrementObject.isAutoIncrement = YES;
    autoIncrementObject.content = self.object3.content;
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.object3 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                BOOL result = [self.database insertObject:autoIncrementObject intoTable:self.tableName];
                if (result) {
                    autoIncrementObject.identifier = (int) autoIncrementObject.lastInsertedRowID;
                }
                return result;
            }];
    XCTAssertTrue(result);
    XCTAssertTrue([autoIncrementObject isEqual:self.object3]);
}

#pragma mark - Database - Insert
- (void)test_database_insert_object
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.object3 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [self.database insertObject:self.object3 intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

- (void)test_database_insert_objects
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.object3, self.object4 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [self.database insertObjects:@[ self.object3, self.object4 ] intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

#pragma mark - Database - Insert or Replace
- (void)test_database_insert_or_replace_object
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [self.database insertOrReplaceObject:self.renewedObject1 intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

- (void)test_database_insert_or_replace_objects
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [self.database insertOrReplaceObjects:@[ self.renewedObject1, self.renewedObject2 ] intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

#pragma mark - Database - Partial Insert
- (void)test_database_insert_object_on_properties
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.partialObject3 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [self.database insertObject:self.object3 onProperties:TestCaseObject.identifier intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

- (void)test_database_insert_objects_on_properties
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.partialObject3, self.partialObject4 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [self.database insertObjects:@[ self.object3, self.object4 ] onProperties:TestCaseObject.identifier intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

#pragma mark - Database - Partial Insert or Replace
- (void)test_database_insert_or_replace_object_on_properties
{
    BOOL result = [self checkObjects:@[ self.renewedPartialObject1, self.object2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [self.database insertOrReplaceObject:self.object1 onProperties:TestCaseObject.identifier intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

- (void)test_database_insert_or_replace_objects_on_properties
{
    BOOL result = [self checkObjects:@[ self.renewedPartialObject1, self.renewedPartialObject2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [self.database insertOrReplaceObjects:@[ self.object1, self.object2 ] onProperties:TestCaseObject.identifier intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

#pragma mark - Table - Insert
- (void)test_table_insert_object
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.object3 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [self.table insertObject:self.object3];
            }];
    XCTAssertTrue(result);
}

- (void)test_table_insert_objects
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.object3, self.object4 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [self.table insertObjects:@[ self.object3, self.object4 ]];
            }];
    XCTAssertTrue(result);
}

#pragma mark - Table - Insert or Replace
- (void)test_table_insert_or_replace_object
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [self.table insertOrReplaceObject:self.renewedObject1];
            }];
    XCTAssertTrue(result);
}

- (void)test_table_insert_or_replace_objects
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [self.table insertOrReplaceObjects:@[ self.renewedObject1, self.renewedObject2 ]];
            }];
    XCTAssertTrue(result);
}

#pragma mark - Table - Partial Insert
- (void)test_table_insert_object_on_properties
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.partialObject3 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [self.table insertObject:self.object3 onProperties:TestCaseObject.identifier];
            }];
    XCTAssertTrue(result);
}

- (void)test_table_insert_objects_on_properties
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.partialObject3, self.partialObject4 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [self.table insertObjects:@[ self.object3, self.object4 ] onProperties:TestCaseObject.identifier];
            }];
    XCTAssertTrue(result);
}

#pragma mark - Table - Partial Insert or Replace
- (void)test_table_insert_or_replace_object_on_properties
{
    BOOL result = [self checkObjects:@[ self.renewedPartialObject1, self.object2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [self.table insertOrReplaceObject:self.object1 onProperties:TestCaseObject.identifier];
            }];
    XCTAssertTrue(result);
}

- (void)test_table_insert_or_replace_objects_on_properties
{
    BOOL result = [self checkObjects:@[ self.renewedPartialObject1, self.renewedPartialObject2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [self.table insertOrReplaceObjects:@[ self.object1, self.object2 ] onProperties:TestCaseObject.identifier];
            }];
    XCTAssertTrue(result);
}

#pragma mark - Handle - Insert
- (void)test_handle_insert_object
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.object3 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [[self.database getHandle] insertObject:self.object3 intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

- (void)test_handle_insert_objects
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.object3, self.object4 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [[self.database getHandle] insertObjects:@[ self.object3, self.object4 ] intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

#pragma mark - Handle - Insert or Replace
- (void)test_handle_insert_or_replace_object
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.object2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [[self.database getHandle] insertOrReplaceObject:self.renewedObject1 intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

- (void)test_handle_insert_or_replace_objects
{
    BOOL result = [self checkObjects:@[ self.renewedObject1, self.renewedObject2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [[self.database getHandle] insertOrReplaceObjects:@[ self.renewedObject1, self.renewedObject2 ] intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

#pragma mark - Handle - Partial Insert
- (void)test_handle_insert_object_on_properties
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.partialObject3 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [[self.database getHandle] insertObject:self.object3 onProperties:TestCaseObject.identifier intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

- (void)test_handle_insert_objects_on_properties
{
    BOOL result = [self checkObjects:@[ self.object1, self.object2, self.partialObject3, self.partialObject4 ]
                        andInsertSQL:@"INSERT INTO main.testTable(identifier) VALUES(?1)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [[self.database getHandle] insertObjects:@[ self.object3, self.object4 ] onProperties:TestCaseObject.identifier intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

#pragma mark - Handle - Partial Insert or Replace
- (void)test_handle_insert_or_replace_object_on_properties
{
    BOOL result = [self checkObjects:@[ self.renewedPartialObject1, self.object2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [[self.database getHandle] insertOrReplaceObject:self.object1 onProperties:TestCaseObject.identifier intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

- (void)test_handle_insert_or_replace_objects_on_properties
{
    BOOL result = [self checkObjects:@[ self.renewedPartialObject1, self.renewedPartialObject2 ]
                        andInsertSQL:@"INSERT OR REPLACE INTO main.testTable(identifier) VALUES(?1)"
                           withCount:2
            asExpectedAfterInsertion:^BOOL {
                return [[self.database getHandle] insertOrReplaceObjects:@[ self.object1, self.object2 ] onProperties:TestCaseObject.identifier intoTable:self.tableName];
            }];
    XCTAssertTrue(result);
}

@end
