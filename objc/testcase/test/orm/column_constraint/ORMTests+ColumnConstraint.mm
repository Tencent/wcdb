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

#import "ORMColumnConstraintNotNull+WCTTableCoding.h"
#import "ORMColumnConstraintNotNull.h"
#import "ORMColumnConstraintPrimary+WCTTableCoding.h"
#import "ORMColumnConstraintPrimary.h"
#import "ORMColumnConstraintPrimaryAsc+WCTTableCoding.h"
#import "ORMColumnConstraintPrimaryAsc.h"
#import "ORMColumnConstraintPrimaryAscAutoIncrement+WCTTableCoding.h"
#import "ORMColumnConstraintPrimaryAscAutoIncrement.h"
#import "ORMColumnConstraintPrimaryAutoIncrement+WCTTableCoding.h"
#import "ORMColumnConstraintPrimaryAutoIncrement.h"
#import "ORMColumnConstraintPrimaryDesc+WCTTableCoding.h"
#import "ORMColumnConstraintPrimaryDesc.h"
#import "ORMColumnConstraintPrimaryImpl+WCTTableCoding.h"
#import "ORMColumnConstraintPrimaryImpl.h"
#import "ORMColumnConstraintUnique+WCTTableCoding.h"
#import "ORMColumnConstraintUnique.h"
#import "ORMTestCase.h"

@interface ORMTests_ColumnConstraint : ORMTestCase

@end

@implementation ORMTests_ColumnConstraint

- (void)test_primary
{
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable INTEGER PRIMARY KEY)", _tableName];
    [_expectedSQLs addObject:expectedSQL];

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMColumnConstraintPrimary.class]);
}

- (void)test_primary_asc
{
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable INTEGER PRIMARY KEY ASC)", _tableName];
    [_expectedSQLs addObject:expectedSQL];

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMColumnConstraintPrimaryAsc.class]);
}

- (void)test_primary_desc
{
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable INTEGER PRIMARY KEY DESC)", _tableName];
    [_expectedSQLs addObject:expectedSQL];

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMColumnConstraintPrimaryDesc.class]);
}

- (void)test_primary_auto_increment
{
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable INTEGER PRIMARY KEY AUTOINCREMENT)", _tableName];
    [_expectedSQLs addObject:expectedSQL];

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMColumnConstraintPrimaryAutoIncrement.class]);
}

- (void)test_primary_asc_auto_increment
{
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable INTEGER PRIMARY KEY ASC AUTOINCREMENT)", _tableName];
    [_expectedSQLs addObject:expectedSQL];

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMColumnConstraintPrimaryAscAutoIncrement.class]);
}

- (void)test_primary_impl
{
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable INTEGER PRIMARY KEY ASC ON CONFLICT REPLACE AUTOINCREMENT)", _tableName];
    [_expectedSQLs addObject:expectedSQL];

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMColumnConstraintPrimaryImpl.class]);
}

- (void)test_not_null
{
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable INTEGER NOT NULL)", _tableName];
    [_expectedSQLs addObject:expectedSQL];

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMColumnConstraintNotNull.class]);
}

- (void)test_unique
{
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable INTEGER UNIQUE)", _tableName];
    [_expectedSQLs addObject:expectedSQL];

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMColumnConstraintUnique.class]);
}

@end
