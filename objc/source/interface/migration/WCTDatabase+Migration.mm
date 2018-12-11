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

#import <WCDB/WCTDatabase+Migration.h>
#import <WCDB/WCTDatabase+Private.h>

@implementation WCTDatabase (Migration)

- (void)filterMigration:(nullable WCTMigrationFilterBlock)filter
{
    WCTDatabaseAssert(return;);
    WCDB::Database::MigrationFilter callback = nullptr;
    if (filter != nil) {
        callback = [filter](WCDB::MigrationUserInfo& userInfo) {
            WCTMigrationUserInfo* nsUserInfo = [[WCTMigrationUserInfo alloc] initWithMigratedTable:[NSString stringWithUTF8String:userInfo.getMigratedTable().c_str()]];
            filter(nsUserInfo);
            if (nsUserInfo.originTable.length > 0) {
                if (nsUserInfo.originDatabase.length > 0) {
                    userInfo.setOrigin(nsUserInfo.originTable);
                } else {
                    userInfo.setOrigin(nsUserInfo.originTable, nsUserInfo.originDatabase);
                }
            }
        };
    }
    _database->filterMigration(callback);
}

- (BOOL)stepMigration:(BOOL)force isDone:(BOOL&)done
{
    WCTDatabaseAssert(return NO;);
    bool succeed;
    std::tie(succeed, done) = _database->stepMigration((bool) force);
    return succeed;
}

- (void)asyncMigration
{
    WCTDatabaseAssert(return;);
    WCDB::Core::shared()->asyncMigration(_database->getPath());
}

@end
