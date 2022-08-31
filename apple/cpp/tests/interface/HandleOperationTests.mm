//
// Created by qiuwenchen on 2022/8/15.
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

@interface HandleOperationTests : CPPCRUDTestCase

@property (nonatomic, assign) std::shared_ptr<WCDB::Handle> handle;

@end

@implementation HandleOperationTests

- (void)setUp
{
    [super setUp];
    self.handle = std::make_shared<WCDB::Handle>(self.database->getHandle());
    [self insertPresetRows];
}

- (void)tearDown
{
    if (_handle->isPrepared()) {
        _handle->finalize();
    }
    _handle = nil;
    [super tearDown];
}

- (void)test_insert_row
{
    WCDB::OneRowValue row = [Random.shared testCaseValuesWithCount:1 startingFromIdentifier:[self nextIdentifier]][0];
    NSString* sql = [NSString stringWithFormat:@"INSERT INTO main.testTable(identifier, content) VALUES(%lld, '%s')", row[0].intValue(), row[1].textValue().data()];
    [self doTestSQLs:@[ sql ]
         inOperation:^BOOL {
             return self.handle->insertOneRow(row, self.columns, self.tableName.UTF8String);
         }];
    TestCaseAssertTrue([self allRowsCount] == 3);
}

- (void)test_insert_or_replace_row
{
    WCDB::OneRowValue row = [Random.shared testCaseValuesWithCount:1 startingFromIdentifier:[self nextIdentifier] - 1][0];
    NSString* sql = [NSString stringWithFormat:@"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(%lld, '%s')", row[0].intValue(), row[1].textValue().data()];
    [self doTestSQLs:@[ sql ]
         inOperation:^BOOL {
             return self.handle->insertOrReplaceOneRow(row, self.columns, self.tableName.UTF8String);
         }];
    TestCaseAssertTrue([self allRowsCount] == 2);
}

- (void)test_insert_multi_rows
{
    WCDB::MultiRowsValue rows = [Random.shared testCaseValuesWithCount:2 startingFromIdentifier:[self nextIdentifier]];
    self.expectMode = DatabaseTestCaseExpectSomeSQLs;
    [self doTestSQLs:@[ @"BEGIN IMMEDIATE",
                        @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)",
                        @"COMMIT" ]
         inOperation:^BOOL {
             return self.handle->insertMultiRows(rows, self.columns, self.tableName.UTF8String);
         }];
    TestCaseAssertTrue([self allRowsCount] == 4);
}

- (void)test_insert_or_replace_multi_rows
{
    WCDB::MultiRowsValue rows = [Random.shared testCaseValuesWithCount:2 startingFromIdentifier:[self nextIdentifier] - 2];
    self.expectMode = DatabaseTestCaseExpectSomeSQLs;
    [self doTestSQLs:@[ @"BEGIN IMMEDIATE",
                        @"INSERT OR REPLACE INTO main.testTable(identifier, content) VALUES(?1, ?2)",
                        @"COMMIT" ]
         inOperation:^BOOL {
             return self.handle->insertOrReplaceMultiRows(rows, self.columns, self.tableName.UTF8String);
         }];
    TestCaseAssertTrue([self allRowsCount] == 2);
}

- (void)test_update_value
{
    NSString* sql = [NSString stringWithFormat:@"UPDATE main.testTable SET content = '%s' WHERE identifier == %lld", self.row1[1].textValue().data(), self.row2[0].intValue()];
    [self doTestSQLs:@[ sql ]
         inOperation:^BOOL {
             return self.handle->updateValues(self.row1[1], WCDB::Column("content"), self.tableName.UTF8String, WCDB::Column("identifier") == self.row2[0]);
         }];
    TestCaseAssertTrue([self allRowsCount] == 2);
}

- (void)test_update_values
{
    NSString* sql = [NSString stringWithFormat:@"UPDATE main.testTable SET identifier = %lld, content = '%s' WHERE identifier == %lld", self.row1[0].intValue(), self.row1[1].textValue().data(), self.row1[0].intValue()];
    [self doTestSQLs:@[ sql ]
         inOperation:^BOOL {
             return self.handle->updateValues(self.row1, self.columns, self.tableName.UTF8String, WCDB::Column("identifier") == self.row1[0]);
         }];
    TestCaseAssertTrue([self allRowsCount] == 2);
}

- (void)test_delete
{
    [self doTestSQLs:@[ @"DELETE FROM main.testTable WHERE identifier == 1" ]
         inOperation:^BOOL {
             return self.handle->deleteValues(self.tableName.UTF8String, WCDB::Column("identifier") == 1);
         }];
    TestCaseAssertTrue([self allRowsCount] == 1);
}

- (void)test_select_value
{
    [self doTestValue:self.row1[0]
               andSQL:@"SELECT identifier FROM main.testTable"
          bySelecting:^WCDB::OptionalValue {
              return self.handle->selectValue(WCDB::Column("identifier"), self.tableName.UTF8String);
          }];
}

- (void)test_select_value_where
{
    [self doTestValue:self.row1[0]
               andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1"
          bySelecting:^WCDB::OptionalValue {
              return self.handle->selectValue(WCDB::Column("identifier"), self.tableName.UTF8String, WCDB::Column("identifier") == 1);
          }];
}

- (void)test_select_one_column
{
    [self doTestColumn:{ self.row1[0], self.row2[0] }
                andSQL:@"SELECT identifier FROM main.testTable"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.handle->selectOneColumn(WCDB::Column("identifier"), self.tableName.UTF8String);
           }];
}

- (void)test_select_one_column_where
{
    [self doTestColumn:self.row1[0]
                andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.handle->selectOneColumn(WCDB::Column("identifier"), self.tableName.UTF8String, WCDB::Column("identifier") == 1);
           }];
}

- (void)test_select_one_column_where_order
{
    [self doTestColumn:self.row1[0]
                andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1 ORDER BY identifier"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.handle->selectOneColumn(WCDB::Column("identifier"), self.tableName.UTF8String, WCDB::Column("identifier") == 1, WCDB::Column("identifier"));
           }];
}

- (void)test_select_one_column_where_order_limit
{
    [self doTestColumn:self.row1[0]
                andSQL:@"SELECT identifier FROM main.testTable WHERE identifier == 1 ORDER BY identifier ASC LIMIT 1"
           bySelecting:^WCDB::OptionalOneColumn {
               return self.handle->selectOneColumn(WCDB::Column("identifier"), self.tableName.UTF8String, WCDB::Column("identifier") == 1, WCDB::Column("identifier").asAscendingOrder(), 1);
           }];
}

- (void)test_select_one_row
{
    [self doTestRow:self.row1
             andSQL:@"SELECT identifier, content FROM main.testTable"
        bySelecting:^WCDB::OptionalOneColumn {
            return self.handle->selectOneRow(self.columns, self.tableName.UTF8String);
        }];
}

- (void)test_select_one_row_where
{
    [self doTestRow:self.row1
             andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier == 1"
        bySelecting:^WCDB::OptionalOneColumn {
            return self.handle->selectOneRow(self.columns, self.tableName.UTF8String, WCDB::Column("identifier") == 1);
        }];
}

- (void)test_select_all_row
{
    [self doTestRows:self.rows
              andSQL:@"SELECT identifier, content FROM main.testTable"
         bySelecting:^WCDB::OptionalMultiRows {
             return self.handle->selectAllRow(self.columns, self.tableName.UTF8String);
         }];
}

- (void)test_select_all_row_where
{
    [self doTestRows:{ self.row1 }
              andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier == 1"
         bySelecting:^WCDB::OptionalMultiRows {
             return self.handle->selectAllRow(self.columns, self.tableName.UTF8String, WCDB::Column("identifier") == 1);
         }];
}

- (void)test_select_all_row_where_order
{
    [self doTestRows:{ self.row1 }
              andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier == 1 ORDER BY identifier"
         bySelecting:^WCDB::OptionalMultiRows {
             return self.handle->selectAllRow(self.columns, self.tableName.UTF8String, WCDB::Column("identifier") == 1, WCDB::Column("identifier"));
         }];
}

- (void)test_select_all_row_where_order_limit
{
    [self doTestRows:{ self.row1 }
              andSQL:@"SELECT identifier, content FROM main.testTable WHERE identifier == 1 ORDER BY identifier ASC LIMIT 1"
         bySelecting:^WCDB::OptionalMultiRows {
             return self.handle->selectAllRow(self.columns, self.tableName.UTF8String, WCDB::Column("identifier") == 1, WCDB::Column("identifier").asAscendingOrder(), 1);
         }];
}

@end
