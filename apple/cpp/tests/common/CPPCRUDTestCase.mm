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
    WCDB::OneRowValue _row1;
    WCDB::OneRowValue _row2;
    WCDB::MultiRowsValue _rows;
}

- (WCDB::OneRowValue)row1
{
    @synchronized(self) {
        if (_row1.size() == 0) {
            _row1 = [Random.shared testCaseValuesWithCount:1 startingFromIdentifier:1][0];
        }
        return _row1;
    }
}

- (WCDB::OneRowValue)row2
{
    @synchronized(self) {
        if (_row2.size() == 0) {
            _row2 = [Random.shared testCaseValuesWithCount:1 startingFromIdentifier:2][0];
        }
        return _row2;
    }
}

- (WCDB::MultiRowsValue)rows
{
    @synchronized(self) {
        if (_rows.size() == 0) {
            _rows = { self.row1, self.row2 };
        }
        return _rows;
    }
}

- (int64_t)nextIdentifier
{
    return 3;
}

- (int)allRowsCount
{
    WCDB::OptionalValue value = self.database->getValueFromStatement(WCDB::StatementSelect().select(WCDB::Expression::function("count").invokeAll()).from(self.tableName.UTF8String));
    TestCaseAssertTrue(value.has_value());
    return value.value();
}

- (void)insertPresetRows
{
    TestCaseAssertTrue([self createTable]);
    TestCaseAssertTrue(self.database->insertMultiRows(self.rows, self.columns, self.tableName.UTF8String));
    self.database->close();
}

@end
