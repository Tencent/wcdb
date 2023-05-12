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

#import "AdditionalORMObject+WCTTableCoding.h"
#import "AdditionalORMObject.h"
#import "AllTypesObject+WCTTableCoding.h"
#import "AllTypesObject.h"
#import "ColumnConstraintAutoIncrement+WCTTableCoding.h"
#import "ColumnConstraintAutoIncrement.h"
#import "ColumnConstraintAutoIncrementAsc+WCTTableCoding.h"
#import "ColumnConstraintAutoIncrementAsc.h"
#import "ColumnConstraintCheck+WCTTableCoding.h"
#import "ColumnConstraintCheck.h"
#import "ColumnConstraintDefault+WCTTableCoding.h"
#import "ColumnConstraintDefault.h"
#import "ColumnConstraintNotNull+WCTTableCoding.h"
#import "ColumnConstraintNotNull.h"
#import "ColumnConstraintPrimary+WCTTableCoding.h"
#import "ColumnConstraintPrimary.h"
#import "ColumnConstraintPrimaryAsc+WCTTableCoding.h"
#import "ColumnConstraintPrimaryAsc.h"
#import "ColumnConstraintPrimaryDesc+WCTTableCoding.h"
#import "ColumnConstraintPrimaryDesc.h"
#import "ColumnConstraintUnique+WCTTableCoding.h"
#import "ColumnConstraintUnique.h"
#import "DropIndexObject+WCTTableCoding.h"
#import "DropIndexObject.h"
#import "IndexObject+WCTTableCoding.h"
#import "IndexObject.h"
#import "NewPropertyObject.h"
#import "NewRemapObject+WCTTableCoding.h"
#import "NewRemapObject.h"
#import "NewlyCreatedTableIndexObject+WCTTableCoding.h"
#import "NewlyCreatedTableIndexObject.h"
#import "OldRemapObject+WCTTableCoding.h"
#import "OldRemapObject.h"
#import "PropertyObject+WCTTableCoding.h"
#import "PropertyObject.h"
#import "TableConstraintObject+WCTTableCoding.h"
#import "TableConstraintObject.h"
#import "TestCase.h"
#import "VirtualTableFTS4Object+WCTTableCoding.h"
#import "VirtualTableFTS4Object.h"
#import "VirtualTableFTS5Object+WCTTableCoding.h"
#import "VirtualTableFTS5Object.h"

@interface ORMTests : TableTestCase

@end

@implementation ORMTests

- (void)setUp
{
    [super setUp];
    self.expectMode = DatabaseTestCaseExpectSomeSQLs;
}

- (void)doTestCreateTableAndIndexSQLsAsExpected:(NSArray<NSString*>*)expected
{
    TestCaseAssertTrue(expected != nil);
    NSMutableArray* sqls = [NSMutableArray array];
    [sqls addObject:@"BEGIN IMMEDIATE"];
    [sqls addObjectsFromArray:expected];
    [sqls addObject:@"COMMIT"];
    [self doTestSQLs:sqls
         inOperation:^BOOL {
             return [self createTable];
         }];
}

- (void)doTestCreateVirtualTableSQLAsExpected:(NSString*)expected
{
    TestCaseAssertTrue(expected != nil);
    self.expectMode = DatabaseTestCaseExpectFirstFewSQLs;
    self.isVirtualTable = YES;
    [self doTestSQLs:@[ expected ]
         inOperation:^BOOL {
             return [self createTable];
         }];
}

#pragma mark - property
- (void)test_property
{
    self.tableClass = PropertyObject.class;
#ifndef WCDB_WECHAT_IOS
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS testTable(property INTEGER, differentName INTEGER)" ];
#else
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS testTable(differentName INTEGER, property INTEGER)" ];
#endif
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_all_types
{
    self.tableClass = AllTypesObject.class;
#ifndef WCDB_WECHAT_IOS
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS testTable(type TEXT PRIMARY KEY, enumNSValue INTEGER, optionNSValue INTEGER, enumValue INTEGER, enumClassValue INTEGER, literalEnumValue INTEGER, trueOrFalseValue INTEGER, yesOrNoValue INTEGER, intValue INTEGER, unsignedIntValue INTEGER, int32Value INTEGER, int64Value INTEGER, uint32Value INTEGER, uint64Value INTEGER, integerValue INTEGER, uintegerValue INTEGER, floatValue REAL, doubleValue REAL, numberValue REAL, dateValue REAL, stringValue TEXT, dataValue BLOB, codingValue BLOB, renamedGSValue INTEGER)" ];
#else
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS testTable(codingValue BLOB, dataValue BLOB, dateValue REAL, doubleValue REAL, enumClassValue INTEGER, enumNSValue INTEGER, enumValue INTEGER, floatValue REAL, int32Value INTEGER, int64Value INTEGER, integerValue INTEGER, intValue INTEGER, literalEnumValue INTEGER, numberValue REAL, optionNSValue INTEGER, renamedGSValue INTEGER, stringValue TEXT, trueOrFalseValue INTEGER, type TEXT PRIMARY KEY, uint32Value INTEGER, uint64Value INTEGER, uintegerValue INTEGER, unsignedIntValue INTEGER, yesOrNoValue INTEGER)" ];
#endif
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];

    AllTypesObject* maxObject = [AllTypesObject maxObject];
    TestCaseAssertTrue([self.table insertObject:maxObject]);

    AllTypesObject* minObject = [AllTypesObject minObject];
    TestCaseAssertTrue([self.table insertObject:minObject]);

    AllTypesObject* emptyObject = [AllTypesObject emptyObject];
    TestCaseAssertTrue([self.table insertObject:emptyObject]);

    AllTypesObject* nilObject = [AllTypesObject nilObject];
    TestCaseAssertTrue([self.table insertObject:nilObject]);

    AllTypesObject* selectedMaxObject = [self.table getObjectWhere:AllTypesObject.type == maxObject.type];
    TestCaseAssertTrue([selectedMaxObject isEqual:maxObject]);

    AllTypesObject* selectedMinObject = [self.table getObjectWhere:AllTypesObject.type == minObject.type];
    TestCaseAssertTrue([selectedMinObject isEqual:minObject]);

    AllTypesObject* selectedEmptyObject = [self.table getObjectWhere:AllTypesObject.type == emptyObject.type];
    TestCaseAssertTrue([selectedEmptyObject isEqual:emptyObject]);

    AllTypesObject* selectedNilObject = [self.table getObjectWhere:AllTypesObject.type == nilObject.type];
    TestCaseAssertTrue([selectedNilObject isEqual:nilObject]);
}

- (void)test_all_properties
{
    TestCaseAssertEqual(2, PropertyObject.allProperties.size());
    TestCaseAssertSQLEqual(PropertyObject.allProperties, @"property, differentName");
}

#pragma mark - column constraint
- (void)test_column_constraint_primary
{
    self.tableClass = ColumnConstraintPrimary.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER PRIMARY KEY)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_new_column
{
    self.tableClass = ColumnConstraintPrimary.class;
    [self createTable];
    [self.database execute:WCDB::StatementSelect().select({ WCDB::Column("newColumn1").table("").schema("newSchema"), WCDB::Column("newColumn2").table("newtable") }).from(self.tableName)];
}

- (void)test_column_constraint_primary_asc
{
    self.tableClass = ColumnConstraintPrimaryAsc.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER PRIMARY KEY ASC)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_primary_desc
{
    self.tableClass = ColumnConstraintPrimaryDesc.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER PRIMARY KEY DESC)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_auto_increment
{
    self.tableClass = ColumnConstraintAutoIncrement.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER PRIMARY KEY AUTOINCREMENT)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_auto_increment_asc
{
    self.tableClass = ColumnConstraintAutoIncrementAsc.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER PRIMARY KEY ASC AUTOINCREMENT)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_unique
{
    self.tableClass = ColumnConstraintUnique.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER UNIQUE)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_default
{
    self.tableClass = ColumnConstraintDefault.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER DEFAULT 1)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_check
{
    self.tableClass = ColumnConstraintCheck.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER CHECK(value > 1))",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

#pragma mark - index
- (void)test_index
{
    self.tableClass = IndexObject.class;
    self.tableName = @"'test@Table'";
    NSArray<NSString*>* expected = @[
#ifndef WCDB_WECHAT_IOS
        @"CREATE TABLE IF NOT EXISTS 'test@Table'(index_ INTEGER, indexAsc INTEGER, indexDesc INTEGER, uniqueIndex INTEGER, uniqueIndexAsc INTEGER, uniqueIndexDesc INTEGER, multiIndex INTEGER, multiIndexAsc INTEGER, multiIndexDesc INTEGER)",
#else
        @"CREATE TABLE IF NOT EXISTS 'test@Table'(index_ INTEGER, indexAsc INTEGER, indexDesc INTEGER, multiIndex INTEGER, multiIndexAsc INTEGER, multiIndexDesc INTEGER, uniqueIndex INTEGER, uniqueIndexAsc INTEGER, uniqueIndexDesc INTEGER)",
#endif
        @"CREATE INDEX IF NOT EXISTS 'test@Table_index' ON 'test@Table'(index_)",
        @"CREATE INDEX IF NOT EXISTS 'test@Table_index_asc' ON 'test@Table'(indexAsc ASC)",
        @"CREATE INDEX IF NOT EXISTS 'test@Table_index_desc' ON 'test@Table'(indexDesc DESC)",
        @"CREATE INDEX IF NOT EXISTS 'test@Table_multi_index' ON 'test@Table'(multiIndex, multiIndexAsc ASC, multiIndexDesc DESC)",
        @"CREATE UNIQUE INDEX IF NOT EXISTS 'test@Table_unique_index' ON 'test@Table'(uniqueIndex)",
        @"CREATE UNIQUE INDEX IF NOT EXISTS 'test@Table_unique_index_asc' ON 'test@Table'(uniqueIndexAsc ASC)",
        @"CREATE UNIQUE INDEX IF NOT EXISTS 'test@Table_unique_index_desc' ON 'test@Table'(uniqueIndexDesc DESC)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];

    [self dropTable];
    self.tableName = nil;
    expected = @[
#ifndef WCDB_WECHAT_IOS
        @"CREATE TABLE IF NOT EXISTS testTable(index_ INTEGER, indexAsc INTEGER, indexDesc INTEGER, uniqueIndex INTEGER, uniqueIndexAsc INTEGER, uniqueIndexDesc INTEGER, multiIndex INTEGER, multiIndexAsc INTEGER, multiIndexDesc INTEGER)",
#else
        @"CREATE TABLE IF NOT EXISTS testTable(index_ INTEGER, indexAsc INTEGER, indexDesc INTEGER, multiIndex INTEGER, multiIndexAsc INTEGER, multiIndexDesc INTEGER, uniqueIndex INTEGER, uniqueIndexAsc INTEGER, uniqueIndexDesc INTEGER)",
#endif
        @"CREATE INDEX IF NOT EXISTS testTable_index ON testTable(index_)",
        @"CREATE INDEX IF NOT EXISTS testTable_index_asc ON testTable(indexAsc ASC)",
        @"CREATE INDEX IF NOT EXISTS testTable_index_desc ON testTable(indexDesc DESC)",
        @"CREATE INDEX IF NOT EXISTS testTable_multi_index ON testTable(multiIndex, multiIndexAsc ASC, multiIndexDesc DESC)",
        @"CREATE UNIQUE INDEX IF NOT EXISTS testTable_unique_index ON testTable(uniqueIndex)",
        @"CREATE UNIQUE INDEX IF NOT EXISTS testTable_unique_index_asc ON testTable(uniqueIndexAsc ASC)",
        @"CREATE UNIQUE INDEX IF NOT EXISTS testTable_unique_index_desc ON testTable(uniqueIndexDesc DESC)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

#pragma mark - table constraint
- (void)test_table_constraint
{
    self.tableClass = TableConstraintObject.class;
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS testTable(multiPrimary INTEGER, multiPrimaryAsc INTEGER, multiPrimaryDesc INTEGER, multiUnique INTEGER, multiUniqueAsc INTEGER, multiUniqueDesc INTEGER, CONSTRAINT multi_primary PRIMARY KEY(multiPrimary, multiPrimaryAsc ASC, multiPrimaryDesc DESC), CONSTRAINT multi_unique UNIQUE(multiUnique, multiUniqueAsc ASC, multiUniqueDesc DESC))" ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

#pragma mark - virtual table
- (void)test_virtual_table_fts3
{
    self.tableClass = VirtualTableFTS4Object.class;
    [self.database addTokenizer:WCTTokenizerOneOrBinary];
    NSString* expected = @"CREATE VIRTUAL TABLE IF NOT EXISTS testTable USING fts4(tokenize = wcdb_one_or_binary, content='contentTable', identifier INTEGER, content TEXT, notindexed=identifier)";
    [self doTestCreateVirtualTableSQLAsExpected:expected];
}

- (void)test_virtual_table_fts5
{
    self.tableClass = VirtualTableFTS5Object.class;
    NSString* expected = @"CREATE VIRTUAL TABLE IF NOT EXISTS testTable USING fts5(tokenize = 'porter', content='contentTable', identifier UNINDEXED, content)";
    [self doTestCreateVirtualTableSQLAsExpected:expected];
}

#pragma mark - addtional object relational mapping
- (void)test_additional_orm
{
    self.tableClass = AdditionalORMObject.class;
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER PRIMARY KEY ON CONFLICT ABORT, CONSTRAINT table_constraint CHECK(value > 10))",
                                      @"CREATE INDEX IF NOT EXISTS testTable_index ON testTable(value ASC)" ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

#pragma mark - remap
- (void)test_remap
{
    self.tableName = @"'test@Table'";
    {
        self.tableClass = OldRemapObject.class;
        NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS 'test@Table'(value INTEGER)" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
    // remap
    {
        self.tableClass = NewRemapObject.class;
        NSArray<NSString*>* expected = @[ @"PRAGMA table_info('test@Table')", @"ALTER TABLE 'test@Table' ADD COLUMN newValue INTEGER", @"CREATE INDEX IF NOT EXISTS 'test@Table_index' ON 'test@Table'(value)" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
    [self dropTable];
    self.tableName = nil;
    {
        self.tableClass = OldRemapObject.class;
        NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER)" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
    // remap
    {
        self.tableClass = NewRemapObject.class;
        NSArray<NSString*>* expected = @[ @"PRAGMA table_info('testTable')", @"ALTER TABLE testTable ADD COLUMN newValue INTEGER", @"CREATE INDEX IF NOT EXISTS testTable_index ON testTable(value)" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
}

- (void)test_remap_with_extra_actions
{
    {
        self.tableClass = OldRemapObject.class;
        NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER)" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
    // remap
    {
        self.tableClass = NewlyCreatedTableIndexObject.class;
        NSArray<NSString*>* expected = @[ @"PRAGMA table_info('testTable')", @"ALTER TABLE testTable ADD COLUMN newValue INTEGER" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
    TestCaseAssertTrue([self dropTable]);
    // newly create
    {
#ifndef WCDB_WECHAT_IOS
        NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS testTable(value INTEGER, newValue INTEGER)", @"CREATE INDEX IF NOT EXISTS testTable_index ON testTable(value)" ];
#else
        NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS testTable(newValue INTEGER, value INTEGER)", @"CREATE INDEX IF NOT EXISTS testTable_index ON testTable(value)" ];
#endif
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
    // drop index
    {
        self.tableClass = DropIndexObject.class;
        NSArray<NSString*>* expected = @[ @"PRAGMA table_info('testTable')", @"DROP INDEX IF EXISTS testTable_index" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
}

#pragma mark - auto add column
- (void)test_auto_add_column
{
    NSString* fakeTable = @"fakeTable";
    TestCaseAssertTrue([self.database createTable:fakeTable withClass:NewPropertyObject.class]);

    [self testAutoAddColumn:NewPropertyObject.insertValue
                  isSucceed:YES
                  byExecute:^bool {
                      return [self.database insertObject:[[NewPropertyObject alloc] init] intoTable:self.tableName];
                  }];

    [self testAutoAddColumn:NewPropertyObject.updateValue
                  isSucceed:YES
                  byExecute:^bool {
                      return [self.database updateTable:self.tableName setProperty:NewPropertyObject.updateValue toValue:@(1)];
                  }];

    [self testAutoAddColumn:NewPropertyObject.deleteValue
                  isSucceed:YES
                  byExecute:^bool {
                      return [self.database deleteFromTable:self.tableName where:NewPropertyObject.deleteValue == 1];
                  }];

    [self testAutoAddColumn:NewPropertyObject.deleteValue
                  isSucceed:YES
                  byExecute:^bool {
                      return [self.database deleteFromTable:self.tableName where:NewPropertyObject.deleteValue.table(self.tableName) == 1];
                  }];

    [self testAutoAddColumn:NewPropertyObject.deleteValue
                  isSucceed:NO
                  byExecute:^bool {
                      return [self.database deleteFromTable:self.tableName where:NewPropertyObject.deleteValue.table(fakeTable) == 1];
                  }];

    [self testAutoAddColumn:NewPropertyObject.deleteValue
                  isSucceed:NO
                  byExecute:^bool {
                      return [self.database deleteFromTable:self.tableName where:NewPropertyObject.deleteValue.table(self.tableName).schema("notExistSchema") == 1];
                  }];

    [self testAutoAddColumn:NewPropertyObject.selectValue
                  isSucceed:YES
                  byExecute:^bool {
                      return [self.database getColumnOnResultColumn:NewPropertyObject.selectValue fromTable:self.tableName] != nil;
                  }];

    [self testAutoAddColumn:NewPropertyObject.selectValue
                  isSucceed:YES
                  byExecute:^bool {
                      return [self.database getColumnOnResultColumn:NewPropertyObject.insertValue fromTable:self.tableName where:NewPropertyObject.selectValue == 1] != nil;
                  }];

    [self testAutoAddColumn:NewPropertyObject.selectValue
                  isSucceed:YES
                  byExecute:^bool {
                      return [self.database getColumnOnResultColumn:NewPropertyObject.insertValue fromTable:self.tableName orders:NewPropertyObject.selectValue] != nil;
                  }];

    [self testAutoAddColumn:NewPropertyObject.selectValue
                  isSucceed:YES
                  byExecute:^bool {
                      return [self.database getColumnOnResultColumn:NewPropertyObject.insertValue fromTable:self.tableName orders:NewPropertyObject.selectValue.table(self.tableName)] != nil;
                  }];

    [self testAutoAddColumn:NewPropertyObject.selectValue
                  isSucceed:NO
                  byExecute:^bool {
                      return [self.database getColumnOnResultColumn:NewPropertyObject.insertValue fromTable:self.tableName orders:NewPropertyObject.selectValue.table(fakeTable)] != nil;
                  }];

    [self testAutoAddColumn:NewPropertyObject.selectValue
                  isSucceed:NO
                  byExecute:^bool {
                      return [self.database getColumnOnResultColumn:NewPropertyObject.insertValue fromTable:self.tableName orders:NewPropertyObject.selectValue.table(self.tableName).schema("notExistSchema")] != nil;
                  }];

    [self testAutoAddColumn:NewPropertyObject.multiSelectValue
                  isSucceed:YES
                  byExecute:^bool {
                      WCTMultiSelect* select = [self.database prepareMultiSelect];
                      [[select onResultColumns:{ NewPropertyObject.multiSelectValue.table(self.tableName), NewPropertyObject.multiSelectValue.table(fakeTable) }] fromTables:@[ self.tableName, fakeTable ]];
                      return select.allMultiObjects != nil;
                  }];

    [self testAutoAddColumn:NewPropertyObject.multiSelectValue
                  isSucceed:NO
                  byExecute:^bool {
                      WCTMultiSelect* select = [self.database prepareMultiSelect];
                      [[select onResultColumns:{ NewPropertyObject.multiSelectValue.table(self.tableName).schema("notExistSchema"), NewPropertyObject.multiSelectValue.table(fakeTable) }] fromTables:@[ self.tableName, fakeTable ]];
                      return select.allMultiObjects != nil;
                  }];

    [self testAutoAddColumn:NewPropertyObject.primaryValue
                  isSucceed:NO
                  byExecute:^bool {
                      return [self.database getColumnOnResultColumn:NewPropertyObject.primaryValue fromTable:self.tableName] != nil;
                  }];

    [self testAutoAddColumn:NewPropertyObject.uniqueValue
                  isSucceed:NO
                  byExecute:^bool {
                      return [self.database getColumnOnResultColumn:NewPropertyObject.uniqueValue fromTable:self.tableName] != nil;
                  }];
}

- (void)testAutoAddColumn:(const WCTProperty&)newProperty isSucceed:(BOOL)isSucceed byExecute:(bool (^)())block
{
    auto createTable = WCDB::StatementCreateTable().createTable(self.tableName);
    auto binding = const_cast<WCTBinding*>(&NewPropertyObject.objectRelationalMapping);
    auto properties = NewPropertyObject.allProperties.propertiesByRemovingProperties(newProperty);
    for (const auto& property : properties) {
        createTable.define(*binding->getColumnDef(property));
    }
    NSString* propertyName = [NSString stringWithUTF8String:newProperty.getDescription().data()];
    TestCaseAssertTrue([self.database execute:createTable]);
    __block bool autoAdded = false;
    [self.database traceError:^(WCTError* error) {
        if (![error.message isEqualToString:@"Auto add column"]) {
            return;
        }
        autoAdded = YES;
        TestCaseAssertTrue([error.userInfo[@"Table"] isEqualToString:self.tableName]);
        TestCaseAssertTrue([error.userInfo[@"Column"] isEqualToString:propertyName]);
    }];
    TestCaseAssertTrue(block() == isSucceed);
    TestCaseAssertTrue(autoAdded || !isSucceed);
    TestCaseAssertTrue([self.database dropTable:self.tableName]);
    [self.database traceError:nil];
}

@end
