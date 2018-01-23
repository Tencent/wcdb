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
#import "WTCIndexBaselineIndexObject+WCTTableCoding.h"
#import "WTCIndexBaselineIndexObject.h"
#import "WTCIndexMultiIndexesObject+WCTTableCoding.h"
#import "WTCIndexMultiIndexesObject.h"
#import "WTCIndexUniqueIndexObject+WCTTableCoding.h"
#import "WTCIndexUniqueIndexObject.h"

@interface WTCIndexConstraintBindingTests : WTCBaseTestCase

@end

@implementation WTCIndexConstraintBindingTests

- (WCDB::StatementCreateIndex)generateCreateIndexStatementForClass:(Class<WCTTableCoding>)cls
{
    return [cls objectRelationalMappingForWCDB]->getIndexBindingMap()->begin()->second->generateCreateIndexStatement(NSStringFromClass(cls).UTF8String);
}

- (void)testIndexBinding
{
    WINQAssertEqual([self generateCreateIndexStatementForClass:WTCIndexBaselineIndexObject.class], @"CREATE INDEX IF NOT EXISTS WTCIndexBaselineIndexObject_index ON WTCIndexBaselineIndexObject(variable)");

    WINQAssertEqual([self generateCreateIndexStatementForClass:WTCIndexUniqueIndexObject.class], @"CREATE UNIQUE INDEX IF NOT EXISTS WTCIndexUniqueIndexObject_index ON WTCIndexUniqueIndexObject(variable)");

    WINQAssertEqual([self generateCreateIndexStatementForClass:WTCIndexMultiIndexesObject.class], @"CREATE INDEX IF NOT EXISTS WTCIndexMultiIndexesObject_index ON WTCIndexMultiIndexesObject(variable1, variable2)");
}

@end
