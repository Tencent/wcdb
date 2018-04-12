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
    std::list<std::shared_ptr<WCDB::MigrationInfo>> infoList;
    for (WCTMigrationInfo *info in infos) {
        infoList.push_back([info getWCDBMigrationInfo]);
    }
    auto migrationInfos = WCDB::MigrationInfos::infos(infoList);
    if (!migrationInfos) {
        return nil;
    }
    return [self initWithDatabase:WCDB::MigrationDatabase::databaseWithPath(path.UTF8String, migrationInfos)];
}

- (instancetype)initWithPath:(NSString *)path
                     andInfo:(WCTMigrationInfo *)info
{
    auto infos = WCDB::MigrationInfos::infos({[info getWCDBMigrationInfo]});
    if (!infos) {
        return nil;
    }
    return [self initWithDatabase:WCDB::MigrationDatabase::databaseWithPath(path.UTF8String, infos)];
}

- (instancetype)initWithPath:(NSString *)path
{
    return [self initWithDatabase:WCDB::MigrationDatabase::databaseWithPath(path.UTF8String, nullptr)];
}

- (instancetype)initWithExistingTag:(WCTTag)tag
{
    return [self initWithDatabase:WCDB::MigrationDatabase::databaseWithExistingTag(tag)];
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

- (void)finalizeDatabase
{
    _migrationDatabase = nullptr;
    [super finalizeDatabase];
}

@end
