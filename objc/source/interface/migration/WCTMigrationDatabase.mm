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

- (BOOL)stepMigration:(BOOL &)done
      onTableMigrated:(WCTTableMigratedBlock)block
{
    std::function<void(const std::shared_ptr<WCDB::MigrationInfo> &)> callback = nullptr;
    if (block) {
        callback = [block](const std::shared_ptr<WCDB::MigrationInfo> info) {
            WCTMigrationInfo *migrationInfo = [[WCTMigrationInfo alloc] initWithWCDBMigrationInfo:info];
            block(migrationInfo);
        };
    }
    return _migrationDatabase->stepMigration((bool &) done, callback);
}

- (void)asyncMigration:(double)interval
       onTableMigrated:(WCTTableMigratedBlock)onTableMigrated
           onCompleted:(WCTMigrationCompletedBlock)onMigrationCompleted
{
    //TODO add exitable threading and move this code to core layer
    [self asyncMigrationOnStepped:^BOOL(WCTMigrationInfo *_Nullable, BOOL) {
      [NSThread sleepForTimeInterval:interval];
      return YES;
    }
                  onTableMigrated:onTableMigrated
                      onCompleted:onMigrationCompleted];
}

- (void)asyncMigrationOnStepped:(WCTMigrationSteppedBlock)onStepped
                onTableMigrated:(WCTTableMigratedBlock)onTableMigrated
                    onCompleted:(WCTMigrationCompletedBlock)onMigrationCompleted
{
    //TODO using template to refactor block/closure convertion
    std::function<void(const std::shared_ptr<WCDB::MigrationInfo> &)> onTableMigratedCallback = nullptr;
    if (onTableMigrated) {
        onTableMigratedCallback = [onTableMigrated](const std::shared_ptr<WCDB::MigrationInfo> &info) {
            WCTMigrationInfo *migrationInfo = [[WCTMigrationInfo alloc] initWithWCDBMigrationInfo:info];
            return onTableMigrated(migrationInfo);
        };
    }
    static dispatch_queue_t s_migrationQueue = dispatch_queue_create("com.Tencent.WCDB.Migration", DISPATCH_QUEUE_CONCURRENT);
    __weak id weakSelf = self;
    dispatch_async(s_migrationQueue, ^{
      bool done = false;
      while (!done) {
          @autoreleasepool{
              id strongSelf = weakSelf;
              if (!strongSelf) {
                  return;
              }
              bool result = self->_migrationDatabase->stepMigration(done, onTableMigratedCallback);
              if (onStepped) {
                  WCTMigrationInfo *migrationInfo = [[WCTMigrationInfo alloc] initWithWCDBMigrationInfo:self->_migrationDatabase->getMigrationInfo()];
                  result = onStepped(migrationInfo, result) || result;
              }
              if (!result) {
                  break;
              }
          }
      }
      if (onMigrationCompleted) {
          onMigrationCompleted(done);
      }
    });
}

- (void)finalizeDatabase
{
    _migrationDatabase = nullptr;
    [super finalizeDatabase];
}

@end
