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

@interface WCTResultColumnTests : WINQTestCase

@property int testProperty1;
@property double testProperty2;

@end

@implementation WCTResultColumnTests {
    WCTProperty property1;
    WCTProperty property2;
}

- (void)setUp
{
    [super setUp];
    property1 = WCTProperty(WCTColumnBinding(WCTResultColumnTests.class, "testProperty1", "testProperty1", (int*) nullptr));
    property2 = WCTProperty(WCTColumnBinding(WCTResultColumnTests.class, "testProperty2", "testProperty2", (double*) nullptr));
}

- (void)test_default_constructible
{
    WCTResultColumn constructible;
}

- (void)test_property_convertible
{
    WCTResultColumn sql(property1);
    NSString* expected = @"testProperty1";
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);

    WINQConvertibleTest(WCTResultColumn, sql, expected);
    WINQConvertibleTest(WCTResultColumns, sql, expected);
}

- (void)test_properties_convertible
{
    WCTResultColumns sqls = {
        property1,
        property2,
    };
    NSString* expected = @"testProperty1, testProperty2";
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);

    WINQConvertibleTest(WCTResultColumns, sqls, expected);
}

- (void)test_redirect
{
    WCTResultColumn resultColumn = property1;
    WCTResultColumn redirected = resultColumn.redirect(1);

    WINQAssertEqual(redirected, @"1");
    XCTAssertEqual(&redirected.getColumnBinding(), &resultColumn.getColumnBinding());
}

- (void)test_results_by_adding
{
    WCTResultColumns resultColumns = {
        property1,
        property2,
    };
    WCTResultColumns newResultColumns = resultColumns.addingNewResultColumns(resultColumns);
    XCTAssertEqual(resultColumns.size(), 4);
    XCTAssertEqual(newResultColumns.size(), 4);
}

- (void)test_results_adding
{
    WCTResultColumns resultColumns = {
        property1,
        property2,
    };
    WCTResultColumns newResultColumns = resultColumns.resultColumnsByAddingNewResultColumns(resultColumns);
    XCTAssertEqual(resultColumns.size(), 2);
    XCTAssertEqual(newResultColumns.size(), 4);
}

@end
