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

#import "PreInsertedCRUDTestCase.h"

@interface MultiSelectTests : PreInsertedCRUDTestCase

@property (nonatomic, readonly) NSString* tableName2;

@property (nonatomic, readonly) TestCaseObject* object1InTable2;
@property (nonatomic, readonly) TestCaseObject* object2InTable2;
@property (nonatomic, readonly) NSArray<TestCaseObject*>* objectsInTable2;

@end

@implementation MultiSelectTests

- (void)setUp
{
    [super setUp];

    _object1InTable2 = [[TestCaseObject alloc] init];
    _object1InTable2.identifier = 1;
    _object1InTable2.content = [NSString randomString];

    _object2InTable2 = [[TestCaseObject alloc] init];
    _object2InTable2.identifier = 2;
    _object2InTable2.content = [NSString randomString];

    _objectsInTable2 = @[ _object1InTable2, _object2InTable2 ];

    _tableName2 = @"testTable2";

    TestCaseAssertTrue([self.database createTableAndIndexes:_tableName2 withClass:self.tableClass]);

    TestCaseAssertTrue([self.database insertObjects:_objectsInTable2 intoTable:_tableName2]);
}

- (void)tearDown
{
    TestCaseAssertTrue([self.database dropTable:self.tableName2]);
    [super tearDown];
}

- (BOOL)checkMultiObjects:(NSArray<WCTMultiObject*>*)multiObjects
                   andSQL:(NSString*)sql
               asExpected:(NSArray<WCTMultiObject*>* (^)())block
{
    if (multiObjects == nil
        || sql == nil) {
        TESTCASE_FAILED
        return NO;
    }
    __block NSArray<WCTMultiObject*>* results;
    if (![self checkAllSQLs:@[ sql ]
        asExpectedInOperation:^BOOL {
            results = block();
            return results != nil;
        }]) {
        return NO;
    }
    return [results isEqualToArray:multiObjects];
}

#pragma mark - Database
- (void)test_database_next
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
    BOOL result = [self checkMultiObjects:objects
                                   andSQL:@"SELECT main.testTable.identifier, main.testTable.content, main.testTable2.identifier, main.testTable2.content FROM main.testTable, main.testTable2 WHERE main.testTable.identifier == main.testTable2.identifier"
                               asExpected:^NSArray<WCTMultiObject*>* {
                                   WCTResultColumns resultColumns = TestCaseObject.allProperties.redirect([self](const WCTProperty& property) -> WCDB::ResultColumn {
                                                                                                    return property.table(self.tableName);
                                                                                                })
                                                                    .addingNewResultColumns(TestCaseObject.allProperties.redirect([self](const WCTProperty& property) -> WCDB::ResultColumn {
                                                                        return property.table(self.tableName2);
                                                                    }));

                                   WCTMultiSelect* select = [[[[self.database prepareMultiSelect] onResultColumns:resultColumns] fromTables:@[ self.tableName, self.tableName2 ]] where:TestCaseObject.identifier.table(self.tableName) == TestCaseObject.identifier.table(self.tableName2)];

                                   NSMutableArray<WCTMultiObject*>* objects = [NSMutableArray<WCTMultiObject*> array];
                                   WCTMultiObject* object;
                                   while ((object = [select nextMultiObject])) {
                                       [objects addObject:object];
                                   }
                                   return objects;
                               }];
    TestCaseAssertTrue(result);
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
    BOOL result = [self checkMultiObjects:objects
                                   andSQL:@"SELECT main.testTable.identifier, main.testTable.content, main.testTable2.identifier, main.testTable2.content FROM main.testTable, main.testTable2 WHERE main.testTable.identifier == main.testTable2.identifier"
                               asExpected:^NSArray<WCTMultiObject*>* {
                                   WCTResultColumns resultColumns = TestCaseObject.allProperties.redirect([self](const WCTProperty& property) -> WCDB::ResultColumn {
                                                                                                    return property.table(self.tableName);
                                                                                                })
                                                                    .addingNewResultColumns(TestCaseObject.allProperties.redirect([self](const WCTProperty& property) -> WCDB::ResultColumn {
                                                                        return property.table(self.tableName2);
                                                                    }));

                                   WCTMultiSelect* select = [[[[self.database prepareMultiSelect] onResultColumns:resultColumns] fromTables:@[ self.tableName, self.tableName2 ]] where:TestCaseObject.identifier.table(self.tableName) == TestCaseObject.identifier.table(self.tableName2)];

                                   return [select allMultiObjects];
                               }];
    TestCaseAssertTrue(result);
}

#pragma mark - Handle
- (void)test_handle_next
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
    BOOL result = [self checkMultiObjects:objects
                                   andSQL:@"SELECT main.testTable.identifier, main.testTable.content, main.testTable2.identifier, main.testTable2.content FROM main.testTable, main.testTable2 WHERE main.testTable.identifier == main.testTable2.identifier"
                               asExpected:^NSArray<WCTMultiObject*>* {
                                   WCTResultColumns resultColumns = TestCaseObject.allProperties.redirect([self](const WCTProperty& property) -> WCDB::ResultColumn {
                                                                                                    return property.table(self.tableName);
                                                                                                })
                                                                    .addingNewResultColumns(TestCaseObject.allProperties.redirect([self](const WCTProperty& property) -> WCDB::ResultColumn {
                                                                        return property.table(self.tableName2);
                                                                    }));

                                   WCTMultiSelect* select = [[[[[self.database getHandle] prepareMultiSelect] onResultColumns:resultColumns] fromTables:@[ self.tableName, self.tableName2 ]] where:TestCaseObject.identifier.table(self.tableName) == TestCaseObject.identifier.table(self.tableName2)];

                                   NSMutableArray<WCTMultiObject*>* objects = [NSMutableArray<WCTMultiObject*> array];
                                   WCTMultiObject* object;
                                   while ((object = [select nextMultiObject])) {
                                       [objects addObject:object];
                                   }
                                   return objects;
                               }];
    TestCaseAssertTrue(result);
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
    BOOL result = [self checkMultiObjects:objects
                                   andSQL:@"SELECT main.testTable.identifier, main.testTable.content, main.testTable2.identifier, main.testTable2.content FROM main.testTable, main.testTable2 WHERE main.testTable.identifier == main.testTable2.identifier"
                               asExpected:^NSArray<WCTMultiObject*>* {
                                   WCTResultColumns resultColumns = TestCaseObject.allProperties.redirect([self](const WCTProperty& property) -> WCDB::ResultColumn {
                                                                                                    return property.table(self.tableName);
                                                                                                })
                                                                    .addingNewResultColumns(TestCaseObject.allProperties.redirect([self](const WCTProperty& property) -> WCDB::ResultColumn {
                                                                        return property.table(self.tableName2);
                                                                    }));

                                   WCTMultiSelect* select = [[[[[self.database getHandle] prepareMultiSelect] onResultColumns:resultColumns] fromTables:@[ self.tableName, self.tableName2 ]] where:TestCaseObject.identifier.table(self.tableName) == TestCaseObject.identifier.table(self.tableName2)];

                                   return [select allMultiObjects];
                               }];
    TestCaseAssertTrue(result);
}

@end
