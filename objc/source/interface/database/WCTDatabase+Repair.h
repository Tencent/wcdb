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

typedef BOOL (^WCTBackupFilterBlock)(NSString *);
typedef void (^WCTRetrieveProgressUpdateBlock)(double, double); // percentage, increment
typedef BOOL (^WCTRecoverNotificationBlock)(WCTDatabase *);

typedef NS_ENUM(NSInteger, WCTRecoveryMode) {
    WCTRecoveryModeCustom = 0,
    WCTRecoveryModeRemove = 1,
    WCTRecoveryModeDeposit = 2,
};

@interface WCTDatabase (Repair)

@property (nonatomic, assign) WCTRecoveryMode recoveryMode;

- (void)setNotificationWhenRecovering:(nullable WCTRecoverNotificationBlock)onRecovering;

- (void)setAutoBackup:(BOOL)flag;

- (BOOL)backup;

- (void)filterBackup:(nullable WCTBackupFilterBlock)tableShouldBeBackedUp;

- (BOOL)deposit;

- (BOOL)removeDeposit;

- (BOOL)hasDeposited;

- (double)retrieve:(nullable WCTRetrieveProgressUpdateBlock)onProgressUpdate;

@end

NS_ASSUME_NONNULL_END
