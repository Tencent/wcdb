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

#import "WINQAssertion.h"

@interface WCTPropertyTests : BaseTestCase

@end

@implementation WCTPropertyTests {
    WCTColumnBinding columnBinding1;
    WCTColumnBinding columnBinding2;
}

- (void)setUp
{
    [super setUp];
    columnBinding1 = WCTColumnBinding::generate<int>(WCTPropertyTests.class, "testProperty1");
    columnBinding2 = WCTColumnBinding::generate<double>(WCTPropertyTests.class, "testProperty2");
}

- (void)test_default_constructible
{
    WCTProperty constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_property_convertible
{
    WCTProperty sql(columnBinding1);
    NSString* expected = @"testProperty1";
    TestCaseAssertWINQConvertible(WCDB::Column, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::Columns, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::Expression, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::Expressions, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::IndexedColumn, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::IndexedColumns, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumn, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumns, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::OrderingTerm, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::OrderingTerms, sql, expected);

    TestCaseAssertWINQConvertible(WCTProperty, sql, expected);
    TestCaseAssertWINQConvertible(WCTProperties, sql, expected);
    TestCaseAssertWINQConvertible(WCTResultColumn, sql, expected);
    TestCaseAssertWINQConvertible(WCTResultColumns, sql, expected);
}

- (void)test_properties_convertible
{
    WCTProperties sqls = {
        WCTProperty(columnBinding1),
        WCTProperty(columnBinding2),
    };
    NSString* expected = @"testProperty1, testProperty2";
    TestCaseAssertWINQConvertible(WCDB::Columns, sqls, expected);
    TestCaseAssertWINQConvertible(WCDB::Expressions, sqls, expected);
    TestCaseAssertWINQConvertible(WCDB::IndexedColumns, sqls, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumns, sqls, expected);
    TestCaseAssertWINQConvertible(WCDB::OrderingTerms, sqls, expected);

    TestCaseAssertWINQConvertible(WCTProperties, sqls, expected);
    TestCaseAssertWINQConvertible(WCTResultColumns, sqls, expected);
}

- (void)test_redirect
{
    WCTProperty property(columnBinding1);
    WCTResultColumn resultColumn = property.redirect(1);

    TestCaseAssertSQLEqual(resultColumn, @"1");
    TestCaseAssertEqual(resultColumn.getColumnBinding(), property.getColumnBinding());
}

- (void)test_as_indexed_column
{
    WCTProperty property(columnBinding1);

    TestCaseAssertSQLEqual(property.asIndex(), @"testProperty1");
}

- (void)test_as_indexed_column_with_order
{
    WCTProperty property(columnBinding1);

    TestCaseAssertSQLEqual(property.asIndex(WCTOrderedAscending), @"testProperty1 ASC");
}

- (void)test_as_ordering_term
{
    WCTProperty property(columnBinding1);

    TestCaseAssertSQLEqual(property.asOrder(), @"testProperty1");
}

- (void)test_as_ordering_term_with_order
{
    WCTProperty property(columnBinding1);

    TestCaseAssertSQLEqual(property.asOrder(WCTOrderedAscending), @"testProperty1 ASC");
}

- (void)test_properties_count
{
    WCTProperties properties = {
        WCTProperty(columnBinding1),
        WCTProperty(columnBinding2),
    };
    TestCaseAssertSQLEqual(properties.count(), @"count(*)");
}

- (void)test_properties_redirect
{
    WCTProperties properties = {
        WCTProperty(columnBinding1),
        WCTProperty(columnBinding2),
    };
    WCTResultColumns resultColumns = properties.redirect({ 1, 2 });
    TestCaseAssertSQLEqual(resultColumns[0], @"1");
    TestCaseAssertSQLEqual(resultColumns[1], @"2");
    TestCaseAssertEqual(resultColumns[0].getColumnBinding(), properties[0].getColumnBinding());
    TestCaseAssertEqual(resultColumns[1].getColumnBinding(), properties[1].getColumnBinding());
}

- (void)test_properties_redirector
{
    WCTProperties properties = {
        WCTProperty(columnBinding1),
        WCTProperty(columnBinding2),
    };
    WCTResultColumns resultColumns = properties.redirect([self](const WCTProperty& property) -> WCDB::ResultColumn {
        return property.table(@"testTable");
    });
    TestCaseAssertSQLEqual(resultColumns[0], @"main.testTable.testProperty1");
    TestCaseAssertSQLEqual(resultColumns[1], @"main.testTable.testProperty2");
    TestCaseAssertEqual(resultColumns[0].getColumnBinding(), properties[0].getColumnBinding());
    TestCaseAssertEqual(resultColumns[1].getColumnBinding(), properties[1].getColumnBinding());
}

- (void)test_properties_redirect_unpaired
{
    {
        WCTProperties properties = {
            WCTProperty(columnBinding1),
            WCTProperty(columnBinding2),
        };
        WCTResultColumns resultColumns = properties.redirect(1);
        TestCaseAssertEqual(resultColumns.size(), 1);
    }
    {
        WCTProperties properties = {
            WCTProperty(columnBinding1),
        };
        WCTResultColumns resultColumns = properties.redirect({ 1, 2 });
        TestCaseAssertEqual(resultColumns.size(), 1);
    }
}

- (void)test_property_table
{
    WCTProperty property(columnBinding1);
    NSString* table = @"testTable";
    WCDB::Expression expression = property.table(table);
    TestCaseAssertSQLEqual(expression, @"main.testTable.testProperty1");
}

- (void)test_properties_table
{
    WCTProperties properties = {
        WCTProperty(columnBinding1),
        WCTProperty(columnBinding2),
    };
    NSString* table = @"testTable";
    WCDB::Expressions expressions = properties.table(table);
    TestCaseAssertEqual(expressions.size(), properties.size());
    TestCaseAssertSQLEqual(expressions[0], @"main.testTable.testProperty1");
    TestCaseAssertSQLEqual(expressions[1], @"main.testTable.testProperty2");
}

- (void)test_properties_by_adding
{
    WCTProperties properties = {
        WCTProperty(columnBinding1),
        WCTProperty(columnBinding2),
    };
    WCTProperties newProperties = properties.addingNewProperties(properties);
    TestCaseAssertEqual(properties.size(), 4);
    TestCaseAssertEqual(newProperties.size(), 4);
}

- (void)test_properties_adding
{
    WCTProperties properties = {
        WCTProperty(columnBinding1),
        WCTProperty(columnBinding2),
    };
    WCTProperties newProperties = properties.propertiesByAddingNewProperties(properties);
    TestCaseAssertEqual(properties.size(), 2);
    TestCaseAssertEqual(newProperties.size(), 4);
}

@end
