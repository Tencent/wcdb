//
// Created by qiuwenchen on 2022/10/26.
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
#import <Foundation/Foundation.h>

@interface ORMMultiSelectTests : CPPCRUDTestCase

@property (nonatomic, assign) NSString* tableName2;

@property (nonatomic, assign) CPPTestCaseObject object1InTable2;
@property (nonatomic, assign) CPPTestCaseObject object2InTable2;
@property (nonatomic, assign) WCDB::ValueArray<CPPTestCaseObject> objectsInTable2;

@end

@implementation ORMMultiSelectTests

- (void)setUp
{
    [super setUp];

    self.tableName2 = @"testTable2";
    self.object1InTable2 = [[Random shared] testCaseObjectWithIdentifier:1];
    self.object2InTable2 = [[Random shared] testCaseObjectWithIdentifier:2];
    self.objectsInTable2 = { self.object1InTable2, self.object2InTable2 };

    [self insertPresetObjects];
    TestCaseAssertTrue(self.database->createTable<CPPTestCaseObject>(self.tableName2.UTF8String));
    TestCaseAssertTrue(self.database->insertObjects(self.objectsInTable2, self.tableName2.UTF8String));
    self.database->close();
}

- (void)tearDown
{
    TestCaseAssertTrue(self.database->dropTable(self.tableName2.UTF8String));
    [super tearDown];
}

- (void)doTestMultiObjects:(const WCDB::ValueArray<WCDB::MultiObject>&)multiObjects
                    andSQL:(NSString*)sql
               bySelecting:(WCDB::OptionalMultiObjectArray (^)())block
{
    TestCaseAssertTrue(multiObjects.size() > 0);
    TestCaseAssertTrue(sql.length > 0);
    __block WCDB::ValueArray<WCDB::MultiObject> results;
    [self doTestSQLs:@[ sql ]
         inOperation:^BOOL {
             auto values = block();
             TestCaseAssertTrue(values.succeed());
             results = values.value();
             return results.size() > 0;
         }];
    TestCaseAssertTrue(multiObjects.size() == results.size());
    for (int i = 0; i < multiObjects.size(); i++) {
        TestCaseAssertTrue(multiObjects[i] == results[i]);
    }
}

#pragma mark - Database
- (void)test_database_next
{
    WCDB::MultiObject multiObj;
    multiObj.addObject(self.object1, self.tableName.UTF8String);
    multiObj.addObject(self.object1InTable2, self.tableName2.UTF8String);
    TestCaseAssertTrue(self.object1 == multiObj.objectAtTable<CPPTestCaseObject>(self.tableName.UTF8String).value());
    TestCaseAssertTrue(self.object1InTable2 == multiObj.objectAtTable<CPPTestCaseObject>(self.tableName2.UTF8String).value());
    [self doTestMultiObjects:multiObj
                      andSQL:@"SELECT testTable.identifier, testTable.content, testTable2.identifier, testTable2.content FROM testTable, testTable2 WHERE testTable.identifier == testTable2.identifier"
                 bySelecting:^WCDB::OptionalMultiObjectArray {
                     WCDB::ResultFields resultColumns
                     = CPPTestCaseObject::allFields()
                       .redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
                           return field.table(self.tableName.UTF8String);
                       })
                       .addingNewResultColumns(CPPTestCaseObject::allFields().redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
                           return field.table(self.tableName2.UTF8String);
                       }));

                     WCDB::MultiSelect select = self.database->prepareMultiSelect().onResultFields(resultColumns).fromTables({ self.tableName.UTF8String, self.tableName2.UTF8String }).where(WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName.UTF8String) == WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName2.UTF8String));
                     return select.firstMultiObject();
                 }];
}

- (void)test_database_all
{
    WCDB::MultiObject multiObj1;
    multiObj1.addObject(self.object1, self.tableName.UTF8String);
    multiObj1.addObject(self.object1InTable2, self.tableName2.UTF8String);
    TestCaseAssertTrue(self.object1 == multiObj1.objectAtTable<CPPTestCaseObject>(self.tableName.UTF8String).value());
    TestCaseAssertTrue(self.object1InTable2 == multiObj1.objectAtTable<CPPTestCaseObject>(self.tableName2.UTF8String).value());
    WCDB::MultiObject multiObj2;
    multiObj2.addObject(self.object2, self.tableName.UTF8String);
    multiObj2.addObject(self.object2InTable2, self.tableName2.UTF8String);
    TestCaseAssertTrue(self.object2 == multiObj2.objectAtTable<CPPTestCaseObject>(self.tableName.UTF8String).value());
    TestCaseAssertTrue(self.object2InTable2 == multiObj2.objectAtTable<CPPTestCaseObject>(self.tableName2.UTF8String).value());
    [self doTestMultiObjects:{ multiObj1, multiObj2 }
                      andSQL:@"SELECT testTable.identifier, testTable.content, testTable2.identifier, testTable2.content FROM testTable, testTable2 WHERE testTable.identifier == testTable2.identifier"
                 bySelecting:^WCDB::OptionalMultiObjectArray {
                     WCDB::ResultFields resultColumns
                     = CPPTestCaseObject::allFields()
                       .redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
                           return field.table(self.tableName.UTF8String);
                       })
                       .addingNewResultColumns(CPPTestCaseObject::allFields().redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
                           return field.table(self.tableName2.UTF8String);
                       }));
                     WCDB::MultiSelect select = self.database->prepareMultiSelect().onResultFields(resultColumns).fromTables({ self.tableName.UTF8String, self.tableName2.UTF8String }).where(WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName.UTF8String) == WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName2.UTF8String));
                     return select.allMultiObjects();
                 }];
}

#pragma mark - Handle
- (void)test_handle_next
{
    WCDB::MultiObject multiObj;
    multiObj.addObject(self.object1, self.tableName.UTF8String);
    multiObj.addObject(self.object1InTable2, self.tableName2.UTF8String);
    TestCaseAssertTrue(self.object1 == multiObj.objectAtTable<CPPTestCaseObject>(self.tableName.UTF8String).value());
    TestCaseAssertTrue(self.object1InTable2 == multiObj.objectAtTable<CPPTestCaseObject>(self.tableName2.UTF8String).value());
    [self doTestMultiObjects:multiObj
                      andSQL:@"SELECT testTable.identifier, testTable.content, testTable2.identifier, testTable2.content FROM testTable, testTable2 WHERE testTable.identifier == testTable2.identifier"
                 bySelecting:^WCDB::OptionalMultiObjectArray {
                     WCDB::ResultFields resultColumns
                     = CPPTestCaseObject::allFields()
                       .redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
                           return field.table(self.tableName.UTF8String);
                       })
                       .addingNewResultColumns(CPPTestCaseObject::allFields().redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
                           return field.table(self.tableName2.UTF8String);
                       }));

                     WCDB::MultiSelect select = self.database->getHandle().prepareMultiSelect().onResultFields(resultColumns).fromTables({ self.tableName.UTF8String, self.tableName2.UTF8String }).where(WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName.UTF8String) == WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName2.UTF8String));
                     return select.firstMultiObject();
                 }];
}

- (void)test_handle_all
{
    WCDB::MultiObject multiObj1;
    multiObj1.addObject(self.object1, self.tableName.UTF8String);
    multiObj1.addObject(self.object1InTable2, self.tableName2.UTF8String);
    TestCaseAssertTrue(self.object1 == multiObj1.objectAtTable<CPPTestCaseObject>(self.tableName.UTF8String).value());
    TestCaseAssertTrue(self.object1InTable2 == multiObj1.objectAtTable<CPPTestCaseObject>(self.tableName2.UTF8String).value());
    WCDB::MultiObject multiObj2;
    multiObj2.addObject(self.object2, self.tableName.UTF8String);
    multiObj2.addObject(self.object2InTable2, self.tableName2.UTF8String);
    TestCaseAssertTrue(self.object2 == multiObj2.objectAtTable<CPPTestCaseObject>(self.tableName.UTF8String).value());
    TestCaseAssertTrue(self.object2InTable2 == multiObj2.objectAtTable<CPPTestCaseObject>(self.tableName2.UTF8String).value());
    [self doTestMultiObjects:{ multiObj1, multiObj2 }
                      andSQL:@"SELECT testTable.identifier, testTable.content, testTable2.identifier, testTable2.content FROM testTable, testTable2 WHERE testTable.identifier == testTable2.identifier"
                 bySelecting:^WCDB::OptionalMultiObjectArray {
                     WCDB::ResultFields resultColumns
                     = CPPTestCaseObject::allFields()
                       .redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
                           return field.table(self.tableName.UTF8String);
                       })
                       .addingNewResultColumns(CPPTestCaseObject::allFields().redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
                           return field.table(self.tableName2.UTF8String);
                       }));
                     WCDB::MultiSelect select = self.database->getHandle().prepareMultiSelect().onResultFields(resultColumns).fromTables({ self.tableName.UTF8String, self.tableName2.UTF8String }).where(WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName.UTF8String) == WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName2.UTF8String));
                     return select.allMultiObjects();
                 }];
}

@end
