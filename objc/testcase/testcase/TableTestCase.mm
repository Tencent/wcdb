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

#import <TestCase/TableTestCase.h>
#import <TestCase/TestCaseAssertion.h>
#import <TestCase/TestCaseObject+WCTTableCoding.h>
#import <TestCase/TestCaseObject.h>

@implementation TableTestCase

- (void)setUp
{
    [super setUp];
    if (self.isVirtualTable) {
        TestCaseAssertTrue([self createVirtualTable]);
    } else {
        TestCaseAssertTrue([self createTable]);
    }
}

- (NSString*)tableName
{
    if (!_tableName) {
        _tableName = @"testTable";
    }
    return _tableName;
}

- (Class)tableClass
{
    if (!_tableClass) {
        _tableClass = TestCaseObject.class;
    }
    return _tableClass;
}

- (void)tearDown
{
    TestCaseAssertTrue([self dropTable]);
    [super tearDown];
}

- (BOOL)createTable
{
    if (![self.database createTableAndIndexes:self.tableName withClass:self.tableClass]) {
        return NO;
    }
    _table = [self.database getTable:self.tableName withClass:self.tableClass];
    return YES;
}

- (BOOL)createVirtualTable
{
    if (![self.database createVirtualTable:self.tableName withClass:self.tableClass]) {
        return NO;
    }
    _table = [self.database getTable:self.tableName withClass:self.tableClass];
    return YES;
}

- (BOOL)dropTable
{
    if (_table == nil) {
        return YES;
    }
    BOOL result = [self.database dropTable:self.tableName];
    _table = nil;
    return result;
}

@end
