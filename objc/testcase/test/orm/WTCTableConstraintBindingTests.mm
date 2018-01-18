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

#import "WTCBaseTestCase.h"
#import "WTCAssert.h"
#import "WTCTableConstraintBaselineMultiPrimaryObject.h"
#import "WTCTableConstraintBaselineMultiPrimaryObject+WCTTableCoding.h"
#import "WTCTableConstraintMultiPrimaryConflictObject.h"
#import "WTCTableConstraintMultiPrimaryConflictObject+WCTTableCoding.h"
#import "WTCTableConstraintBaselineMultiUniqueObject.h"
#import "WTCTableConstraintBaselineMultiUniqueObject+WCTTableCoding.h"
#import "WTCTableConstraintMultiUniqueConflictObject.h"
#import "WTCTableConstraintMultiUniqueConflictObject+WCTTableCoding.h"
#import "WTCTableConstraintCheckObject.h"
#import "WTCTableConstraintCheckObject+WCTTableCoding.h"
#import "WTCTableConstraintForeignKeyObject.h"
#import "WTCTableConstraintForeignKeyObject+WCTTableCoding.h"

@interface WTCTableConstraintBindingTests : WTCBaseTestCase

@end

@implementation WTCTableConstraintBindingTests

- (WCDB::StatementCreateTable)generateCreateTableStatementForClass:(Class<WCTTableCoding>)cls
{
    return [cls objectRelationalMappingForWCDB]->generateCreateTableStatement(NSStringFromClass(cls).UTF8String);
}

- (void)testMultiPrimaryBinding
{
    WINQAssertEqual([self generateCreateTableStatementForClass:WTCTableConstraintBaselineMultiPrimaryObject.class], @"CREATE TABLE IF NOT EXISTS WTCTableConstraintBaselineMultiPrimaryObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT WTCTableConstraintBaselineMultiPrimaryObjectConstraint PRIMARY KEY(variable1, variable2))");
    
    WINQAssertEqual([self generateCreateTableStatementForClass: WTCTableConstraintMultiPrimaryConflictObject.class], @"CREATE TABLE IF NOT EXISTS WTCTableConstraintMultiPrimaryConflictObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT WTCTableConstraintMultiPrimaryConflictObjectConstraint PRIMARY KEY(variable1, variable2) ON CONFLICT REPLACE)");
}

- (void)testMultiUniqueBinding
{
    WINQAssertEqual([self generateCreateTableStatementForClass:WTCTableConstraintBaselineMultiUniqueObject.class], @"CREATE TABLE IF NOT EXISTS WTCTableConstraintBaselineMultiUniqueObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT WTCTableConstraintBaselineMultiUniqueObjectConstraint UNIQUE(variable1, variable2))");
    
    WINQAssertEqual([self generateCreateTableStatementForClass:WTCTableConstraintMultiUniqueConflictObject.class], @"CREATE TABLE IF NOT EXISTS WTCTableConstraintMultiUniqueConflictObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT WTCTableConstraintMultiUniqueConflictObjectConstraint UNIQUE(variable1, variable2) ON CONFLICT REPLACE)");
}

- (void)testCheckBinding {
    WINQAssertEqual([self generateCreateTableStatementForClass:WTCTableConstraintCheckObject.class], @"CREATE TABLE IF NOT EXISTS WTCTableConstraintCheckObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT WTCTableConstraintCheckObjectConstraint CHECK(variable1 > 1))");
}

- (void)testForeignKeyBinding {
    WINQAssertEqual([self generateCreateTableStatementForClass:WTCTableConstraintForeignKeyObject.class], @"CREATE TABLE IF NOT EXISTS WTCTableConstraintForeignKeyObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT WTCTableConstraintForeignKeyObjectConstraint FOREIGN KEY(variable1) REFERENCES WTCTableConstraintForeignKeyObject(variable2))");
}

@end
