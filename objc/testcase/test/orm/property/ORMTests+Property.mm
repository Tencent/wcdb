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

#import "ORMProperty+WCTTableCoding.h"
#import "ORMProperty.h"
#import "ORMTestCase.h"

@interface ORMTests_Property : ORMTestCase

@end

@implementation ORMTests_Property

- (void)test
{
    XCTAssertEqual(ORMProperty.property1.getDescription(), "property1");
    XCTAssertEqual(ORMProperty.property2.getDescription(), "renamedProperty2");
    XCTAssertEqual(ORMProperty.property3.getDescription(), "property3");
    XCTAssertEqual(ORMProperty.property4.getDescription(), "renamedProperty4");

    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(property1 INTEGER PRIMARY KEY, property3 TEXT DEFAULT 'defaultString', renamedProperty2 REAL, renamedProperty4 REAL DEFAULT 1.1)", _tableName];
    [_expectedSQLs addObject:expectedSQL];

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMProperty.class]);
}

@end
