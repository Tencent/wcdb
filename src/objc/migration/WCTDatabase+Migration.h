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

#import "WCTDatabase.h"
#import "WCTMigrationInfo.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Triggered at any time when WCDB needs to know whether a table in the current database needs to migrate data,
 mainly including creating a new table, reading and writing a table, and starting to migrate a new table.
 If the current table does not need to migrate data, you need to set the sourceTable in `WCTMigrationUserInfo` to nil.
 */
typedef void (^WCTMigrationFilterBlock)(WCTMigrationUserInfo*);

/**
 Triggered when a table is migrated completely.
 When a table is migrated successfully, tableInfo will carry the information of the table.
 When a database is migrated, tableInfo will .
 */
typedef void (^WCTMigratedNotificationBlock)(WCTDatabase* _Nonnull database, WCTMigrationBaseInfo* _Nullable tableInfo);

WCDB_API @interface WCTDatabase(Migration)

/**
 @brief Configure which tables in the current database need to migrate data, and the source table they need to migrate data from.
 Once configured, you can treat the target table as if it already has all the data of the source table, and can read and write these data through the target table. WCDB will internally convert your CRUD operations on the target table into the CRUD operations on both the target table and the source table appropriately. You neither need to be aware of the existence of the source table, nor care about the progress of data migration.
 @warning  The column definition of the target table must be exactly the same as the column definition of the source table. The database does not record the state of the migration to disk, so if you have data to migrate, you need to use this method to configure the migration before executing any statements on current database.
 @note  If the source table is in the current database, you can set sourceDatabasePath to nil.
 @note  If the source table is not in the current database, the database containing the source table will be attached to the current database before the migration is complete. After migration, source tables will be dropped.
 @see   `WCTMigrationFilterBlock`
 */
- (void)addMigration:(nullable NSString*)sourceDatabasePath
          withFilter:(nullable WCDB_ESCAPE WCTMigrationFilterBlock)filter;

- (void)addMigration:(nullable NSString*)sourceDatabasePath
    withSourceCipher:(nullable NSData*)cipher
          withFilter:(nullable WCDB_ESCAPE WCTMigrationFilterBlock)filter;

/**
 @brief Manually spend about 0.01 sec. to migrate data. You can call this method periodically until all data is migrated.
 @return YES if no error occurred.
 */
- (BOOL)stepMigration;

/**
 @brief Configure the database to automatically step migration every two seconds.
 @param flag to enable auto-migration.
 */
- (void)enableAutoMigration:(BOOL)flag;

/**
 @brief Register a callback for migration notification. The callback will be called when each table completes the migration.
 @param onMigrated block
 @see   `WCTMigratedNotificationBlock`
 */
- (void)setNotificationWhenMigrated:(nullable WCDB_ESCAPE WCTMigratedNotificationBlock)onMigrated;

/**
 @brief Check if all tables in the database has finished migration.
 @note  It only check an internal flag of database.
 @return YES if all tables in the database has finished migration.
 */
- (BOOL)isMigrated;

@end

NS_ASSUME_NONNULL_END
