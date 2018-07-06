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

#import "TestCaseCommon.h"

@interface HandleTests_Transaction : TestCase

@end

@implementation HandleTests_Transaction {
    WCTHandle *_handle;

    NSString *_tableName;

    Class _cls;

    TestCaseObject *_object1;

    TestCaseObject *_object2;

    TestCaseObject *_object3;

    TestCaseObject *_object4;
}

- (void)setUp
{
    [super setUp];

    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];

    _handle = [database getHandle];

    _tableName = self.className;

    _cls = TestCaseObject.class;

    XCTAssertTrue([_handle createTableAndIndexes:_tableName withClass:_cls]);

    _object1 = [TestCaseObject objectWithId:0];

    _object2 = [TestCaseObject objectWithId:1];

    _object3 = [TestCaseObject objectWithId:2];

    _object4 = [TestCaseObject objectWithId:3];
}

- (void)tearDown
{
    XCTAssertTrue([_handle dropTable:_tableName]);
    WCTDatabase *database = [_handle getDatabase];
    [_handle finalizeHandle];
    [database close:^{
        XCTAssertTrue([database removeFiles]);
    }];
    _handle = nil;
    _tableName = nil;
    _cls = nil;
    _object1 = nil;
    _object2 = nil;
    [super tearDown];
}

- (void)test_commit
{
    BOOL result = [_handle runTransaction:^BOOL(WCTHandle *handle) {
        XCTAssertTrue([handle insertObject:_object1 intoTable:_tableName]);
        return true;
    }];
    XCTAssertTrue(result);
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertEqual(objects.count, 1);
    XCTAssertTrue([objects[0] isEqualToObject:_object1]);
}

- (void)test_rollback
{
    BOOL result = [_handle runTransaction:^BOOL(WCTHandle *handle) {
        XCTAssertTrue([handle insertObject:_object1 intoTable:_tableName]);
        return false;
    }];
    XCTAssertFalse(result);
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertNotNil(objects);
    XCTAssertEqual(objects.count, 0);
}

- (void)test_separated_commit
{
    XCTAssertTrue([_handle beginTransaction]);
    XCTAssertTrue([_handle insertObject:_object1 intoTable:_tableName]);
    XCTAssertTrue([_handle commitOrRollbackTransaction]);
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertEqual(objects.count, 1);
    XCTAssertTrue([objects[0] isEqualToObject:_object1]);
}

- (void)test_separated_rollback
{
    XCTAssertTrue([_handle beginTransaction]);
    XCTAssertTrue([_handle insertObject:_object1 intoTable:_tableName]);
    [_handle rollbackTransaction];
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertNotNil(objects);
    XCTAssertEqual(objects.count, 0);
}

- (void)test_commit_nested
{
    BOOL result = [_handle runNestedTransaction:^BOOL(WCTHandle *handle) {
        XCTAssertTrue([handle insertObject:_object1 intoTable:_tableName]);
        return true;
    }];
    XCTAssertTrue(result);
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertEqual(objects.count, 1);
    XCTAssertTrue([objects[0] isEqualToObject:_object1]);
}

- (void)test_rollback_nested
{
    BOOL result = [_handle runNestedTransaction:^BOOL(WCTHandle *handle) {
        XCTAssertTrue([handle insertObject:_object1 intoTable:_tableName]);
        return false;
    }];
    XCTAssertFalse(result);
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertNotNil(objects);
    XCTAssertEqual(objects.count, 0);
}

- (void)test_separated_commit_nested
{
    XCTAssertTrue([_handle beginNestedTransaction]);
    XCTAssertTrue([_handle insertObject:_object1 intoTable:_tableName]);
    XCTAssertTrue([_handle commitOrRollbackNestedTransaction]);
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertEqual(objects.count, 1);
    XCTAssertTrue([objects[0] isEqualToObject:_object1]);
}

- (void)test_separated_rollback_nested
{
    XCTAssertTrue([_handle beginNestedTransaction]);
    XCTAssertTrue([_handle insertObject:_object1 intoTable:_tableName]);
    [_handle rollbackNestedTransaction];
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertNotNil(objects);
    XCTAssertEqual(objects.count, 0);
}

- (void)test_commit_and_commit_nested
{
    XCTAssertTrue([_handle runTransaction:^BOOL(WCTHandle *handle) {
        XCTAssertTrue([handle insertObject:_object1 intoTable:_tableName]);
        XCTAssertTrue([handle runNestedTransaction:^BOOL(WCTHandle *handle) {
            XCTAssertTrue([handle insertObject:_object2 intoTable:_tableName]);
            return true;
        }]);
        return true;
    }]);
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertEqual(objects.count, 2);
    XCTAssertTrue([objects[0] isEqualToObject:_object1]);
    XCTAssertTrue([objects[1] isEqualToObject:_object2]);
}

- (void)test_commit_but_rollback_nested
{
    XCTAssertTrue([_handle runTransaction:^BOOL(WCTHandle *handle) {
        XCTAssertTrue([handle insertObject:_object1 intoTable:_tableName]);
        XCTAssertFalse([handle runNestedTransaction:^BOOL(WCTHandle *handle) {
            XCTAssertTrue([handle insertObject:_object2 intoTable:_tableName]);
            return false;
        }]);
        return true;
    }]);
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertEqual(objects.count, 1);
    XCTAssertTrue([objects[0] isEqualToObject:_object1]);
}

- (void)test_rollback_but_commit_nested
{
    XCTAssertFalse([_handle runTransaction:^BOOL(WCTHandle *handle) {
        XCTAssertTrue([handle insertObject:_object1 intoTable:_tableName]);
        XCTAssertTrue([handle runNestedTransaction:^BOOL(WCTHandle *handle) {
            XCTAssertTrue([handle insertObject:_object2 intoTable:_tableName]);
            return true;
        }]);
        return false;
    }]);
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertNotNil(objects);
    XCTAssertEqual(objects.count, 0);
}

- (void)test_rollback_and_rollback_nested
{
    XCTAssertFalse([_handle runTransaction:^BOOL(WCTHandle *handle) {
        XCTAssertTrue([handle insertObject:_object1 intoTable:_tableName]);
        XCTAssertFalse([handle runNestedTransaction:^BOOL(WCTHandle *handle) {
            XCTAssertTrue([handle insertObject:_object2 intoTable:_tableName]);
            return false;
        }]);
        return false;
    }]);
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertNotNil(objects);
    XCTAssertEqual(objects.count, 0);
}

- (void)test_interspersed
{
    XCTAssertTrue([_handle runTransaction:^BOOL(WCTHandle *handle) {
        XCTAssertTrue([handle insertObject:_object1 intoTable:_tableName]);
        XCTAssertFalse([handle runNestedTransaction:^BOOL(WCTHandle *handle) {
            XCTAssertTrue([handle insertObject:_object2 intoTable:_tableName]);
            return false;
        }]);
        XCTAssertTrue([handle runNestedTransaction:^BOOL(WCTHandle *handle) {
            XCTAssertTrue([handle insertObject:_object3 intoTable:_tableName]);
            XCTAssertFalse([handle runNestedTransaction:^BOOL(WCTHandle *handle) {
                XCTAssertTrue([handle insertObject:_object4 intoTable:_tableName]);
                return false;
            }]);
            return true;
        }]);
        return true;
    }]);
    NSArray<TestCaseObject *> *objects = [_handle getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1.asOrder(WCTOrderedAscending)];
    XCTAssertEqual(objects.count, 2);
    XCTAssertTrue([objects[0] isEqualToObject:_object1]);
    XCTAssertTrue([objects[1] isEqualToObject:_object3]);
}

@end
