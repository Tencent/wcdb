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

#import <WCDB/Core.h>
#import <WCDB/WCTDatabase+Transaction.h>
#import <WCDB/WCTTable+Private.h>
#import <WCDB/WCTTable+Transaction.h>

@implementation WCTTable (Transaction)

- (BOOL)beginTransaction
{
    return [_database beginTransaction];
}

- (BOOL)commitOrRollbackTransaction
{
    return [_database commitOrRollbackTransaction];
}

- (void)rollbackTransaction
{
    [_database rollbackTransaction];
}

- (BOOL)runTransaction:(WCDB_NO_ESCAPE WCTTransactionBlock)inTransaction
{
    return [_database runTransaction:inTransaction];
}

- (BOOL)beginNestedTransaction
{
    return [_database beginNestedTransaction];
}

- (BOOL)commitOrRollbackNestedTransaction
{
    return [_database commitOrRollbackNestedTransaction];
}

- (void)rollbackNestedTransaction
{
    [_database rollbackNestedTransaction];
}

- (BOOL)runNestedTransaction:(WCDB_NO_ESCAPE WCTTransactionBlock)inTransaction
{
    return [_database runNestedTransaction:inTransaction];
}

@end
