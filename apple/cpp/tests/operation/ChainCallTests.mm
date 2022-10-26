//
// Created by qiuwenchen on 2022/9/8.
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

@interface ChainCallTests : CPPCRUDTestCase
@property (nonatomic, strong) NSString* tableName2;
@end

@implementation ChainCallTests

- (void)setUp
{
    [super setUp];
    self.tableName2 = @"testTable2";
}

#pragma mark - Delete
- (void)test_database_delete
{
    WCDB::Delete delete_ = self.database->prepareDelete().fromTable(self.tableName.UTF8String);
    TestCaseAssertSQLEqual(delete_.getStatement(), @"DELETE FROM main.testTable");
}

- (void)test_table_delete
{
    WCDB::Delete delete_ = self.table.prepareDelete();
    TestCaseAssertSQLEqual(delete_.getStatement(), @"DELETE FROM main.testTable");
}

- (void)test_handle_delete
{
    WCDB::Delete delete_ = self.database->getHandle().prepareDelete().fromTable(self.tableName.UTF8String);
    TestCaseAssertSQLEqual(delete_.getStatement(), @"DELETE FROM main.testTable");
}

#pragma mark - Insert
- (void)test_database_insert
{
    WCDB::Insert<CPPTestCaseObject> insert = self.database->prepareInsert<CPPTestCaseObject>().intoTable(self.tableName.UTF8String).onFields(CPPTestCaseObject::allFields());
    TestCaseAssertSQLEqual(insert.getStatement(), @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)");
}

- (void)test_table_insert
{
    WCDB::Insert<CPPTestCaseObject> insert = self.table.prepareInsert().onFields(CPPTestCaseObject::allFields());
    TestCaseAssertSQLEqual(insert.getStatement(), @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)");
}

- (void)test_handle_insert
{
    WCDB::Insert<CPPTestCaseObject> insert = self.database->getHandle().prepareInsert<CPPTestCaseObject>().intoTable(self.tableName.UTF8String).onFields(CPPTestCaseObject::allFields());
    TestCaseAssertSQLEqual(insert.getStatement(), @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)");
}

#pragma mark - Update
- (void)test_database_update
{
    WCDB::Update<CPPTestCaseObject> update = self.database->prepareUpdate<CPPTestCaseObject>().table(self.tableName.UTF8String).set(WCDB_FIELD(CPPTestCaseObject::content));
    TestCaseAssertSQLEqual(update.getStatement(), @"UPDATE main.testTable SET content = ?1");
}

- (void)test_table_update
{
    WCDB::Update<CPPTestCaseObject> update = self.table.prepareUpdate().set(WCDB_FIELD(CPPTestCaseObject::content));
    TestCaseAssertSQLEqual(update.getStatement(), @"UPDATE main.testTable SET content = ?1");
}

- (void)test_handle_update
{
    WCDB::Update<CPPTestCaseObject> update = self.database->getHandle().prepareUpdate<CPPTestCaseObject>().table(self.tableName.UTF8String).set(WCDB_FIELD(CPPTestCaseObject::content));
    TestCaseAssertSQLEqual(update.getStatement(), @"UPDATE main.testTable SET content = ?1");
}

#pragma mark - Select
- (void)test_database_select
{
    WCDB::Select<CPPTestCaseObject> select = self.database->prepareSelect<CPPTestCaseObject>().fromTable(self.tableName.UTF8String).onResultFields(CPPTestCaseObject::allFields()).where(WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
    TestCaseAssertSQLEqual(select.getStatement(), @"SELECT identifier, content FROM main.testTable WHERE identifier == 1");
}

- (void)test_table_select
{
    WCDB::Select<CPPTestCaseObject> select = self.table.prepareSelect().onResultFields(CPPTestCaseObject::allFields()).where(WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
    TestCaseAssertSQLEqual(select.getStatement(), @"SELECT identifier, content FROM main.testTable WHERE identifier == 1");
}

- (void)test_handle_select
{
    WCDB::Select<CPPTestCaseObject> select = self.database->getHandle().prepareSelect<CPPTestCaseObject>().fromTable(self.tableName.UTF8String).onResultFields(CPPTestCaseObject::allFields()).where(WCDB_FIELD(CPPTestCaseObject::identifier) == 1);
    TestCaseAssertSQLEqual(select.getStatement(), @"SELECT identifier, content FROM main.testTable WHERE identifier == 1");
}

#pragma mark - MultiSelect
- (void)test_database_multiselect
{
    WCDB::ResultFields resultColumns
    = CPPTestCaseObject::allFields()
      .redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
          return field.table(self.tableName.UTF8String);
      })
      .addingNewResultColumns(CPPTestCaseObject::allFields().redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
          return field.table(self.tableName2.UTF8String);
      }));
    WCDB::MultiSelect select = self.database->prepareMultiSelect().onResultFields(resultColumns).fromTables({ self.tableName.UTF8String, self.tableName2.UTF8String }).where(WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName.UTF8String) == WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName2.UTF8String));
    TestCaseAssertSQLEqual(select.getStatement(), @"SELECT main.testTable.identifier, main.testTable.content, main.testTable2.identifier, main.testTable2.content FROM main.testTable, main.testTable2 WHERE main.testTable.identifier == main.testTable2.identifier");
}

- (void)test_handle_multiselect
{
    WCDB::ResultFields resultColumns
    = CPPTestCaseObject::allFields()
      .redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
          return field.table(self.tableName.UTF8String);
      })
      .addingNewResultColumns(CPPTestCaseObject::allFields().redirect([self](const WCDB::Field& field) -> WCDB::ResultColumn {
          return field.table(self.tableName2.UTF8String);
      }));
    WCDB::MultiSelect select = self.database->getHandle().prepareMultiSelect().onResultFields(resultColumns).fromTables({ self.tableName.UTF8String, self.tableName2.UTF8String }).where(WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName.UTF8String) == WCDB_FIELD(CPPTestCaseObject::identifier).table(self.tableName2.UTF8String));
    TestCaseAssertSQLEqual(select.getStatement(), @"SELECT main.testTable.identifier, main.testTable.content, main.testTable2.identifier, main.testTable2.content FROM main.testTable, main.testTable2 WHERE main.testTable.identifier == main.testTable2.identifier");
}

@end
