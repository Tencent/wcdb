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

#import <WCDB/Assertion.hpp>
#import <WCDB/Core.h>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTDatabase+Repair.h>

@implementation WCTDatabase (Repair)

static_assert((int) WCTRecoveryModeCustom == (int) WCDB::Database::RecoveryMode::Custom, "");
static_assert((int) WCTRecoveryModeRemove == (int) WCDB::Database::RecoveryMode::Remove, "");
static_assert((int) WCTRecoveryModeDeposit == (int) WCDB::Database::RecoveryMode::Deposit, "");

- (void)setRecoveryMode:(WCTRecoveryMode)recoveryMode
{
    WCTDatabaseAssert(return;);
    _database->setRecoveryMode((WCDB::Database::RecoveryMode) recoveryMode);
}

- (WCTRecoveryMode)recoveryMode
{
    WCTDatabaseAssert(return WCTRecoveryModeCustom;);
    return (WCTRecoveryMode) _database->getRecoverMode();
}

- (void)setNotificationWhenRecovering:(WCTRecoverNotificationBlock)onRecovering
{
    WCTDatabaseAssert(return;);
    WCDB::Database::RecoverNotification notification = nullptr;
    if (onRecovering) {
        notification = [onRecovering](WCDB::Database *database) -> bool {
            return onRecovering([[WCTDatabase alloc] initWithUnsafeDatabase:database]);
        };
    }
    _database->setNotificationWhenRecovering(notification);
}

- (void)filterBackup:(WCTBackupFilterBlock)tableShouldBeBackedUp
{
    WCTDatabaseAssert(return;);
    WCDB::Database::BackupFilter filter = nullptr;
    if (tableShouldBeBackedUp) {
        filter = [tableShouldBeBackedUp](const WCDB::String &tableName) -> bool {
            return tableShouldBeBackedUp([NSString stringWithUTF8String:tableName.c_str()]);
        };
    }
    _database->filterBackup(filter);
}

- (BOOL)deposit
{
    WCTDatabaseAssert(return NO;);
    return _database->deposit();
}

- (void)setAutoBackup:(BOOL)flag
{
    WCTDatabaseAssert(return;);
    if (flag) {
        _database->setConfig(WCDB::Core::backupConfigName, WCDB::Core::shared()->backupConfig(), WCDB::Configs::Priority::Low);
    } else {
        _database->removeConfig(WCDB::Core::backupConfigName);
    }
}

- (BOOL)backup
{
    WCTDatabaseAssert(return NO;);
    return _database->backup();
}

- (double)retrieve:(WCTRetrieveProgressUpdateBlock)onProgressUpdate
{
    WCTDatabaseAssert(return 0;);
    WCDB::Database::RetrieveProgressCallback callback = nullptr;
    if (onProgressUpdate) {
        callback = [onProgressUpdate](double percentage, double increment) {
            onProgressUpdate(percentage, increment);
        };
    }
    return _database->retrieve(callback);
}

- (BOOL)removeDeposit
{
    WCTDatabaseAssert(return NO;);
    return _database->removeDeposit();
}

- (BOOL)hasDeposited
{
    WCTDatabaseAssert(return NO;);
    return _database->hasDeposited();
}

@end
