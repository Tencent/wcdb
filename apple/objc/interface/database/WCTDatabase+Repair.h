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

NS_ASSUME_NONNULL_BEGIN

typedef BOOL (^WCTBackupFilterBlock)(NSString * /* table */);
typedef void (^WCTRetrieveProgressUpdateBlock)(double /* percentage */, double /* increment */);
typedef void (^WCTCorruptedNotificationBlock)(WCTDatabase *);

@interface WCTDatabase (Repair)

- (void)setNotificationWhenCorrupted:(nullable WCDB_ESCAPE WCTCorruptedNotificationBlock)onCorrupted;

- (BOOL)checkIfCorrupted;

- (BOOL)isAlreadyCorrupted;

- (void)enableAutoBackup:(BOOL)flag;

- (BOOL)backup;

- (void)filterBackup:(nullable WCDB_ESCAPE WCTBackupFilterBlock)tableShouldBeBackedUp;

- (BOOL)deposit;

- (BOOL)removeDeposited;

- (BOOL)containsDeposited;

- (double)retrieve:(nullable WCDB_NO_ESCAPE WCTRetrieveProgressUpdateBlock)onProgressUpdate;

@end

NS_ASSUME_NONNULL_END
