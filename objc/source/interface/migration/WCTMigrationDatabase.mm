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

#import <WCDB/Error.hpp>
#import <WCDB/Interface.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTMigrationInfo+Private.h>
#import <atomic>

@implementation WCTMigrationDatabase {
    WCDB::MigrationDatabase *_migrationDatabase;
}

- (instancetype)initWithPath:(NSString *)path
                    andInfos:(NSArray<WCTMigrationInfo *> *)infos
{
    WCTRemedialAssert(path, "Path can't be null.", return nil;);
    WCTRemedialAssert(infos.count > 0, "Migration infos can't be null or empty. If you want to refer a inited migration database, use [initWithExistingPath:] instead.", return nil;);
    std::list<std::shared_ptr<WCDB::MigrationInfo>> infoList;
    for (WCTMigrationInfo *info in infos) {
        infoList.push_back([info getWCDBMigrationInfo]);
    }
    auto migrationInfos = WCDB::MigrationSetting::setting(infoList);
    if (!migrationInfos) {
        return nil;
    }
    return [self initWithDatabase:WCDB::MigrationDatabase::databaseWithPath(path.cppString, migrationInfos)];
}

- (instancetype)initWithPath:(NSString *)path
                     andInfo:(WCTMigrationInfo *)info
{
    WCTRemedialAssert(info, "Migration info can't be null. If you want to refer an inited migration database, use [initWithExistingPath:] instead.", return nil;);
    WCTRemedialAssert(path, "Path can't be null.", return nil;);
    auto infos = WCDB::MigrationSetting::setting({[info getWCDBMigrationInfo]});
    if (!infos) {
        return nil;
    }
    return [self initWithDatabase:WCDB::MigrationDatabase::databaseWithPath(path.cppString, infos)];
}

- (instancetype)initWithExistingPath:(nonnull NSString *)path
{
    return [self initWithDatabase:WCDB::MigrationDatabase::databaseWithExistingPath(path.cppString)];
}

- (instancetype)initWithExistingTag:(WCTTag)tag
{
    return [self initWithDatabase:WCDB::MigrationDatabase::databaseWithExistingTag(tag)];
}

- (instancetype)initWithPath:(NSString *)path
{
    WCTFatalError("Use [initWithExistingPath:] instead.");
    return nil;
}

- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database
{
    if (self = [super initWithDatabase:database]) {
        _migrationDatabase = static_cast<WCDB::MigrationDatabase *>(database.get());
    }
    return self;
}

- (BOOL)stepMigration:(BOOL &)done
{
    return _migrationDatabase->stepMigration((bool &) done);
}

- (void)asyncMigration
{
    _migrationDatabase->asyncMigration();
}

- (void)setMigratedCallback:(WCTMigratedBlock)onMigrated
{
    auto callback = [onMigrated](const WCDB::MigrationInfo *info) {
        WCTMigrationInfo *nsInfo = [[WCTMigrationInfo alloc] initWithWCDBMigrationInfo:info];
        onMigrated(nsInfo);
    };
    _migrationDatabase->getMigrationSetting()->setMigratedCallback(callback);
}

- (void)setMigrateRowPerStep:(int)row
{
    _migrationDatabase->getMigrationSetting()->setMigrateRowPerStep(row);
}

- (void)setConflictCallback:(WCTMigrationConflictBlock)onConflict
{
    WCDB::MigrationSetting::ConflictCallback callback = nullptr;
    if (onConflict) {
        callback = [onConflict](const WCDB::MigrationInfo *info, const long long &rowid) -> bool {
            WCTMigrationInfo *nsInfo = [[WCTMigrationInfo alloc] initWithWCDBMigrationInfo:info];
            return onConflict(nsInfo, rowid);
        };
    }
    _migrationDatabase->getMigrationSetting()->setConflictCallback(callback);
}

- (void)finalizeDatabase
{
    _migrationDatabase = nullptr;
    [super finalizeDatabase];
}

@end
