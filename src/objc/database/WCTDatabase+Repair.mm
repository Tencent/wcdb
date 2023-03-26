//
// Created by sanhuazhang on 2019/05/02
//

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

#import "Assertion.hpp"
#import "Core.h"
#import "WCTCommon.h"
#import "WCTDatabase+Private.h"
#import "WCTDatabase+Repair.h"
#import "WCTFoundation.h"

@implementation WCTDatabase (Repair)

- (void)setNotificationWhenCorrupted:(WCTCorruptedNotificationBlock)onCorrupted
{
    WCDB::Core::CorruptedNotification notification = nullptr;
    if (onCorrupted != nil) {
        notification = [onCorrupted](WCDB::InnerDatabase *database) {
            onCorrupted([[WCTDatabase alloc] initWithUnsafeDatabase:database]);
        };
    }
    WCDB::Core::shared().setNotificationWhenDatabaseCorrupted(self.path, notification);
}

- (BOOL)checkIfCorrupted
{
    _database->checkIntegrity(false);
    return self.isAlreadyCorrupted;
}

- (BOOL)isAlreadyCorrupted
{
    return WCDB::Core::shared().isFileObservedCorrupted(self.path);
}

- (void)filterBackup:(WCTBackupFilterBlock)tableShouldBeBackedUp
{
    WCDB::InnerDatabase::BackupFilter filter = nullptr;
    if (tableShouldBeBackedUp != nil) {
        filter = [tableShouldBeBackedUp](const WCDB::UnsafeStringView &tableName) -> bool {
            return tableShouldBeBackedUp([NSString stringWithView:tableName]);
        };
    }
    _database->filterBackup(filter);
}

- (BOOL)deposit
{
    return _database->deposit();
}

- (void)enableAutoBackup:(BOOL)flag
{
    WCDB::Core::shared().enableAutoBackup(_database, flag);
}

- (BOOL)backup
{
    return _database->backup(false);
}

- (double)retrieve:(WCDB_NO_ESCAPE WCTRetrieveProgressUpdateBlock)onProgressUpdated
{
    WCDB::InnerDatabase::RetrieveProgressCallback callback = nullptr;
    if (onProgressUpdated != nil) {
        callback = [onProgressUpdated](double percentage, double increment) {
            onProgressUpdated(percentage, increment);
        };
    }
    return _database->retrieve(callback);
}

- (BOOL)removeDeposited
{
    return _database->removeDeposited();
}

- (BOOL)containsDeposited
{
    return _database->containsDeposited();
}

@end
