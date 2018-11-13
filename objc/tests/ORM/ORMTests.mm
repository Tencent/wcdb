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

#import "ORMTests.h"
#import "AdditionalORMObject+WCTTableCoding.h"
#import "AdditionalORMObject.h"
#import "BuiltinTypesObject+WCTTableCoding.h"
#import "BuiltinTypesObject.h"
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
#import "NewRebindObject+WCTTableCoding.h"
#import "NewRebindObject.h"
#import "OldRebindObject+WCTTableCoding.h"
#import "OldRebindObject.h"
#import "PropertyObject+WCTTableCoding.h"
#import "PropertyObject.h"
#import "TableConstraintObject+WCTTableCoding.h"
#import "TableConstraintObject.h"

typedef NS_ENUM(NSUInteger, ORMTestsState) {
    ORMTestsStateNotStarted,
    ORMTestsStateTesting,
    ORMTestsStateTested,
    ORMTestsStateFailed,
};

@implementation ORMTests

- (void)setUp
{
    [super setUp];
    self.continueAfterFailure = NO;
}

- (BOOL)checkCreateTableAndIndexSQLsAsExpected:(NSArray<NSString*>*)expected
{
    NSMutableArray* sqls = [NSMutableArray array];
    [sqls addObject:@"BEGIN IMMEDIATE"];
    [sqls addObjectsFromArray:expected];
    [sqls addObject:@"COMMIT"];
    return [self checkAllSQLs:sqls
        asExpectedByOperation:^BOOL {
            return [self createTable];
        }];
}

- (BOOL)checkCreateVirtualTableSQLAsExpected:(NSString*)expected
{
    if (expected.length == 0) {
        TESTCASE_FAILED
        return NO;
    }
    return [self checkSomeSQLs:@[ expected ]
         asExpectedByOperation:^BOOL {
             return [self createVirtualTable];
         }];
}

#pragma mark - property
- (void)test_property
{
    self.tableClass = PropertyObject.class;
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(differentName INTEGER, property INTEGER)" ];
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

- (void)test_builtin_types
{
    self.tableClass = BuiltinTypesObject.class;
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(codingValue BLOB, cppStringValue TEXT, cstringValue TEXT, dataValue BLOB, dateValue REAL, doubleValue REAL, floatValue REAL, int32Value INTEGER, int64Value INTEGER, integerValue INTEGER, intValue INTEGER, numberValue REAL, stringValue TEXT, uint32Value INTEGER, uint64Value INTEGER, uintegerValue INTEGER, unsignedIntValue INTEGER)" ];
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
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
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

- (void)test_column_constraint_primary_asc
{
    self.tableClass = ColumnConstraintPrimaryAsc.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER PRIMARY KEY ASC)",
    ];
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

- (void)test_column_constraint_primary_desc
{
    self.tableClass = ColumnConstraintPrimaryDesc.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER PRIMARY KEY DESC)",
    ];
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

- (void)test_column_constraint_auto_increment
{
    self.tableClass = ColumnConstraintAutoIncrement.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER PRIMARY KEY AUTOINCREMENT)",
    ];
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

- (void)test_column_constraint_auto_increment_asc
{
    self.tableClass = ColumnConstraintAutoIncrementAsc.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER PRIMARY KEY ASC AUTOINCREMENT)",
    ];
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

- (void)test_column_constraint_unique
{
    self.tableClass = ColumnConstraintUnique.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER UNIQUE)",
    ];
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

- (void)test_column_constraint_default
{
    self.tableClass = ColumnConstraintDefault.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER DEFAULT 1)",
    ];
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

- (void)test_column_constraint_check
{
    self.tableClass = ColumnConstraintCheck.class;
    NSArray<NSString*>* expected = @[
        @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER CHECK(value > 1))",
    ];
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
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
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

#pragma mark - table constraint
- (void)test_table_constraint
{
    self.tableClass = TableConstraintObject.class;
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(multiPrimary INTEGER, multiPrimaryAsc INTEGER, multiPrimaryDesc INTEGER, multiUnique INTEGER, multiUniqueAsc INTEGER, multiUniqueDesc INTEGER, CONSTRAINT testTable_multi_primary PRIMARY KEY(multiPrimary, multiPrimaryAsc ASC, multiPrimaryDesc DESC), CONSTRAINT testTable_multi_unique UNIQUE(multiUnique, multiUniqueAsc ASC, multiUniqueDesc DESC))" ];
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

#pragma mark - virtual table
- (void)test_fts3
{
    self.tableClass = FTS3Object.class;
    [self.database setTokenizer:WCTTokenizerWCDB];
    NSString* expected = @"CREATE VIRTUAL TABLE IF NOT EXISTS main.testTable USING fts3(tokenize = WCDB, value INTEGER)";
    TestCaseAssertTrue([self checkCreateVirtualTableSQLAsExpected:expected]);
}

- (void)test_fts5
{
    self.tableClass = FTS5Object.class;
    NSString* expected = @"CREATE VIRTUAL TABLE IF NOT EXISTS main.testTable USING fts5(tokenize = porter, value)";
    TestCaseAssertTrue([self checkCreateVirtualTableSQLAsExpected:expected]);
}

#pragma mark - addtional object relational mapping
- (void)test_additional_orm
{
    self.tableClass = AdditionalORMObject.class;
    NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER PRIMARY KEY ON CONFLICT ABORT, CONSTRAINT testTable_constraint CHECK(value > 10))",
                                      @"CREATE INDEX IF NOT EXISTS main.testTable_index ON testTable(value ASC)" ];
    TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

#pragma mark - rebind
- (void)test_rebind
{
    {
        self.tableClass = OldRebindObject.class;
        NSArray<NSString*>* expected = @[ @"CREATE TABLE IF NOT EXISTS main.testTable(value INTEGER)" ];
        TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
    }
    // rebind
    {
        self.tableClass = NewRebindObject.class;
        NSArray<NSString*>* expected = @[ @"SELECT 1 FROM main.testTable LIMIT 0", @"PRAGMA main.table_info('testTable')", @"ALTER TABLE main.testTable ADD COLUMN newValue INTEGER", @"CREATE INDEX IF NOT EXISTS main.testTable_index ON testTable(value)" ];
        TestCaseAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
    }
}

@end
