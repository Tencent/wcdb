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
#import <WCDB/Interface.h>
#import <WCDB/NSString+CppString.h>
#import <WCDB/WCTCore+Private.h>

@implementation WCTDatabase (Repair)

static_assert((int) WCTRecoveryModeCustom == (int) WCDB::Database::RecoveryMode::Custom, "");
static_assert((int) WCTRecoveryModeRemove == (int) WCDB::Database::RecoveryMode::Remove, "");
static_assert((int) WCTRecoveryModeDeposit == (int) WCDB::Database::RecoveryMode::Deposit, "");

- (void)setRecoveryMode:(WCTRecoveryMode)recoveryMode
{
    _database->setRecoveryMode((WCDB::Database::RecoveryMode) recoveryMode);
}

- (WCTRecoveryMode)recoveryMode
{
    return (WCTRecoveryMode) _database->getRecoverMode();
}

- (void)setNotificationWhenRecovering:(WCTRecoverNotificationBlock)onRecovering
{
    WCDB::Database::RecoverNotification notification = nullptr;
    if (onRecovering) {
        notification = [onRecovering](WCDB::Database *database) -> bool {
            return onRecovering([[WCTDatabase alloc] initWithDatabase:database]);
        };
    }
    _database->setNotificationWhenRecovering(notification);
}

- (void)filterBackup:(WCTBackupFilterBlock)tableShouldBeBackedUp
{
    if (tableShouldBeBackedUp) {
        _database->filterBackup([tableShouldBeBackedUp](const WCDB::String &tableName) -> bool {
            return tableShouldBeBackedUp([NSString stringWithCppString:tableName]);
        });
    } else {
        _database->filterBackup(nullptr);
    }
}

- (BOOL)deposit
{
    return _database->deposit();
}

- (void)setAutoBackup:(BOOL)flag
{
    if (flag) {
        _database->setConfig(WCDB::Core::backupConfigName, WCDB::Core::shared()->backupConfig(), WCDB::Configs::Priority::Low);
    } else {
        _database->removeConfig(WCDB::Core::backupConfigName);
    }
}

- (BOOL)backup
{
    return _database->backup();
}

- (double)retrieve:(WCTRetrieveProgressUpdateBlock)onProgressUpdate
{
    if (onProgressUpdate) {
        return _database->retrieve([onProgressUpdate](double percentage, double increment) {
            onProgressUpdate(percentage, increment);
        });
    } else {
        return _database->retrieve(nullptr);
    }
}

- (BOOL)removeDeposit
{
    return _database->removeDeposit();
}

- (BOOL)canRetrieve
{
    return _database->canRetrieve();
}

@end
