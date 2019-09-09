//
// Created by sanhuazhang on 2019/05/02
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

#import "WINQAssertion.h"

@interface WCTResultColumnTests : BaseTestCase

@end

@implementation WCTResultColumnTests {
    WCTProperty property1;
    WCTProperty property2;
}

- (void)setUp
{
    [super setUp];
    property1 = WCTProperty(WCTColumnBinding::generate<int>(TestCaseObject.class, "identifier"));
    property2 = WCTProperty(WCTColumnBinding::generate<double>(TestCaseObject.class, "content"));
}

- (void)test_default_constructible
{
    WCTResultColumn constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_property_convertible
{
    WCTResultColumn sql(property1);
    NSString* expected = @"identifier";
    TestCaseAssertWINQConvertible(WCDB::ResultColumn, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumns, sql, expected);

    TestCaseAssertWINQConvertible(WCTResultColumn, sql, expected);
    TestCaseAssertWINQConvertible(WCTResultColumns, sql, expected);
}

- (void)test_properties_convertible
{
    WCTResultColumns sqls = {
        property1,
        property2,
    };
    NSString* expected = @"identifier, content";
    TestCaseAssertWINQConvertible(WCDB::ResultColumns, sqls, expected);

    TestCaseAssertWINQConvertible(WCTResultColumns, sqls, expected);
}

- (void)test_redirect
{
    WCTResultColumn resultColumn = property1;
    WCTResultColumn redirected = resultColumn.redirect(1);

    TestCaseAssertSQLEqual(redirected, @"1");
    TestCaseAssertEqual(redirected.getColumnBinding(), resultColumn.getColumnBinding());
}

- (void)test_results_by_adding
{
    WCTResultColumns resultColumns = {
        property1,
        property2,
    };
    WCTResultColumns newResultColumns = resultColumns.addingNewResultColumns(resultColumns);
    TestCaseAssertEqual(resultColumns.size(), 4);
    TestCaseAssertEqual(newResultColumns.size(), 4);
}

- (void)test_results_adding
{
    WCTResultColumns resultColumns = {
        property1,
        property2,
    };
    WCTResultColumns newResultColumns = resultColumns.resultColumnsByAddingNewResultColumns(resultColumns);
    TestCaseAssertEqual(resultColumns.size(), 2);
    TestCaseAssertEqual(newResultColumns.size(), 4);
}

- (void)test_results_by_removing
{
    WCTResultColumns resultColumns = {
        property1,
        property2,
    };
    WCTResultColumns newResultColumns = resultColumns.removingResultColumns(property1);
    TestCaseAssertEqual(resultColumns.size(), 1);
    TestCaseAssertEqual(newResultColumns.size(), 1);
}

- (void)test_results_removing
{
    WCTResultColumns resultColumns = {
        property1,
        property2,
    };
    WCTResultColumns newResultColumns = resultColumns.resultColumnsByRemovingResultColumns(property1);
    TestCaseAssertEqual(resultColumns.size(), 2);
    TestCaseAssertEqual(newResultColumns.size(), 1);
}

@end
