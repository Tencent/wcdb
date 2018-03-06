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

#import <WCDB/WCTDatabase.h>

/**
 Controllable Transaction block
 */
typedef BOOL (^WCTControllableTransactionBlock)(WCTUnsafeHandle *);

/**
 Transaction block
 */
typedef void (^WCTTransactionBlock)(WCTUnsafeHandle *);

@interface WCTDatabase (Transaction)

/**
 @brief This interface is equivalent to [database runControllableTransaction:transaction withError:nil];
 @param inTransaction Operation inside transaction.
 @see WCTControllableTransactionBlock
 */
- (BOOL)runControllableTransaction:(WCTControllableTransactionBlock)inTransaction;

/**
 @brief This interface is equivalent to [database runNestedTransaction:transaction withError:nil];
 @param inTransaction Operation inside transaction.
 @see WCTTransactionBlock
 */
- (BOOL)runNestedTransaction:(WCTTransactionBlock)inTransaction;

/**
 @brief This interface is equivalent to [database runTransaction:transaction withError:nil];
 @param inTransaction Operation inside transaction.
 */
- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction;

/**
 @brief Separate interface of runTransaction:
 @warning You should call begin, commit, rollback and all other operations in same thread.
 @return YES only if no error occurs.
 */
- (BOOL)begin;

/**
 @brief Separate interface of runTransaction:
 @warning You should call begin , commit , rollback and all other operations in same thread.
 @return YES only if no error occurs.
 */
- (BOOL)commit;

/**
 @brief Separate interface of runTransaction:
 @warning You should call begin , commit , rollback and all other operations in same thread.
 @return YES only if no error occurs.
 */
- (BOOL)rollback;

@end
