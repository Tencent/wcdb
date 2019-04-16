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

#import <TestCase/TestCase.h>

@interface ChainCallTests : CRUDTestCase

@end

@implementation ChainCallTests

- (void)setUp
{
    [super setUp];
    [self insertPresetObjects];
}

#pragma mark - Delete
- (void)test_database_delete
{
    WCTDelete* delete_ = [[self.database prepareDelete] fromTable:self.tableName];
    TestCaseAssertSQLEqual(delete_.statement, @"DELETE FROM main.testTable");
    TestCaseAssertEqual(delete_.handle.database.tag, self.database.tag);
}

- (void)test_table_delete
{
    WCTDelete* delete_ = [self.table prepareDelete];
    TestCaseAssertSQLEqual(delete_.statement, @"DELETE FROM main.testTable");
    TestCaseAssertEqual(delete_.handle.database.tag, self.database.tag);
}

- (void)test_handle_delete
{
    WCTDelete* delete_ = [[[self.database getHandle] prepareDelete] fromTable:self.tableName];
    TestCaseAssertSQLEqual(delete_.statement, @"DELETE FROM main.testTable");
    TestCaseAssertEqual(delete_.handle.database.tag, self.database.tag);
}

#pragma mark - Insert
- (void)test_database_insert
{
    WCTInsert* insert = [[[self.database prepareInsert] onProperties:TestCaseObject.allProperties] intoTable:self.tableName];
    TestCaseAssertSQLEqual(insert.statement, @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)");
    TestCaseAssertEqual(insert.handle.database.tag, self.database.tag);
}

- (void)test_table_insert
{
    WCTInsert* insert = [[self.table prepareInsert] onProperties:TestCaseObject.allProperties];
    TestCaseAssertSQLEqual(insert.statement, @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)");
    TestCaseAssertEqual(insert.handle.database.tag, self.database.tag);
}

- (void)test_handle_insert
{
    WCTInsert* insert = [[[[self.database getHandle] prepareInsert] onProperties:TestCaseObject.allProperties] intoTable:self.tableName];
    TestCaseAssertSQLEqual(insert.statement, @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)");
    TestCaseAssertEqual(insert.handle.database.tag, self.database.tag);
}

#pragma mark - Update
- (void)test_database_update
{
    WCTUpdate* update = [[[self.database prepareUpdate] table:self.tableName] set:TestCaseObject.content];
    TestCaseAssertSQLEqual(update.statement, @"UPDATE main.testTable SET content = ?1");
    TestCaseAssertEqual(update.handle.database.tag, self.database.tag);
}

- (void)test_table_update
{
    WCTUpdate* update = [[self.table prepareUpdate] set:TestCaseObject.content];
    TestCaseAssertSQLEqual(update.statement, @"UPDATE main.testTable SET content = ?1");
    TestCaseAssertEqual(update.handle.database.tag, self.database.tag);
}

- (void)test_handle_update
{
    WCTUpdate* update = [[[[self.database getHandle] prepareUpdate] table:self.tableName] set:TestCaseObject.content];
    TestCaseAssertSQLEqual(update.statement, @"UPDATE main.testTable SET content = ?1");
    TestCaseAssertEqual(update.handle.database.tag, self.database.tag);
}

#pragma mark - Select
- (void)test_database_select
{
    WCTSelect* select = [[[self.database prepareSelect] onResultColumns:TestCaseObject.allProperties] fromTable:self.tableName];
    TestCaseAssertSQLEqual(select.statement, @"SELECT identifier, content FROM main.testTable");
    TestCaseAssertEqual(select.handle.database.tag, self.database.tag);
}

- (void)test_table_select
{
    WCTSelect* select = [[self.table prepareSelect] onResultColumns:TestCaseObject.allProperties];
    TestCaseAssertSQLEqual(select.statement, @"SELECT identifier, content FROM main.testTable");
    TestCaseAssertEqual(select.handle.database.tag, self.database.tag);
}

- (void)test_handle_select
{
    WCTSelect* select = [[[[self.database getHandle] prepareSelect] onResultColumns:TestCaseObject.allProperties] fromTable:self.tableName];
    TestCaseAssertSQLEqual(select.statement, @"SELECT identifier, content FROM main.testTable");
    TestCaseAssertEqual(select.handle.database.tag, self.database.tag);
}

- (void)test_check_next_object_failed_using_done
{
    WCTSelect* select = [[[[self.database prepareSelect] onResultColumns:TestCaseObject.allProperties] fromTable:self.tableName] limit:1];
    TestCaseAssertTrue([select.handle validate]);

    [WCTDatabase simulateIOError:WCTSimulateWriteIOError | WCTSimulateReadIOError];
    // nextObject is nil due to IOError, so done is false.
    TestCaseAssertTrue([select nextObject] == nil);
    TestCaseAssertFalse([select.handle done]);

    [WCTDatabase simulateIOError:WCTSimulateNoneIOError];
    // nextObject is not nil, so done is false.
    TestCaseAssertTrue([select nextObject] != nil);
    TestCaseAssertFalse([select.handle done]);

    // nextObject is nil since it' ended, so done is true.
    TestCaseAssertTrue([select nextObject] == nil);
    TestCaseAssertTrue([select.handle done]);
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
    TestCaseAssertSQLEqual(select.statement, @"SELECT main.testTable.content, main.testTable2.content FROM main.testTable, main.testTable2 WHERE main.testTable.identifier == main.testTable2.identifier");
    TestCaseAssertEqual(select.handle.database.tag, self.database.tag);
}

- (void)test_handle_multi_select
{
    NSString* tableName2 = @"testTable2";
    WCTResultColumns resultColumns = {
        TestCaseObject.content.redirect(TestCaseObject.content.table(self.tableName)),
        TestCaseObject.content.redirect(TestCaseObject.content.table(tableName2)),
    };
    WCTMultiSelect* select = [[[[[self.database getHandle] prepareMultiSelect] onResultColumns:resultColumns] fromTables:@[ self.tableName, tableName2 ]] where:TestCaseObject.identifier.table(self.tableName) == TestCaseObject.identifier.table(tableName2)];
    TestCaseAssertSQLEqual(select.statement, @"SELECT main.testTable.content, main.testTable2.content FROM main.testTable, main.testTable2 WHERE main.testTable.identifier == main.testTable2.identifier");
    TestCaseAssertEqual(select.handle.database.tag, self.database.tag);
}

@end
