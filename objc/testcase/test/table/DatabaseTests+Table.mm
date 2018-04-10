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

@interface DatabaseTests_Table : TestCase

@end

@implementation DatabaseTests_Table {
    WCTDatabase *_database;
    NSString *_tableName;
    Class _cls;
}

- (void)setUp
{
    [super setUp];

    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];

    _tableName = self.class.className;

    _cls = TestCaseObject.class;
}

- (void)tearDown
{
    XCTAssertTrue([_database dropTable:_tableName]);

    [_database close:^{
      XCTAssertTrue([_database removeFiles]);
    }];

    _database = nil;

    [super tearDown];
}

- (void)test_create_table_and_indexes
{
    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:TestCaseIndexedObject.class]);

    {
        WCTValue *value = [_database getValueOnResult:WCTMaster.sql fromTable:WCTMaster.TableName where:WCTMaster.name == _tableName];
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE %@(variable1 INTEGER PRIMARY KEY ASC AUTOINCREMENT, variable2 TEXT, variable3 REAL)", _tableName];
        XCTAssertTrue([value.stringValue isEqualToString:expectedSQL]);
    }

    {
        WCTValue *value = [_database getValueOnResult:WCTMaster.sql fromTable:WCTMaster.TableName where:WCTMaster.name == [_tableName stringByAppendingString:@"_index"]];
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE INDEX %@_index ON %@(variable2)", _tableName, _tableName];
        XCTAssertTrue([value.stringValue isEqualToString:expectedSQL]);
    }
}

- (void)test_get_table
{
    WCTTable<TestCaseObject *> *table = [_database getTable:_tableName withClass:_cls];
    XCTAssertTrue([table.tableName isEqualToString:_tableName]);
    XCTAssertEqual(table.cls, _cls);
}

- (void)test_table_exists
{
    WCTError *error;
    XCTAssertFalse([_database isTableExists:_tableName withError:&error]);
    XCTAssertNil(error);
    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:_cls]);
    XCTAssertTrue([_database isTableExists:_tableName]);
}

- (void)test_drop_table
{
    WCTError *error;

    XCTAssertFalse([_database isTableExists:_tableName withError:&error]);
    XCTAssertNil(error);

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:_cls]);
    XCTAssertTrue([_database isTableExists:_tableName]);

    XCTAssertTrue([_database dropTable:_tableName]);

    XCTAssertFalse([_database isTableExists:_tableName withError:&error]);
    XCTAssertNil(error);
}

- (BOOL)isIndexExists:(NSString *)indexName
{
    WCTValue *rowValue = [_database getValueFromStatement:WCDB::StatementSelect().select(WCTMaster.AllResults.count()).from(WCTMaster.TableName.UTF8String).where(WCTMaster.name == indexName)];
    XCTAssertNotNil(rowValue);
    return rowValue.integer32Value > 0;
}

- (void)test_drop_index
{
    NSString *indexName = [_tableName stringByAppendingString:@"_index"];
    XCTAssertFalse([self isIndexExists:indexName]);
    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:TestCaseIndexedObject.class]);
    XCTAssertTrue([self isIndexExists:indexName]);
    XCTAssertTrue([_database dropIndex:indexName]);
    XCTAssertFalse([self isIndexExists:indexName]);
}

- (void)test_create_virtual_table
{
    [_database setTokenizer:WCTTokenizerNameWCDB];
    XCTAssertTrue([_database createVirtualTable:_tableName withClass:TestCaseVirtualObject.class]);

    WCTValue *value = [_database getValueOnResult:WCTMaster.sql fromTable:WCTMaster.TableName where:WCTMaster.name == _tableName];
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE VIRTUAL TABLE %@ USING fts3(tokenize=WCDB, variable1 INTEGER PRIMARY KEY ASC AUTOINCREMENT, variable2 TEXT)", _tableName];
    XCTAssertTrue([value.stringValue isEqualToString:expectedSQL]);
}

@end
