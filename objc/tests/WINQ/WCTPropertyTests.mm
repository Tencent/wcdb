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

#import "WINQTestCase.h"

@interface WCTPropertyTests : WINQTestCase

@property int testProperty1;
@property double testProperty2;

@end

@implementation WCTPropertyTests {
    std::shared_ptr<WCTColumnBinding> columnBinding1;
    std::shared_ptr<WCTColumnBinding> columnBinding2;
}

- (void)setUp
{
    [super setUp];
    columnBinding1.reset(new WCTColumnBinding(WCTPropertyTests.class, "testProperty1", "testProperty1", (int*) nullptr));
    columnBinding2.reset(new WCTColumnBinding(WCTPropertyTests.class, "testProperty2", "testProperty2", (double*) nullptr));
}

- (void)test_default_constructible
{
    WCTProperty constructible;
}

- (void)test_property_convertible
{
    WCTProperty sql(*columnBinding1.get());
    NSString* expected = @"testProperty1";
    WINQConvertibleTest(WCDB::Column, sql, expected);
    WINQConvertibleTest(WCDB::Columns, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);

    WINQConvertibleTest(WCTProperty, sql, expected);
    WINQConvertibleTest(WCTProperties, sql, expected);
    WINQConvertibleTest(WCTResultColumn, sql, expected);
    WINQConvertibleTest(WCTResultColumns, sql, expected);
}

- (void)test_properties_convertible
{
    WCTProperties sqls = {
        WCTProperty(*columnBinding1.get()),
        WCTProperty(*columnBinding2.get()),
    };
    NSString* expected = @"testProperty1, testProperty2";
    WINQConvertibleTest(WCDB::Columns, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);

    WINQConvertibleTest(WCTProperties, sqls, expected);
    WINQConvertibleTest(WCTResultColumns, sqls, expected);
}

- (void)test_redirect
{
    WCTProperty property(*columnBinding1.get());
    WCTResultColumn resultColumn = property.redirect(1);

    WINQAssertEqual(resultColumn, @"1");
    XCTAssertEqual(&resultColumn.getColumnBinding(), &property.getColumnBinding());
}

- (void)test_as_indexed_column
{
    WCTProperty property(*columnBinding1.get());

    WINQAssertEqual(property.asIndex().order(WCTOrderedAscending), @"testProperty1 ASC");
}

- (void)test_as_ordering_term
{
    WCTProperty property(*columnBinding1.get());

    WINQAssertEqual(property.asOrder().order(WCTOrderedAscending), @"testProperty1 ASC");
}

- (void)test_properties_count
{
    WCTProperties properties = {
        WCTProperty(*columnBinding1.get()),
        WCTProperty(*columnBinding2.get()),
    };
    WINQAssertEqual(properties.count(), @"count(*)");
}

- (void)test_properties_redirect
{
    WCTProperties properties = {
        WCTProperty(*columnBinding1.get()),
        WCTProperty(*columnBinding2.get()),
    };
    WCTResultColumns resultColumns = properties.redirect({ 1, 2 });
    WINQAssertEqual(resultColumns[0], @"1");
    WINQAssertEqual(resultColumns[1], @"2");
    XCTAssertEqual(&(resultColumns[0].getColumnBinding()), &(properties[0].getColumnBinding()));
    XCTAssertEqual(&(resultColumns[1].getColumnBinding()), &(properties[1].getColumnBinding()));
}

- (void)test_properties_redirect_unpaired
{
    {
        WCTProperties properties = {
            WCTProperty(*columnBinding1.get()),
            WCTProperty(*columnBinding2.get()),
        };
        WCTResultColumns resultColumns = properties.redirect(1);
        XCTAssertEqual(resultColumns.size(), 1);
    }
    {
        WCTProperties properties = {
            WCTProperty(*columnBinding1.get()),
        };
        WCTResultColumns resultColumns = properties.redirect({ 1, 2 });
        XCTAssertEqual(resultColumns.size(), 1);
    }
}

- (void)test_property_table
{
    WCTProperty property(*columnBinding1.get());
    NSString* table = @"testTable";
    WCDB::Expression expression = property.table(table);
    WINQAssertEqual(expression, @"main.testTable.testProperty1");
}

- (void)test_properties_table
{
    WCTProperties properties = {
        WCTProperty(*columnBinding1.get()),
        WCTProperty(*columnBinding2.get()),
    };
    NSString* table = @"testTable";
    WCDB::Expressions expressions = properties.table(table);
    XCTAssertEqual(expressions.size(), properties.size());
    WINQAssertEqual(expressions[0], @"main.testTable.testProperty1");
    WINQAssertEqual(expressions[1], @"main.testTable.testProperty2");
}

- (void)test_properties_by_adding
{
    WCTProperties properties = {
        WCTProperty(*columnBinding1.get()),
        WCTProperty(*columnBinding2.get()),
    };
    WCTProperties newProperties = properties.addingNewProperties(properties);
    XCTAssertEqual(properties.size(), 4);
    XCTAssertEqual(newProperties.size(), 4);
}

- (void)test_properties_adding
{
    WCTProperties properties = {
        WCTProperty(*columnBinding1.get()),
        WCTProperty(*columnBinding2.get()),
    };
    WCTProperties newProperties = properties.propertiesByAddingNewProperties(properties);
    XCTAssertEqual(properties.size(), 2);
    XCTAssertEqual(newProperties.size(), 4);
}

@end
