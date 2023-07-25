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

#import "CoreConst.h"
#import "WCTDatabase+Migration.h"
#import "WCTDatabase+Private.h"
#import "WCTMigrationInfo+Private.h"

@implementation WCTDatabase (Migration)

- (void)addMigration:(nullable NSString*)sourceDatabasePath
          withFilter:(nullable WCDB_ESCAPE WCTMigrationFilterBlock)filter
{
    [self addMigration:sourceDatabasePath withSourceCipher:nil withFilter:filter];
}

- (void)addMigration:(nullable NSString*)sourceDatabasePath
    withSourceCipher:(nullable NSData*)cipher
          withFilter:(nullable WCDB_ESCAPE WCTMigrationFilterBlock)filter
{
    WCDB::InnerDatabase::TableFilter callback = nullptr;
    if (filter != nil) {
        callback = [filter](WCDB::MigrationUserInfo& userInfo) {
            WCTMigrationUserInfo* nsUserInfo = [[WCTMigrationUserInfo alloc] initWithBaseInfo:userInfo];
            filter(nsUserInfo);
            if (nsUserInfo.sourceTable.length > 0) {
                userInfo.setSource(nsUserInfo.sourceTable, nsUserInfo.filterCondition);
            }
        };
    }
    _database->addMigration(sourceDatabasePath, cipher, callback);
}

- (BOOL)stepMigration
{
    auto done = _database->stepMigration(false);
    return done.succeed();
}

- (void)enableAutoMigration:(BOOL)flag
{
    WCDB::Core::shared().enableAutoMigration(_database, flag);
}

- (void)setNotificationWhenMigrated:(WCTMigratedNotificationBlock)onMigrated
{
    WCDB::InnerDatabase::MigratedCallback callback = nullptr;
    if (onMigrated != nil) {
        callback = [onMigrated](WCDB::InnerDatabase* database, const WCDB::MigrationBaseInfo* info) {
            WCTMigrationBaseInfo* nsInfo = nil;
            if (info != nil) {
                nsInfo = [[WCTMigrationBaseInfo alloc] initWithBaseInfo:*info];
            }
            WCTDatabase* nsDatabase = [[WCTDatabase alloc] initWithUnsafeDatabase:database];
            onMigrated(nsDatabase, nsInfo);
        };
    }
    _database->setNotificationWhenMigrated(callback);
}

- (BOOL)isMigrated
{
    return _database->isMigrated();
}

@end
