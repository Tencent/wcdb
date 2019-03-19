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
#import "FTS3Object+WCTTableCoding.h"
#import "FTS3Object.h"
#import "FTS5Object+WCTTableCoding.h"
#import "FTS5Object.h"
#import "IndexObject+WCTTableCoding.h"
#import "IndexObject.h"
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
#import <TestCase/TestCase.h>

@interface ORMTests : TableTestCase

@end

@implementation ORMTests

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
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(differentName INTEGER, property INTEGER)" ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_all_types
{
    self.tableClass = AllTypesObject.class;
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(codingValue BLOB, dataValue BLOB, dateValue REAL, doubleValue REAL, enumClassValue INTEGER, enumNSValue INTEGER, enumValue INTEGER, floatValue REAL, int32Value INTEGER, int64Value INTEGER, integerValue INTEGER, intValue INTEGER, literalEnumValue INTEGER, numberValue REAL, optionNSValue INTEGER, stringValue TEXT, type TEXT PRIMARY KEY, uint32Value INTEGER, uint64Value INTEGER, uintegerValue INTEGER, unsignedIntValue INTEGER)" ];
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
}

#pragma mark - column constraint
- (void)test_column_constraint_primary
{
    self.tableClass = ColumnConstraintPrimary.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER PRIMARY KEY)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_primary_asc
{
    self.tableClass = ColumnConstraintPrimaryAsc.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER PRIMARY KEY ASC)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_primary_desc
{
    self.tableClass = ColumnConstraintPrimaryDesc.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER PRIMARY KEY DESC)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_auto_increment
{
    self.tableClass = ColumnConstraintAutoIncrement.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER PRIMARY KEY AUTOINCREMENT)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_auto_increment_asc
{
    self.tableClass = ColumnConstraintAutoIncrementAsc.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER PRIMARY KEY ASC AUTOINCREMENT)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_unique
{
    self.tableClass = ColumnConstraintUnique.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER UNIQUE)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_default
{
    self.tableClass = ColumnConstraintDefault.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER DEFAULT 1)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

- (void)test_column_constraint_check
{
    self.tableClass = ColumnConstraintCheck.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER CHECK(value > 1))",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

#pragma mark - index
- (void)test_index
{
    self.tableClass = IndexObject.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(index_ INTEGER, indexAsc INTEGER, indexDesc INTEGER, multiIndex INTEGER, multiIndexAsc INTEGER, multiIndexDesc INTEGER, uniqueIndex INTEGER, uniqueIndexAsc INTEGER, uniqueIndexDesc INTEGER)",
        @"CREATE INDEX IF NOT EXISTS main.testTable_index ON testTable(index_)",
        @"CREATE INDEX IF NOT EXISTS main.testTable_index_asc ON testTable(indexAsc)",
        @"CREATE INDEX IF NOT EXISTS main.testTable_index_desc ON testTable(indexDesc)",
        @"CREATE INDEX IF NOT EXISTS main.testTable_multi_index ON testTable(multiIndex, multiIndexAsc, multiIndexDesc)",
        @"CREATE UNIQUE INDEX IF NOT EXISTS main.testTable_unique_index ON testTable(uniqueIndex)",
        @"CREATE UNIQUE INDEX IF NOT EXISTS main.testTable_unique_index_asc ON testTable(uniqueIndexAsc)",
        @"CREATE UNIQUE INDEX IF NOT EXISTS main.testTable_unique_index_desc ON testTable(uniqueIndexDesc)",
    ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

#pragma mark - table constraint
- (void)test_table_constraint
{
    self.tableClass = TableConstraintObject.class;
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(multiPrimary INTEGER, multiPrimaryAsc INTEGER, multiPrimaryDesc INTEGER, multiUnique INTEGER, multiUniqueAsc INTEGER, multiUniqueDesc INTEGER, CONSTRAINT multi_primary PRIMARY KEY(multiPrimary, multiPrimaryAsc ASC, multiPrimaryDesc DESC), CONSTRAINT multi_unique UNIQUE(multiUnique, multiUniqueAsc ASC, multiUniqueDesc DESC))" ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

#pragma mark - virtual table
- (void)test_fts3
{
    self.tableClass = FTS3Object.class;
    [self.database setTokenizer:WCTTokenizerOneOrBinary];
    NSString* expected = @"CREATE VIRTUAL TABLE IF NOT EXISTS main.testTable USING fts3(tokenize = wcdb_one_or_binary, value INTEGER)";
    [self doTestCreateVirtualTableSQLAsExpected:expected];
}

- (void)test_fts5
{
    self.tableClass = FTS5Object.class;
    NSString* expected = @"CREATE VIRTUAL TABLE IF NOT EXISTS main.testTable USING fts5(tokenize = porter, value)";
    [self doTestCreateVirtualTableSQLAsExpected:expected];
}

#pragma mark - addtional object relational mapping
- (void)test_additional_orm
{
    self.tableClass = AdditionalORMObject.class;
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER PRIMARY KEY ON CONFLICT ABORT, CONSTRAINT table_constraint CHECK(value > 10))",
                                      @"CREATE INDEX IF NOT EXISTS main.testTable_index ON testTable(value ASC)" ];
    [self doTestCreateTableAndIndexSQLsAsExpected:expected];
}

#pragma mark - remap
- (void)test_remap
{
    {
        self.tableClass = OldRemapObject.class;
        NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER)" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
    // remap
    {
        self.tableClass = NewRemapObject.class;
        NSArray<NSString*>* expected = @[ @"PRAGMA main.table_info('testTable')", @"ALTER TABLE main.testTable ADD COLUMN newValue INTEGER", @"CREATE INDEX IF NOT EXISTS main.testTable_index ON testTable(value)" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
}

- (void)test_remap_will_not_affect_index_for_newly_created_table_only
{
    {
        self.tableClass = OldRemapObject.class;
        NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER)" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
    // remap
    {
        self.tableClass = NewlyCreatedTableIndexObject.class;
        NSArray<NSString*>* expected = @[ @"PRAGMA main.table_info('testTable')", @"ALTER TABLE main.testTable ADD COLUMN newValue INTEGER" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
    TestCaseAssertTrue([self dropTable]);
    // newly create
    {
        NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(newValue INTEGER, value INTEGER)", @"CREATE INDEX IF NOT EXISTS main.testTable_index ON testTable(value)" ];
        [self doTestCreateTableAndIndexSQLsAsExpected:expected];
    }
}

@end
