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

@interface HandleTests_Table : TestCase

@end

@implementation HandleTests_Table {
    WCTHandle *_handle;
    NSString *_tableName;
    Class _cls;
}

- (void)setUp
{
    [super setUp];

    WCTDatabase *database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
    _handle = [database getHandle];

    XCTAssertNotNil(_handle);

    _tableName = self.className;

    _cls = TestCaseObject.class;
}

- (void)tearDown
{
    [_handle finalizeHandle];
    _handle = nil;

    WCTDatabase *database = [_handle getDatabase];
    [database close:^{
      XCTAssertTrue([database removeFiles]);
    }];

    [super tearDown];
}

- (void)test_create_table_and_indexes
{
    XCTAssertTrue([_handle createTableAndIndexes:_tableName withClass:TestCaseIndexedObject.class]);

    {
        WCTValue *value = [_handle getValueOnResult:WCTMaster.sql fromTable:WCTMaster.tableName where:WCTMaster.name == _tableName];
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE %@(variable1 INTEGER PRIMARY KEY ASC AUTOINCREMENT, variable2 TEXT, variable3 REAL)", _tableName];
        XCTAssertTrue([value.stringValue isEqualToString:expectedSQL]);
    }

    {
        NSString *indexName = [_tableName stringByAppendingString:@"_index"];
        WCTValue *value = [_handle getValueOnResult:WCTMaster.sql fromTable:WCTMaster.tableName where:WCTMaster.name == indexName];
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE INDEX %@_index ON %@(variable2)", _tableName, _tableName];
        XCTAssertTrue([value.stringValue isEqualToString:expectedSQL]);
    }
}

- (void)test_get_table
{
    WCTTable<TestCaseObject *> *table = [_handle getTable:_tableName withClass:_cls];
    XCTAssertTrue([table.tableName isEqualToString:_tableName]);
    XCTAssertEqual(table.cls, _cls);
}

- (void)test_table_exists
{
    WCTError *error;
    XCTAssertFalse([_handle isTableExists:_tableName withError:&error]);
    XCTAssertNil(error);
    XCTAssertTrue([_handle createTableAndIndexes:_tableName withClass:_cls]);
    XCTAssertTrue([_handle isTableExists:_tableName]);
}

- (void)test_drop_table
{
    WCTError *error;

    XCTAssertFalse([_handle isTableExists:_tableName withError:&error]);
    XCTAssertNil(error);

    XCTAssertTrue([_handle createTableAndIndexes:_tableName withClass:_cls]);
    XCTAssertTrue([_handle isTableExists:_tableName]);

    XCTAssertTrue([_handle dropTable:_tableName]);

    XCTAssertFalse([_handle isTableExists:_tableName withError:&error]);
    XCTAssertNil(error);
}

- (BOOL)isIndexExists:(NSString *)indexName
{
    WCTValue *rowValue = [_handle getValueFromStatement:WCDB::StatementSelect().select(WCTMaster.AllResults.count()).from(WCTMaster.tableName.UTF8String).where(WCTMaster.name == indexName)];
    XCTAssertNotNil(rowValue);
    return rowValue.boolValue;
}

- (void)test_drop_index
{
    NSString *indexName = [_tableName stringByAppendingString:@"_index"];
    XCTAssertFalse([self isIndexExists:indexName]);
    XCTAssertTrue([_handle createTableAndIndexes:_tableName withClass:TestCaseIndexedObject.class]);
    XCTAssertTrue([self isIndexExists:indexName]);
    XCTAssertTrue([_handle dropIndex:indexName]);
    XCTAssertFalse([self isIndexExists:indexName]);
}

- (void)test_create_virtual_table
{
    [[_handle getDatabase] setTokenizer:WCTTokenizer.name];
    XCTAssertTrue([_handle createVirtualTable:_tableName withClass:TestCaseVirtualObject.class]);

    WCTValue *value = [_handle getValueOnResult:WCTMaster.sql fromTable:WCTMaster.tableName where:WCTMaster.name == _tableName];
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE VIRTUAL TABLE %@ USING fts3(tokenize=WCDB, variable1 INTEGER PRIMARY KEY ASC AUTOINCREMENT, variable2 TEXT)", _tableName];
    XCTAssertTrue([value.stringValue isEqualToString:expectedSQL]);
}

@end
