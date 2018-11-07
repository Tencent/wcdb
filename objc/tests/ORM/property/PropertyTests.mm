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

#import "BuiltinTypesObject+WCTTableCoding.h"
#import "BuiltinTypesObject.h"
#import "ORMTestCase.h"
#import "PropertyObject+WCTTableCoding.h"
#import "PropertyObject.h"

@interface PropertyTests : ORMTestCase

@end

@implementation PropertyTests

- (void)test_property
{
    self.tableClass = PropertyObject.class;
    NSArray<NSString*>* expected = @[ [NSString stringWithFormat:@"CREATE TABLE main.%@(differentName INTEGER, differentNameAndDefaultValue INTEGER DEFAULT 1, property INTEGER, propertyWithDefaultValue INTEGER DEFAULT 1)", self.tableName] ];
    XCTAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

- (void)test_builtin_types
{
    self.tableClass = BuiltinTypesObject.class;
    NSArray<NSString*>* expected = @[ [NSString stringWithFormat:@"CREATE TABLE main.%@(codingValue BLOB, cppStringValue TEXT, cstringValue TEXT, dataValue BLOB, dateValue REAL, doubleValue REAL, floatValue REAL, int32Value INTEGER, int64Value INTEGER, integerValue INTEGER, intValue INTEGER, numberValue REAL, stringValue TEXT, uint32Value INTEGER, uint64Value INTEGER, uintegerValue INTEGER, unsignedIntValue INTEGER)", self.tableName] ];
    XCTAssertTrue([self checkCreateTableAndIndexSQLsAsExpected:expected]);
}

- (void)test_all_properties
{
    XCTAssertEqual(4, PropertyObject.allProperties.size());
}

@end
