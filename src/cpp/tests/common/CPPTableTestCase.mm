//
// Created by qiuwenchen on 2022/8/13.
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
#import "CPPORMTestUtil.h"
#import "TestCaseAssertion.h"
#import <Foundation/Foundation.h>

@interface CPPTableTestCase () {
    std::shared_ptr<WCDB::Table<CPPTestCaseObject>> m_table;
}
@end

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

#pragma mark - Object
- (WCDB::Table<CPPTestCaseObject>&)table
{
    if (!m_table) {
        m_table = std::make_shared<WCDB::Table<CPPTestCaseObject>>(self.database->getTable<CPPTestCaseObject>(self.tableName.UTF8String));
    }
    return *m_table.get();
}

- (BOOL)createObjectTable
{
    return self.database->createTable<CPPTestCaseObject>(self.tableName.UTF8String);
}

- (void)doTestObjects:(const WCDB::ValueArray<CPPTestCaseObject>&)objects
               andSQL:(NSString*)sql
    afterModification:(BOOL (^)())block
{
    [self doTestObjects:objects andSQLs:@[ sql ] afterModification:block];
}

- (void)doTestObjects:(const WCDB::ValueArray<CPPTestCaseObject>&)objects
              andSQLs:(NSArray<NSString*>*)sqls
    afterModification:(BOOL (^)())block
{
    TestCaseAssertTrue(sqls.count > 0);
    [self doTestSQLs:sqls inOperation:block];
    WCDB::ValueArray<CPPTestCaseObject> selected = [self getAllObjects];
    [self check:CPPMultiRowValueExtract(selected) isEqualTo:CPPMultiRowValueExtract(objects)];
}

- (void)doTestObjects:(const WCDB::ValueArray<CPPTestCaseObject>&)objects
            andNumber:(int)numberOfInsertSQLs
         ofInsertSQLs:(NSString*)insertSQL
       afterInsertion:(BOOL (^)())block
{
    TestCaseAssertTrue(numberOfInsertSQLs > 0);
    TestCaseAssertTrue(insertSQL != nil);
    NSMutableArray<NSString*>* sqls = [NSMutableArray array];
    if (numberOfInsertSQLs > 1) {
        [sqls addObject:@"BEGIN IMMEDIATE"];
    }
    for (int i = 0; i < numberOfInsertSQLs; ++i) {
        [sqls addObject:insertSQL];
    }
    if (numberOfInsertSQLs > 1) {
        [sqls addObject:@"COMMIT"];
    }
    [self doTestObjects:objects
                andSQLs:sqls
      afterModification:block];
}

- (void)doTestObject:(const CPPTestCaseObject&)object
              andSQL:(NSString*)sql
         bySelecting:(WCDB::Optional<CPPTestCaseObject> (^)())block
{
    [self doTestObjects:object
                andSQLs:@[ sql ]
            bySelecting:^WCDB::OptionalValueArray<CPPTestCaseObject> {
                const WCDB::Optional<CPPTestCaseObject> result = block();
                TestCaseAssertTrue(result.succeed());
                return { result.value() };
            }];
}

- (void)doTestObjects:(const WCDB::ValueArray<CPPTestCaseObject>&)expectedObjects
               andSQL:(NSString*)sql
          bySelecting:(WCDB::OptionalValueArray<CPPTestCaseObject> (^)())block
{
    [self doTestObjects:expectedObjects andSQLs:@[ sql ] bySelecting:block];
}

- (void)doTestObjects:(const WCDB::ValueArray<CPPTestCaseObject>&)expectedObjects
              andSQLs:(NSArray<NSString*>*)expectedSQLs
          bySelecting:(WCDB::OptionalValueArray<CPPTestCaseObject> (^)())block
{
    __block WCDB::ValueArray<CPPTestCaseObject> selected;
    [self doTestSQLs:expectedSQLs
         inOperation:^BOOL {
             auto values = block();
             TestCaseAssertTrue(values.succeed());
             selected = values.value();
             return selected.size() > 0;
         }];
    [self check:CPPMultiRowValueExtract(selected) isEqualTo:CPPMultiRowValueExtract(expectedObjects)];
}

- (WCDB::ValueArray<CPPTestCaseObject>)getAllObjects
{
    auto allObject = self.table.getAllObjects();
    TestCaseAssertTrue(allObject.succeed());
    return allObject.value();
}

#pragma mark - Value

- (BOOL)createValueTable
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
      bySelecting:(WCDB::OptionalOneRow (^)())block
{
    TestCaseAssertTrue(row.size() > 0);
    TestCaseAssertTrue(sql != nil);
    [self doTestRows:{ row }
             andSQLs:@[ sql ]
         bySelecting:^WCDB::OptionalMultiRows {
             WCDB::OptionalOneRow result = block();
             TestCaseAssertTrue(result.succeed());
             return { result.value() };
         }];
}

- (void)doTestColumn:(const WCDB::OneColumnValue&)column
              andSQL:(NSString*)sql
         bySelecting:(WCDB::OptionalOneColumn (^)())block
{
    TestCaseAssertTrue(sql != nil);
    TestCaseAssertTrue(column.size() > 0);
    WCDB::MultiRowsValue rows;
    for (WCDB::Value value : column) {
        rows.push_back({ value });
    }
    [self doTestRows:rows
             andSQLs:@[ sql ]
         bySelecting:^WCDB::OptionalMultiRows {
             WCDB::OptionalOneColumn result = block();
             TestCaseAssertTrue(result.succeed());
             WCDB::MultiRowsValue resultRows;
             for (WCDB::Value value : result.value()) {
                 resultRows.push_back({ value });
             }
             return resultRows;
         }];
}

- (void)doTestValue:(const WCDB::Value&)value
             andSQL:(NSString*)sql
        bySelecting:(WCDB::OptionalValue (^)())block
{
    TestCaseAssertTrue(sql != nil);
    [self doTestRows:{ { value } }
    andSQLs:@[ sql ]
    bySelecting:^WCDB::OptionalMultiRows {
        WCDB::OptionalValue result = block();
        TestCaseAssertTrue(result.succeed());
        static_assert(std::is_constructible<WCDB::MultiRowsValue, std::vector<std::vector<WCDB::Value>>>::value, "");
        WCDB::MultiRowsValue values = { { result.value() } };

        return values;
    }];
}

- (void)doTestRows:(const WCDB::MultiRowsValue&)rows
            andSQL:(NSString*)sql
       bySelecting:(WCDB::OptionalMultiRows (^)())block
{
    TestCaseAssertTrue(sql != nil);
    [self doTestRows:rows andSQLs:@[ sql ] bySelecting:block];
}

- (void)doTestRows:(const WCDB::MultiRowsValue&)expectedRows
           andSQLs:(NSArray<NSString*>*)expectedSQLs
       bySelecting:(WCDB::OptionalMultiRows (^)())block
{
    __block WCDB::MultiRowsValue selected;
    [self doTestSQLs:expectedSQLs
         inOperation:^BOOL {
             auto values = block();
             TestCaseAssertTrue(values.succeed());
             selected = values.value();
             return values.succeed();
         }];
    [self check:selected isEqualTo:expectedRows];
}

- (void)check:(const WCDB::MultiRowsValue&)selected isEqualTo:(const WCDB::MultiRowsValue&)expectedRows
{
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
    auto values = self.database->getAllRowsFromStatement(WCDB::StatementSelect().select(self.resultColumns).from(self.tableName.UTF8String));
    TestCaseAssertTrue(values.succeed());
    return values.value();
}

@end
