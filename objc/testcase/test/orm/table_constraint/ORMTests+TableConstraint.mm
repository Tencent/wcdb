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

#import "ORMTableConstraintCheck+WCTTableCoding.h"
#import "ORMTableConstraintCheck.h"
#import "ORMTableConstraintForeignKey+WCTTableCoding.h"
#import "ORMTableConstraintForeignKey.h"
#import "ORMTableConstraintMultiPrimary+WCTTableCoding.h"
#import "ORMTableConstraintMultiPrimary.h"
#import "ORMTableConstraintMultiUnique+WCTTableCoding.h"
#import "ORMTableConstraintMultiUnique.h"
#import "ORMTestCase.h"

@interface ORMTests_TableConstraint : ORMTestCase

@end

@implementation ORMTests_TableConstraint

- (void)test_multi_primary
{
    NSString *constraintName = NSStringFromClass(ORMTableConstraintMultiPrimary.class);
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable1 TEXT, variable2 INTEGER, variable3 REAL, CONSTRAINT %@ PRIMARY KEY(variable1, variable2 ASC, variable3 DESC) ON CONFLICT REPLACE)", _tableName, constraintName];
    [_expectedSQLs addObject:expectedSQL];
    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMTableConstraintMultiPrimary.class]);
}

- (void)test_multi_unique
{
    NSString *constraintName = NSStringFromClass(ORMTableConstraintMultiUnique.class);
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable1 TEXT, variable2 INTEGER, variable3 REAL, CONSTRAINT %@ UNIQUE(variable1, variable2 ASC, variable3 DESC) ON CONFLICT ABORT)", _tableName, constraintName];
    [_expectedSQLs addObject:expectedSQL];
    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMTableConstraintMultiUnique.class]);
}

- (void)test_check
{
    NSString *constraintName = NSStringFromClass(ORMTableConstraintCheck.class);
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable INTEGER, CONSTRAINT %@ CHECK((variable > 2)))", _tableName, constraintName];
    [_expectedSQLs addObject:expectedSQL];
    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMTableConstraintCheck.class]);
}

- (void)test_foreign_key
{
    NSString *constraintName = NSStringFromClass(ORMTableConstraintForeignKey.class);
    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable INTEGER, CONSTRAINT %@ FOREIGN KEY(variable) REFERENCES otherTable ON DELETE SET NULL)", _tableName, constraintName];
    [_expectedSQLs addObject:expectedSQL];
    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:ORMTableConstraintForeignKey.class]);
}

@end
