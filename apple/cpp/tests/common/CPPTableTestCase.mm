//
// Created by 陈秋文 on 2022/8/13.
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

#import "CPPTableTestCase.h"
#import "TestCaseAssertion.h"
#import <Foundation/Foundation.h>

@implementation CPPTableTestCase

- (NSString*)tableName
{
    @synchronized(self) {
        if (_tableName == nil) {
            _tableName = @"testTable";
        }
        return _tableName;
    }
}

- (BOOL)createTable
{
    WCDB::StatementCreateTable createTable = WCDB::StatementCreateTable().createTable(self.tableName.UTF8String);
    createTable.define(WCDB::ColumnDef("identifier", WCDB::ColumnType::Integer).constraint(WCDB::ColumnConstraint().primaryKey().autoIncrement()));
    createTable.define(WCDB::ColumnDef("content", WCDB::ColumnType::Text));
    return self.database->execute(createTable);
}

- (BOOL)dropTable
{
    return self.database->dropTable(self.tableName.UTF8String);
}

- (WCDB::Columns)columns
{
    return {
        WCDB::Column("identifier"),
        WCDB::Column("content"),
    };
}
- (WCDB::ResultColumns)resultColumns
{
    return self.columns;
}

- (void)doTestRow:(const WCDB::OneRowValue&)row
           andSQL:(NSString*)sql
      bySelecting:(WCDB::OneRowValue (^)())block
{
    TestCaseAssertTrue(row.size() > 0);
    TestCaseAssertTrue(sql != nil);
    [self doTestRows:{ row }
             andSQLs:@[ sql ]
         bySelecting:^WCDB::MultiRowsValue {
             WCDB::OneRowValue result = block();
             return { result };
         }];
}

- (void)doTestColumn:(const WCDB::OneColumnValue&)column
              andSQL:(NSString*)sql
         bySelecting:(WCDB::OneColumnValue (^)())block
{
    TestCaseAssertTrue(sql != nil);
    TestCaseAssertTrue(column.size() > 0);
    WCDB::MultiRowsValue rows;
    for (WCDB::Value value : column) {
        rows.push_back({ value });
    }
    [self doTestRows:rows
             andSQLs:@[ sql ]
         bySelecting:^WCDB::MultiRowsValue {
             WCDB::OneColumnValue result = block();
             WCDB::MultiRowsValue resultRows;
             for (WCDB::Value value : result) {
                 resultRows.push_back({ value });
             }
             return resultRows;
         }];
}

- (void)doTestValue:(WCDB::Value)value
             andSQL:(NSString*)sql
        bySelecting:(WCDB::Value (^)())block
{
    TestCaseAssertTrue(sql != nil);
    [self doTestRows:{ { value } }
    andSQLs:@[ sql ]
    bySelecting:^WCDB::MultiRowsValue {
        WCDB::Value result = block();
        return { { result } };
    }];
}

- (void)doTestRows:(const WCDB::MultiRowsValue&)rows
            andSQL:(NSString*)sql
       bySelecting:(WCDB::MultiRowsValue (^)())block
{
    TestCaseAssertTrue(rows.size() > 0);
    TestCaseAssertTrue(sql != nil);
    [self doTestRows:rows andSQLs:@[ sql ] bySelecting:block];
}

- (void)doTestRows:(const WCDB::MultiRowsValue&)expectedRows
           andSQLs:(NSArray<NSString*>*)expectedSQLs
       bySelecting:(WCDB::MultiRowsValue (^)())block
{
    __block WCDB::MultiRowsValue selected;
    [self doTestSQLs:expectedSQLs
         inOperation:^BOOL {
             selected = block();
             return selected.size() > 0;
         }];
    TestCaseAssertEqual(selected.size(), expectedRows.size());
    for (int i = 0; i < selected.size(); i++) {
        const WCDB::OneRowValue& selectedRow = selected[i];
        const WCDB::OneRowValue& expectedRow = expectedRows[i];
        TestCaseAssertEqual(selectedRow.size(), expectedRow.size());
        for (int j = 0; j < selectedRow.size(); j++) {
            TestCaseAssertEqual(selectedRow[j], expectedRow[j]);
        }
    }
}

- (WCDB::MultiRowsValue)getAllvalues
{
    return self.database->getAllRowsFromStatement(WCDB::StatementSelect().select(self.resultColumns).from(self.tableName.UTF8String));
}

@end
