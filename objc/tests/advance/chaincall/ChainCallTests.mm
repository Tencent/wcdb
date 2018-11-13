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

@interface ChainCallTests : CRUDTestCase

@end

@implementation ChainCallTests

#pragma mark - Delete
- (void)test_database_delete
{
    WCTDelete* delete_ = [[self.database prepareDelete] fromTable:self.tableName];
    SQLAssertEqual(delete_.statement, @"DELETE FROM main.testTable");
    XCTAssertEqual(delete_.handle.database.tag, self.database.tag);
}

- (void)test_table_delete
{
    WCTDelete* delete_ = [self.table prepareDelete];
    SQLAssertEqual(delete_.statement, @"DELETE FROM main.testTable");
    XCTAssertEqual(delete_.handle.database.tag, self.database.tag);
}

- (void)test_handle_delete
{
    WCTDelete* delete_ = [[[self.database getHandle] prepareDelete] fromTable:self.tableName];
    SQLAssertEqual(delete_.statement, @"DELETE FROM main.testTable");
    XCTAssertEqual(delete_.handle.database.tag, self.database.tag);
}

#pragma mark - Insert
- (void)test_database_insert
{
    WCTInsert* insert = [[[self.database prepareInsert] onProperties:TestCaseObject.allProperties] intoTable:self.tableName];
    SQLAssertEqual(insert.statement, @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)");
    XCTAssertEqual(insert.handle.database.tag, self.database.tag);
}

- (void)test_table_insert
{
    WCTInsert* insert = [[self.table prepareInsert] onProperties:TestCaseObject.allProperties];
    SQLAssertEqual(insert.statement, @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)");
    XCTAssertEqual(insert.handle.database.tag, self.database.tag);
}

- (void)test_handle_insert
{
    WCTInsert* insert = [[[[self.database getHandle] prepareInsert] onProperties:TestCaseObject.allProperties] intoTable:self.tableName];
    SQLAssertEqual(insert.statement, @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)");
    XCTAssertEqual(insert.handle.database.tag, self.database.tag);
}

#pragma mark - Update
- (void)test_database_update
{
    WCTUpdate* update = [[[self.database prepareUpdate] table:self.tableName] set:TestCaseObject.content];
    SQLAssertEqual(update.statement, @"UPDATE main.testTable SET content = ?1");
    XCTAssertEqual(update.handle.database.tag, self.database.tag);
}

- (void)test_table_update
{
    WCTUpdate* update = [[self.table prepareUpdate] set:TestCaseObject.content];
    SQLAssertEqual(update.statement, @"UPDATE main.testTable SET content = ?1");
    XCTAssertEqual(update.handle.database.tag, self.database.tag);
}

- (void)test_handle_update
{
    WCTUpdate* update = [[[[self.database getHandle] prepareUpdate] table:self.tableName] set:TestCaseObject.content];
    SQLAssertEqual(update.statement, @"UPDATE main.testTable SET content = ?1");
    XCTAssertEqual(update.handle.database.tag, self.database.tag);
}

#pragma mark - Select
- (void)test_database_select
{
    WCTSelect* select = [[[self.database prepareSelect] onResultColumns:TestCaseObject.allProperties] fromTable:self.tableName];
    SQLAssertEqual(select.statement, @"SELECT identifier, content FROM main.testTable");
    XCTAssertEqual(select.handle.database.tag, self.database.tag);
}

- (void)test_table_select
{
    WCTSelect* select = [[self.table prepareSelect] onResultColumns:TestCaseObject.allProperties];
    SQLAssertEqual(select.statement, @"SELECT identifier, content FROM main.testTable");
    XCTAssertEqual(select.handle.database.tag, self.database.tag);
}

- (void)test_handle_select
{
    WCTSelect* select = [[[[self.database getHandle] prepareSelect] onResultColumns:TestCaseObject.allProperties] fromTable:self.tableName];
    SQLAssertEqual(select.statement, @"SELECT identifier, content FROM main.testTable");
    XCTAssertEqual(select.handle.database.tag, self.database.tag);
}

#pragma mark - Row Select
- (void)test_database_row_select
{
    WCTRowSelect* select = [[[self.database prepareRowSelect] onResultColumns:TestCaseObject.allProperties] fromTable:self.tableName];
    SQLAssertEqual(select.statement, @"SELECT identifier, content FROM main.testTable");
    XCTAssertEqual(select.handle.database.tag, self.database.tag);
}

- (void)test_table_row_select
{
    WCTRowSelect* select = [[self.table prepareRowSelect] onResultColumns:TestCaseObject.allProperties];
    SQLAssertEqual(select.statement, @"SELECT identifier, content FROM main.testTable");
    XCTAssertEqual(select.handle.database.tag, self.database.tag);
}

- (void)test_handle_row_select
{
    WCTRowSelect* select = [[[[self.database getHandle] prepareRowSelect] onResultColumns:TestCaseObject.allProperties] fromTable:self.tableName];
    SQLAssertEqual(select.statement, @"SELECT identifier, content FROM main.testTable");
    XCTAssertEqual(select.handle.database.tag, self.database.tag);
}

#pragma mark - Multi Select
- (void)test_database_multi_select
{
    NSString* tableName2 = @"testTable2";
    WCTResultColumns resultColumns = {
        TestCaseObject.content.redirect(TestCaseObject.content.table(self.tableName)),
        TestCaseObject.content.redirect(TestCaseObject.content.table(tableName2)),
    };
    WCTMultiSelect* select = [[[[self.database prepareMultiSelect] onResultColumns:resultColumns] fromTables:@[ self.tableName, tableName2 ]] where:TestCaseObject.identifier.table(self.tableName) == TestCaseObject.identifier.table(tableName2)];
    SQLAssertEqual(select.statement, @"SELECT main.testTable.content, main.testTable2.content FROM main.testTable, main.testTable2 WHERE main.testTable.identifier == main.testTable2.identifier");
    XCTAssertEqual(select.handle.database.tag, self.database.tag);
}

- (void)test_handle_multi_select
{
    NSString* tableName2 = @"testTable2";
    WCTResultColumns resultColumns = {
        TestCaseObject.content.redirect(TestCaseObject.content.table(self.tableName)),
        TestCaseObject.content.redirect(TestCaseObject.content.table(tableName2)),
    };
    WCTMultiSelect* select = [[[[[self.database getHandle] prepareMultiSelect] onResultColumns:resultColumns] fromTables:@[ self.tableName, tableName2 ]] where:TestCaseObject.identifier.table(self.tableName) == TestCaseObject.identifier.table(tableName2)];
    SQLAssertEqual(select.statement, @"SELECT main.testTable.content, main.testTable2.content FROM main.testTable, main.testTable2 WHERE main.testTable.identifier == main.testTable2.identifier");
    XCTAssertEqual(select.handle.database.tag, self.database.tag);
}

@end
