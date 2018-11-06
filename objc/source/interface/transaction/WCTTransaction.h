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

#import <WCDB/Macro.hpp>
#import <WCDB/WCTCommon.h>

NS_ASSUME_NONNULL_BEGIN

typedef BOOL (^WCTTransactionBlock)(WCTHandle *);

@protocol WCTTransaction
@required
- (BOOL)beginTransaction;
- (BOOL)commitOrRollbackTransaction;
- (void)rollbackTransaction;
- (BOOL)runTransaction:(WCDB_NO_ESCAPE WCTTransactionBlock)inTransaction;

- (BOOL)beginNestedTransaction;
- (BOOL)commitOrRollbackNestedTransaction;
- (void)rollbackNestedTransaction;
- (BOOL)runNestedTransaction:(WCDB_NO_ESCAPE WCTTransactionBlock)inTransaction;
@end

NS_ASSUME_NONNULL_END
