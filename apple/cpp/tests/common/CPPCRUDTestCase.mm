//
// Created by 陈秋文 on 2022/8/14.
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

#import "CPPCRUDTestCase.h"
#import "CPPTestCase.h"

@implementation CPPCRUDTestCase {
    CPPTestCaseObject _object1;
    CPPTestCaseObject _object2;
    WCDB::ValueArray<CPPTestCaseObject> _objects;

    WCDB::OneRowValue _row1;
    WCDB::OneRowValue _row2;
    WCDB::MultiRowsValue _rows;
}

- (void)setUp
{
    [super setUp];
    _object1 = [Random.shared testCaseObjectWithIdentifier:1];
    _object2 = [Random.shared testCaseObjectWithIdentifier:2];
    _objects = { _object1, _object2 };

    _row1 = [Random.shared testCaseValuesWithCount:1 startingFromIdentifier:1][0];
    _row2 = [Random.shared testCaseValuesWithCount:1 startingFromIdentifier:2][0];
    _rows = { self.row1, self.row2 };
}

- (CPPTestCaseObject&)object1
{
    return _object1;
}

- (CPPTestCaseObject&)object2
{
    return _object2;
}

- (WCDB::ValueArray<CPPTestCaseObject>&)objects
{
    return _objects;
}

- (void)insertPresetObjects
{
    TestCaseAssertTrue([self createObjectTable]);
    TestCaseAssertTrue(self.database->insertObjects(_objects, self.tableName.UTF8String));
    self.database->close();
}

- (WCDB::OneRowValue)row1
{
    return _row1;
}

- (WCDB::OneRowValue)row2
{
    return _row2;
}

- (WCDB::MultiRowsValue)rows
{
    return _rows;
}

- (int64_t)nextIdentifier
{
    return 3;
}

- (int)allRowsCount
{
    WCDB::OptionalValue value = self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB::Expression::function("count").invokeAll()).from(self.tableName.UTF8String));
    TestCaseAssertTrue(value.succeed());
    return value.value();
}

- (void)insertPresetRows
{
    TestCaseAssertTrue([self createValueTable]);
    TestCaseAssertTrue(self.database->insertRows(self.rows, self.columns, self.tableName.UTF8String));
    self.database->close();
}

@end
