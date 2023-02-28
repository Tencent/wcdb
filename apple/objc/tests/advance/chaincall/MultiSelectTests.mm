//
// Created by sanhuazhang on 2019/05/02
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

#import "TestCase.h"

@interface MultiSelectTests : CRUDTestCase

@property (nonatomic, readonly) NSString* tableName2;

@property (nonatomic, readonly) TestCaseObject* object1InTable2;
@property (nonatomic, readonly) TestCaseObject* object2InTable2;
@property (nonatomic, readonly) NSArray<TestCaseObject*>* objectsInTable2;

@end

@implementation MultiSelectTests {
    NSString* _tableName2;

    TestCaseObject* _object1InTable2;
    TestCaseObject* _object2InTable2;
    NSArray<TestCaseObject*>* _objectsInTable2;
}

- (void)setUp
{
    [super setUp];

    [self insertPresetObjects];
    TestCaseAssertTrue([self.database createTable:self.tableName2 withClass:self.tableClass]);
    TestCaseAssertTrue([self.database insertObjects:self.objectsInTable2 intoTable:self.tableName2]);
    [self.database close];
}

- (NSString*)tableName2
{
    @synchronized(self) {
        if (_tableName2 == nil) {
            _tableName2 = @"testTable2";
        }
        return _tableName2;
    }
}

- (TestCaseObject*)object1InTable2
{
    @synchronized(self) {
        if (_object1InTable2 == nil) {
            _object1InTable2 = [Random.shared testCaseObjectWithIdentifier:1];
        }
        return _object1InTable2;
    }
}

- (TestCaseObject*)object2InTable2
{
    @synchronized(self) {
        if (_object2InTable2 == nil) {
            _object2InTable2 = [Random.shared testCaseObjectWithIdentifier:2];
        }
        return _object2InTable2;
    }
}

- (NSArray<TestCaseObject*>*)objectsInTable2
{
    @synchronized(self) {
        if (_objectsInTable2 == nil) {
            _objectsInTable2 = @[ self.object1InTable2, self.object2InTable2 ];
        }
        return _objectsInTable2;
    }
}

- (void)tearDown
{
    TestCaseAssertTrue([self.database dropTable:self.tableName2]);
    [super tearDown];
}

- (void)doTestMultiObjects:(NSArray<WCTMultiObject*>*)multiObjects
                    andSQL:(NSString*)sql
               bySelecting:(NSArray<WCTMultiObject*>* (^)())block
{
    TestCaseAssertTrue(multiObjects != nil);
    TestCaseAssertTrue(sql != nil);
    __block NSArray<WCTMultiObject*>* results;
    [self doTestSQLs:@[ sql ]
         inOperation:^BOOL {
             results = block();
             return results != nil;
         }];
    TestCaseAssertTrue([results isEqualToArray:multiObjects]);
}

#pragma mark - Database
- (void)test_database_next
{
    NSArray<WCTMultiObject*>* objects = @[
        @{
            self.tableName : self.object1,
            self.tableName2 : self.object1InTable2,
        }
    ];
    [self doTestMultiObjects:objects
                      andSQL:@"SELECT testTable.identifier, testTable.content, testTable2.identifier, testTable2.content FROM testTable, testTable2 WHERE testTable.identifier == testTable2.identifier"
                 bySelecting:^NSArray<WCTMultiObject*>* {
                     WCTResultColumns resultColumns
                     = TestCaseObject.allProperties
                       .redirect(^WCDB::ResultColumn(const WCTProperty& property) {
                           return property.table(self.tableName);
                       })
                       .addingNewResultColumns(TestCaseObject.allProperties.redirect(^WCDB::ResultColumn(const WCTProperty& property) {
                           return property.table(self.tableName2);
                       }));

                     WCTMultiSelect* select =
                     [[[[self.database prepareMultiSelect] onResultColumns:resultColumns] fromTables:@[ self.tableName, self.tableName2 ]] where:TestCaseObject.identifier.table(self.tableName) == TestCaseObject.identifier.table(self.tableName2)];

                     NSMutableArray<WCTMultiObject*>* results = [NSMutableArray<WCTMultiObject*> array];
                     [results addObject:[select firstMultiObject]];
                     return results;
                 }];
}

- (void)test_database_all
{
    NSArray<WCTMultiObject*>* objects = @[
        @{
            self.tableName : self.object1,
            self.tableName2 : self.object1InTable2,
        },
        @{
            self.tableName : self.object2,
            self.tableName2 : self.object2InTable2,
        },
    ];
    [self doTestMultiObjects:objects
                      andSQL:@"SELECT testTable.identifier, testTable.content, testTable2.identifier, testTable2.content FROM testTable, testTable2 WHERE testTable.identifier == testTable2.identifier"
                 bySelecting:^NSArray<WCTMultiObject*>* {
                     WCTResultColumns resultColumns
                     = TestCaseObject.allProperties
                       .redirect(^WCDB::ResultColumn(const WCTProperty& property) {
                           return property.table(self.tableName);
                       })
                       .addingNewResultColumns(TestCaseObject.allProperties.redirect(^WCDB::ResultColumn(const WCTProperty& property) {
                           return property.table(self.tableName2);
                       }));

                     WCTMultiSelect* select = [[[[self.database prepareMultiSelect] onResultColumns:resultColumns] fromTables:@[ self.tableName, self.tableName2 ]] where:TestCaseObject.identifier.table(self.tableName) == TestCaseObject.identifier.table(self.tableName2)];

                     return [select allMultiObjects];
                 }];
}

#pragma mark - Handle
- (void)test_handle_next
{
    NSArray<WCTMultiObject*>* objects = @[
        @{
            self.tableName : self.object1,
            self.tableName2 : self.object1InTable2,
        }
    ];
    [self doTestMultiObjects:objects
                      andSQL:@"SELECT testTable.identifier, testTable.content, testTable2.identifier, testTable2.content FROM testTable, testTable2 WHERE testTable.identifier == testTable2.identifier"
                 bySelecting:^NSArray<WCTMultiObject*>* {
                     WCTResultColumns resultColumns
                     = TestCaseObject.allProperties
                       .redirect(^WCDB::ResultColumn(const WCTProperty& property) {
                           return property.table(self.tableName);
                       })
                       .addingNewResultColumns(TestCaseObject.allProperties.redirect(^WCDB::ResultColumn(const WCTProperty& property) {
                           return property.table(self.tableName2);
                       }));

                     WCTMultiSelect* select = [[[[[self.database getHandle] prepareMultiSelect] onResultColumns:resultColumns] fromTables:@[ self.tableName, self.tableName2 ]] where:TestCaseObject.identifier.table(self.tableName) == TestCaseObject.identifier.table(self.tableName2)];

                     NSMutableArray<WCTMultiObject*>* results = [NSMutableArray<WCTMultiObject*> array];
                     [results addObject:[select firstMultiObject]];
                     return results;
                 }];
}

- (void)test_handle_all
{
    NSArray<WCTMultiObject*>* objects = @[
        @{
            self.tableName : self.object1,
            self.tableName2 : self.object1InTable2,
        },
        @{
            self.tableName : self.object2,
            self.tableName2 : self.object2InTable2,
        },
    ];
    [self doTestMultiObjects:objects
                      andSQL:@"SELECT testTable.identifier, testTable.content, testTable2.identifier, testTable2.content FROM testTable, testTable2 WHERE testTable.identifier == testTable2.identifier"
                 bySelecting:^NSArray<WCTMultiObject*>* {
                     WCTResultColumns resultColumns
                     = TestCaseObject.allProperties
                       .redirect(^WCDB::ResultColumn(const WCTProperty& property) {
                           return property.table(self.tableName);
                       })
                       .addingNewResultColumns(TestCaseObject.allProperties.redirect(^WCDB::ResultColumn(const WCTProperty& property) {
                           return property.table(self.tableName2);
                       }));

                     WCTMultiSelect* select = [[[[[self.database getHandle] prepareMultiSelect] onResultColumns:resultColumns] fromTables:@[ self.tableName, self.tableName2 ]] where:TestCaseObject.identifier.table(self.tableName) == TestCaseObject.identifier.table(self.tableName2)];

                     return [select allMultiObjects];
                 }];
}

@end
