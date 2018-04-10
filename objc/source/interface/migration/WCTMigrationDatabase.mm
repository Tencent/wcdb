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
{
    if (self = [self initWithDatabase:WCDB::MigrationDatabase::databaseWithPath(path.UTF8String)]) {
    }
    return self;
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

- (void)setMigrationInfo:(WCTMigrationInfo *)info
{
    _migrationDatabase->setMigrationInfo([info getWCDBMigrationInfo]);
}

- (BOOL)stepMigration:(BOOL &)done
{
    return _migrationDatabase->stepMigration((bool &) done);
}

- (const WCDB::MigrationInfo *)getWCDBMigrationInfo
{
    return _migrationDatabase->getMigrationInfo();
}

- (void)finalizeDatabase
{
    _migrationDatabase = nullptr;
    [super finalizeDatabase];
}

@end
