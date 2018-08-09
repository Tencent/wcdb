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

typedef BOOL (^WCTMigrationConflictBlock)(WCTMigrationInfo *, long long);

typedef void (^WCTTableMigratedBlock)(WCTMigrationInfo *);

typedef NS_ENUM(NSUInteger, WCTMigrationState) {
    WCTMigrationStateMigrating,
    WCTMigrationStateDone,
};
typedef BOOL (^WCTSteppedBlock)(WCTMigrationState, BOOL);

@interface WCTMigrationDatabase : WCTDatabase

- (instancetype)initWithPath:(NSString *)path;

- (void)setMigrationInfo:(WCTMigrationInfo *)info;

- (void)setMigrationInfos:(NSArray<WCTMigrationInfo *> *)infos;

- (void)setTableMigratedCallback:(nullable WCTTableMigratedBlock)onMigrated;

- (void)setMigrateRowPerStep:(int)row;

- (int)migrateRowPerStep;

- (void)setConflictCallback:(nullable WCTMigrationConflictBlock)onConflict;

- (BOOL)stepMigration:(BOOL &)done;

- (void)asyncMigrationWhenStepped:(nonnull WCTSteppedBlock)onStepped;

- (void)asyncMigrationWithInterval:(double)interval
                     andRetryTimes:(int)retryTimes;

@end

NS_ASSUME_NONNULL_END
