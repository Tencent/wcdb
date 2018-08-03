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

@interface UnsafeHandleTests_ChainCall : CRUDTestCase

@end

@implementation UnsafeHandleTests_ChainCall {
    NSString *_configName;
}

- (void)setUp
{
    [super setUp];

    _configName = self.className;
}

- (void)tearDown
{
    [_database removeConfigForName:_configName];

    [super tearDown];
}

- (void)verification_test_insert:(WCTHandle *)handle
                      withObject:(TestCaseObject *)object
{
    WCTInsert *insert = [[handle prepareInsert] intoTable:_tableName];
    XCTAssertTrue([insert executeWithObject:object]);
}

- (void)test_insert_object
{
    TestCaseObject *object = [TestCaseObject objectWithId:(int) _preInserted.count];

    __block BOOL tested = NO;
    __weak id weakSelf = self;
    [_database
           setConfig:^BOOL(WCTHandle *_Nonnull unsafeHandle) {
               id strongSelf = weakSelf;
               [strongSelf verification_test_insert:unsafeHandle withObject:object];
               tested = YES;
               return YES;
           }
    withUninvocation:nil
             forName:_configName];
    [_database purge];
    XCTAssertTrue([_database canOpen]);
    XCTAssertTrue(tested);

    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    [expected addObject:object];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)verification_test_update:(WCTHandle *)handle
                      withObject:(TestCaseObject *)object
{
    WCTUpdate *update = [[handle prepareUpdate] table:_tableName];
    XCTAssertTrue(([[update onProperties:{ TestCaseObject.variable2, TestCaseObject.variable3 }] executeWithObject:object]));
}

- (void)test_update_withObject
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];

    __block BOOL tested = NO;
    __weak id weakSelf = self;
    [_database
           setConfig:^BOOL(WCTHandle *_Nonnull unsafeHandle) {
               id strongSelf = weakSelf;
               [strongSelf verification_test_update:unsafeHandle withObject:object];
               tested = YES;
               return YES;
           }
    withUninvocation:nil
             forName:_configName];
    [_database purge];
    XCTAssertTrue([_database canOpen]);
    XCTAssertTrue(tested);

    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    for (TestCaseObject *element in expected) {
        element.variable2 = object.variable2;
        element.variable3 = object.variable3;
    }
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)verification_test_delete:(WCTHandle *)handle
{
    WCTDelete *delete_ = [[handle prepareDelete] fromTable:_tableName];
    XCTAssertTrue([delete_ execute]);
}

- (void)test_delete
{
    __block BOOL tested = NO;
    __weak id weakSelf = self;
    [_database
           setConfig:^BOOL(WCTHandle *_Nonnull unsafeHandle) {
               id strongSelf = weakSelf;
               [strongSelf verification_test_delete:unsafeHandle];
               tested = YES;
               return YES;
           }
    withUninvocation:nil
             forName:_configName];
    [_database purge];
    XCTAssertTrue([_database canOpen]);
    XCTAssertTrue(tested);

    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)verification_test_select:(WCTHandle *)handle
{
    WCTSelect *select = [[handle prepareSelect] fromTable:_tableName];
    NSArray<TestCaseObject *> *objects = [[select ofClass:_cls] allObjects];
    XCTAssertTrue([objects isEqualToTestCaseObjects:_preInserted]);
}

- (void)test_select
{
    __block BOOL tested = NO;
    __weak id weakSelf = self;
    [_database
           setConfig:^BOOL(WCTHandle *_Nonnull unsafeHandle) {
               id strongSelf = weakSelf;
               [strongSelf verification_test_select:unsafeHandle];
               tested = YES;
               return YES;
           }
    withUninvocation:nil
             forName:_configName];
    [_database purge];
    XCTAssertTrue([_database canOpen]);
    XCTAssertTrue(tested);
}

- (void)verification_test_rowSelect:(WCTHandle *)handle
{
    WCTRowSelect *rowSelect = [handle prepareRowSelect];
    WCTColumnsXRows *rows = [[rowSelect fromTable:_tableName] allRows];
    XCTAssertTrue([rows isEqualToObjects:_preInserted
                          withComparator:^BOOL(WCTOneRow *lhs, TestCaseObject *rhs) {
                              if (lhs.count != 3) {
                                  return NO;
                              }
                              return lhs[0].numberValue.intValue == rhs.variable1 && [lhs[1].stringValue isEqualToString:rhs.variable2] && lhs[2].numberValue.doubleValue == rhs.variable3;
                          }]);
}

- (void)test_rowSelect
{
    __block BOOL tested = NO;
    __weak id weakSelf = self;
    [_database
           setConfig:^BOOL(WCTHandle *_Nonnull unsafeHandle) {
               id strongSelf = weakSelf;
               [strongSelf verification_test_rowSelect:unsafeHandle];
               tested = YES;
               return YES;
           }
    withUninvocation:nil
             forName:_configName];
    [_database purge];
    XCTAssertTrue([_database canOpen]);
    XCTAssertTrue(tested);
}

- (void)verification_test_multiSelect:(WCTHandle *)handle
                          withObjects:(NSArray<TestCaseObject *> *)preInserted2
                             andTable:(NSString *)tableName2
{
    WCTPropertyList properties = [_cls allProperties].inTable(_tableName);
    properties.addProperties([_cls allProperties].inTable(tableName2));

    WCTMultiSelect *multiSelect = [[[[handle prepareMultiSelect] onProperties:properties] fromTables:@[ _tableName, tableName2 ]] where:TestCaseObject.variable1.inTable(_tableName) == TestCaseObject.variable1.inTable(tableName2)];

    XCTAssertNotNil(multiSelect);

    NSArray<WCTMultiObject *> *multiObjects = [multiSelect allMultiObjects];
    XCTAssertEqual(multiObjects.count, _preInserted.count);
    for (NSUInteger i = 0; i < multiObjects.count; ++i) {
        TestCaseObject *first = (TestCaseObject *) [multiObjects[i] objectForKey:_tableName];
        XCTAssertTrue([first isEqualToObject:_preInserted[i]]);
        TestCaseObject *second = (TestCaseObject *) [multiObjects[i] objectForKey:tableName2];
        XCTAssertTrue([second isEqualToObject:preInserted2[i]]);
    }
}

- (void)test_multiSelect
{
    NSString *tableName2 = [_tableName stringByAppendingString:@"2"];

    XCTAssertTrue([_database createTableAndIndexes:tableName2 withClass:_cls]);

    NSArray<TestCaseObject *> *preInserted2 = [TestCaseObject objectsWithCount:(int) _preInserted.count];

    XCTAssertTrue([_database insertObjects:preInserted2 intoTable:tableName2]);

    __block BOOL tested = NO;
    __weak id weakSelf = self;
    [_database
           setConfig:^BOOL(WCTHandle *_Nonnull unsafeHandle) {
               id strongSelf = weakSelf;
               [strongSelf verification_test_multiSelect:unsafeHandle
                                             withObjects:preInserted2
                                                andTable:tableName2];
               tested = YES;
               return YES;
           }
    withUninvocation:nil
             forName:_configName];
    [_database purge];
    XCTAssertTrue([_database canOpen]);
    XCTAssertTrue(tested);
}

@end
