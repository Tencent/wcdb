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

#import "WTCBaseTestCase.h"
#import "WTCTableAutoFitBaselineObject+WCTTableCoding.h"
#import "WTCTableAutoFitBaselineObject.h"
#import "WTCTableBaselineObject+WCTTableCoding.h"
#import "WTCTableBaselineObject.h"
#import "WTCTableConstraintObject+WCTTableCoding.h"
#import "WTCTableConstraintObject.h"
#import "WTCTableIndexObject+WCTTableCoding.h"
#import "WTCTableIndexObject.h"
#import "WTCTableSkipColumnObject+WCTTableCoding.h"
#import "WTCTableSkipColumnObject.h"
#import "WTCTableVirtualTableObject+WCTTableCoding.h"
#import "WTCTableVirtualTableObject.h"

@interface WTCTableTests : WTCBaseTestCase
@property(nonatomic, readonly) WCTDatabase *database;
@end

@implementation WTCTableTests

- (void)setUp
{
    [super setUp];
    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
    _database.tag = self.recommendedTag;
}

- (void)tearDown
{
    [_database close:^{
      XCTAssertTrue([_database removeFilesWithError:nil]);
    }];
    _database = nil;
    [super tearDown];
}

- (void)testCreateTable
{
    //Give
    NSString *tableName = WTCTableBaselineObject.Name;
    NSString *expected = [NSString stringWithFormat:@"CREATE TABLE %@(anInt32 INTEGER PRIMARY KEY ASC AUTOINCREMENT, anInt64 INTEGER, aString TEXT, aData BLOB, aDouble REAL)", tableName];
    //When
    XCTAssertTrue([self.database createTableAndIndexesOfName:WTCTableBaselineObject.Name withClass:WTCTableBaselineObject.class]);
    //Then
    WCTMaster *object = [self.database getObjectOfClass:WCTMaster.class fromTable:WCTMaster.TableName];
    XCTAssertNotNil(object);
    XCTAssertTrue([object.sql isEqualToString:expected]);
}

- (void)testCreateTableSkipColumns
{
    //Give
    NSString *tableName = WTCTableBaselineObject.Name;
    XCTAssertTrue([self.database createTableAndIndexesOfName:tableName withClass:WTCTableBaselineObject.class]);
    //Then
    XCTAssertTrue([self.database createTableAndIndexesOfName:tableName withClass:WTCTableSkipColumnObject.class]);
}

- (void)testCreateTableWithIndex
{
    //Give
    NSString *tableName = WTCTableIndexObject.Name;
    NSString *indexName = [tableName stringByAppendingString:@"_index"];
    NSString *expected = [NSString stringWithFormat:@"CREATE INDEX %@ ON %@(variable)", indexName, tableName];
    //When
    XCTAssertTrue([self.database createTableAndIndexesOfName:tableName withClass:WTCTableIndexObject.class]);
    //Then
    WCTMaster *object = [self.database getObjectOfClass:WCTMaster.class fromTable:WCTMaster.TableName where:WCTMaster.name == indexName];
    XCTAssertNotNil(object);
    XCTAssertTrue([object.sql isEqualToString:expected]);
}

- (void)testCreateTableWithConstraint
{
    //Give
    NSString *tableName = WTCTableConstraintObject.Name;
    NSString *expected = [NSString stringWithFormat:@"CREATE TABLE %@(variable1 INTEGER, variable2 INTEGER, CONSTRAINT WTCTableConstraintObjectConstraint UNIQUE(variable1, variable2))", tableName];
    //When
    XCTAssertTrue([self.database createTableAndIndexesOfName:WTCTableConstraintObject.Name withClass:WTCTableConstraintObject.class]);
    //Then
    WCTMaster *object = [self.database getObjectOfClass:WCTMaster.class fromTable:WCTMaster.TableName];
    XCTAssertNotNil(object);
    XCTAssertTrue([object.sql isEqualToString:expected]);
}

- (void)testCreateVirtualTable
{
    //Give
    NSString *tableName = WTCTableVirtualTableObject.Name;
    [self.database setTokenizer:WCTTokenizerNameWCDB];
    NSString *expected = [NSString stringWithFormat:@"CREATE VIRTUAL TABLE %@ USING fts3(tokenize=WCDB, variable1 INTEGER, variable2 INTEGER)", tableName];
    //When
    XCTAssertTrue([self.database createVirtualTableOfName:tableName withClass:WTCTableVirtualTableObject.class]);
    //Then
    WCTMaster *object = [self.database getObjectOfClass:WCTMaster.class fromTable:WCTMaster.TableName where:WCTMaster.name == tableName];
    XCTAssertNotNil(object);
    XCTAssertTrue([object.sql isEqualToString:expected]);
}

- (void)testCreateTableAutoFitORM
{
    //Give
    NSString *tableName = WTCTableAutoFitBaselineObject.Name;
    NSString *expected = [NSString stringWithFormat:@"CREATE TABLE %@(anInt32 INTEGER PRIMARY KEY ASC AUTOINCREMENT, anInt64 INTEGER, aString TEXT, aData BLOB, aDouble REAL, newColumn INTEGER)", tableName];
    XCTAssertTrue([self.database createTableAndIndexesOfName:tableName withClass:WTCTableAutoFitBaselineObject.class]);
    //Then
    XCTAssertTrue([self.database createTableAndIndexesOfName:tableName withClass:WTCTableAutoFitBaselineObject.class]);
    WCTMaster *object = [self.database getObjectOfClass:WCTMaster.class fromTable:WCTMaster.TableName where:WCTMaster.name == tableName];
    XCTAssertNotNil(object);
    XCTAssertTrue([object.sql isEqualToString:expected]);
}

- (void)testDropTable
{
    //Give
    NSString *tableName = WTCTableBaselineObject.Name;
    //When
    XCTAssertTrue([self.database createTableAndIndexesOfName:tableName withClass:WTCTableBaselineObject.class]);
    XCTAssertTrue([self.database dropTableOfName:tableName]);
    //Then
    WCTMaster *object = [self.database getObjectOfClass:WCTMaster.class fromTable:WCTMaster.TableName where:WCTMaster.name == tableName];
    XCTAssertNil(object);
}

- (void)testDropIndex
{
    //Give
    NSString *tableName = WTCTableIndexObject.Name;
    NSString *indexName = [tableName stringByAppendingString:@"_index"];
    //When
    XCTAssertTrue([self.database createTableAndIndexesOfName:tableName withClass:WTCTableIndexObject.class]);
    XCTAssertTrue([self.database dropIndexOfName:indexName]);

    //Then
    WCTMaster *object = [self.database getObjectOfClass:WCTMaster.class fromTable:WCTMaster.TableName where:WCTMaster.name == indexName];
    XCTAssertNil(object);
}

- (void)testManuallyCreateTable
{
    //Give
    NSString *tableName = WTCTableBaselineObject.Name;
    NSString *expected = [NSString stringWithFormat:@"CREATE TABLE %@(anInt32 INTEGER PRIMARY KEY ASC AUTOINCREMENT, anInt64 INTEGER, CONSTRAINT WTCTableBaselineObjectConstraint CHECK(anInt32 > 0))", tableName];

    WCDB::TableConstraint tableConstraint("WTCTableBaselineObjectConstraint");
    tableConstraint.withChecking(WTCTableBaselineObject.anInt32 > 0);

    WCDB::ColumnDef def1 = WTCTableBaselineObject.anInt32.getColumnDef();
    WCDB::Column column2 = WCDB::Column("anInt64");
    WCDB::ColumnDef def2 = WCDB::ColumnDef(column2).withType(WCDB::ColumnType::Integer64);
    std::list<WCDB::ColumnDef> defList = {def1, def2};
    //When
    XCTAssertTrue([self.database createTableOfName:tableName withColumnDefs:defList andConstraints:{tableConstraint}]);
    WCTMaster *object = [self.database getObjectOfClass:WCTMaster.class fromTable:WCTMaster.TableName where:WCTMaster.name == tableName];
    XCTAssertNotNil(object);
    NSLog(@"%@", object.sql);
    XCTAssertTrue([object.sql isEqualToString:expected]);
}

- (void)testManuallyAddColumn
{
    //Give
    NSString *tableName = WTCTableBaselineObject.Name;
    WCDB::Column column("newColumn");
    WCDB::ColumnDef def = WCDB::ColumnDef(column).withType(WCDB::ColumnType::Integer32);
    NSString *expected = [NSString stringWithFormat:@"CREATE TABLE %@(anInt32 INTEGER PRIMARY KEY ASC AUTOINCREMENT, anInt64 INTEGER, aString TEXT, aData BLOB, aDouble REAL, newColumn INTEGER)", tableName];
    //When
    XCTAssertTrue([self.database createTableAndIndexesOfName:tableName withClass:WTCTableBaselineObject.class]);
    XCTAssertTrue([self.database addColumn:def forTable:tableName]);
    //Then
    WCTMaster *object = [self.database getObjectOfClass:WCTMaster.class fromTable:WCTMaster.TableName where:WCTMaster.name == tableName];
    XCTAssertNotNil(object);
    XCTAssertTrue([object.sql isEqualToString:expected]);
}

- (void)testManuallyCreateIndex
{
    //Give
    NSString *tableName = WTCTableBaselineObject.Name;
    NSString *indexName = [tableName stringByAppendingString:@"_index"];
    WCDB::IndexedColumn index1 = WTCTableBaselineObject.aString.asIndex();
    WCDB::IndexedColumn index2 = WTCTableBaselineObject.aDouble.asIndex();
    std::list<WCDB::IndexedColumn> indexes = {index1, index2};
    NSString *expected = [NSString stringWithFormat:@"CREATE INDEX %@ ON %@(aString, aDouble)", indexName, tableName];
    //When
    XCTAssertTrue([self.database createTableAndIndexesOfName:tableName withClass:WTCTableBaselineObject.class]);
    XCTAssertTrue([self.database createIndexOfName:indexName withIndexedColumns:indexes forTable:tableName]);
    WCTMaster *object = [self.database getObjectOfClass:WCTMaster.class fromTable:WCTMaster.TableName where:WCTMaster.name == indexName];
    XCTAssertNotNil(object);
    XCTAssertTrue([object.sql isEqualToString:expected]);
}

- (void)testGetTable
{
    //Give
    NSString *tableName = WTCTableBaselineObject.Name;
    //When
    WCTTable *table = [self.database getTableOfName:tableName withClass:WTCTableBaselineObject.class];
    XCTAssertNil(table);
    XCTAssertTrue([self.database createTableAndIndexesOfName:tableName withClass:WTCTableBaselineObject.class]);
    //Then
    table = [self.database getTableOfName:tableName withClass:WTCTableBaselineObject.class];
    XCTAssertNotNil(table);
    XCTAssertEqual(table.tag, self.database.tag);
    XCTAssertTrue([table.path isEqualToString:self.database.path]);
}

@end
