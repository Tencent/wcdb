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

#import "WTCAssert.h"
#import "WTCBaseTestCase.h"
#import "WTCColumnConstraintAutoIncrementPrimaryKeyObject+WCTTableCoding.h"
#import "WTCColumnConstraintAutoIncrementPrimaryKeyObject.h"
#import "WTCColumnConstraintBaselineObject+WCTTableCoding.h"
#import "WTCColumnConstraintBaselineObject.h"
#import "WTCColumnConstraintConflictPrimaryKeyObject+WCTTableCoding.h"
#import "WTCColumnConstraintConflictPrimaryKeyObject.h"
#import "WTCColumnConstraintDefaultObject+WCTTableCoding.h"
#import "WTCColumnConstraintDefaultObject.h"
#import "WTCColumnConstraintNotNullObject+WCTTableCoding.h"
#import "WTCColumnConstraintNotNullObject.h"
#import "WTCColumnConstraintOrderedPrimaryKeyObject+WCTTableCoding.h"
#import "WTCColumnConstraintOrderedPrimaryKeyObject.h"
#import "WTCColumnConstraintPrimaryKeyObject+WCTTableCoding.h"
#import "WTCColumnConstraintPrimaryKeyObject.h"
#import "WTCColumnConstraintUniqueObject+WCTTableCoding.h"
#import "WTCColumnConstraintUniqueObject.h"

@interface WTCColumnConstraintBindingTests : WTCBaseTestCase

@end

@implementation WTCColumnConstraintBindingTests

- (WCDB::StatementCreateTable)generateCreateTableStatementForClass:(Class<WCTTableCoding>)cls
{
    return [cls objectRelationalMappingForWCDB]->generateCreateTableStatement(NSStringFromClass(cls).UTF8String);
}

- (void)testColumnConstraintBinding
{
    WINQAssertEqual([self generateCreateTableStatementForClass:WTCColumnConstraintBaselineObject.class], @"CREATE TABLE IF NOT EXISTS WTCColumnConstraintBaselineObject(variable INTEGER)");

    WINQAssertEqual([self generateCreateTableStatementForClass:WTCColumnConstraintPrimaryKeyObject.class], @"CREATE TABLE IF NOT EXISTS WTCColumnConstraintPrimaryKeyObject(primaryVariable INTEGER PRIMARY KEY)");

    WINQAssertEqual([self generateCreateTableStatementForClass:WTCColumnConstraintOrderedPrimaryKeyObject.class], @"CREATE TABLE IF NOT EXISTS WTCColumnConstraintOrderedPrimaryKeyObject(orderedPrimaryVariable INTEGER PRIMARY KEY ASC)");

    WINQAssertEqual([self generateCreateTableStatementForClass:WTCColumnConstraintAutoIncrementPrimaryKeyObject.class], @"CREATE TABLE IF NOT EXISTS WTCColumnConstraintAutoIncrementPrimaryKeyObject(autoIncrementPrimaryVariable INTEGER PRIMARY KEY AUTOINCREMENT)");

    WINQAssertEqual([self generateCreateTableStatementForClass:WTCColumnConstraintConflictPrimaryKeyObject.class], @"CREATE TABLE IF NOT EXISTS WTCColumnConstraintConflictPrimaryKeyObject(conflictPrimaryVariable INTEGER PRIMARY KEY ON CONFLICT REPLACE)");

    WINQAssertEqual([self generateCreateTableStatementForClass:WTCColumnConstraintNotNullObject.class], @"CREATE TABLE IF NOT EXISTS WTCColumnConstraintNotNullObject(notNullVariable INTEGER NOT NULL)");

    WINQAssertEqual([self generateCreateTableStatementForClass:WTCColumnConstraintUniqueObject.class], @"CREATE TABLE IF NOT EXISTS WTCColumnConstraintUniqueObject(uniqueVariable INTEGER UNIQUE)");

    WINQAssertEqual([self generateCreateTableStatementForClass:WTCColumnConstraintDefaultObject.class], @"CREATE TABLE IF NOT EXISTS WTCColumnConstraintDefaultObject(defaultValueVariable INTEGER DEFAULT 1)");
}

@end
