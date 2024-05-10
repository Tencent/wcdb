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

NS_ASSUME_NONNULL_BEGIN

/**
 Triggered when a database is confirmed to be corrupted.
 */
typedef void (^WCTCorruptedNotificationBlock)(WCTDatabase *);

/**
 Triggered before backup of each table. Return NO to skip backup of this table.
 */
typedef BOOL (^WCTBackupFilterBlock)(NSString * /* table */);

WCDB_API @interface WCTDatabase(Repair)

/**
 @brief Register database corruption notification callback.
 If the current database reports an error of `SQLITE_CORRUPT` or `SQLITE_NOTADB` during operation, WCDB will asynchronously use `PRAGMA integrity_check` to check whether this database is truly corrupted. Once confirmed, WCDB will notify you through the callback registered by this method. In the callback, you can delete the corrupted database or try to repair the database.
 @see   `-[WCTDatabase removeFiles]`
 @see   `-[WCTDatabase retrieve:]`
 @param onCorrupted The callback for database corruption.
 */
- (void)setNotificationWhenCorrupted:(nullable WCDB_ESCAPE WCTCorruptedNotificationBlock)onCorrupted;

/**
 @brief Check if the current database is corrupted.
 @warning Since this method will check all the contents of the database until it finds something corrupted, it may take a lot of time to come to a conclusion.
 @return YES if the current database is corrupted.
 */
- (BOOL)checkIfCorrupted;

/**
 @brief Check if the current database is already observed corrupted by WCDB.
 @note  It only checks a internal flag of current database.
 @return YES if the current database is already corrupted.
 */
- (BOOL)isAlreadyCorrupted;

/**
 @brief Enable database to automatically backup itself after there are updates.
 The backup content mainly includes the SQL statements related to table creation and all leaf page numbers of each table in database. The backup content will be saved in the same directory of the database. And the backup file name is the database file name plus "-first.material" and "-last.material" suffixes.
 @note  Auto-backup do not degrade IO performance of the database.
 @param flag to enable auto-backup.
 */
- (void)enableAutoBackup:(BOOL)flag;

/**
 @brief Back up the database manually.
 @see   `-[WCTDatabase enableAutoBackup:]`
 @return YES if the database is backuped successfully.
 */
- (BOOL)backup;

/**
 @brief Set a filter to tell which table should be backed up.
 The filter will be called for every table in database. If there are some tables that do not need to be backed up, return false when these table names are passed into the filter.
 @param tableShouldBeBackedUp block.
 @see `WCTBackupFilterBlock`
 */
- (void)filterBackup:(nullable WCDB_ESCAPE WCTBackupFilterBlock)tableShouldBeBackedUp;

/**
 @brief Move the current database to a temporary directory and create a new database at current path.
 This method is designed for conditions where the database is corrupted and cannot be repaired temporarily. It can deposit the corrupted database to a new location and create a temporary database for the user. The contents of the deposited database will be restored when you recover the database.
 @note  This method can be called multiple times without loss of data.
 @return YES if new database is successfully created.
 */
- (BOOL)deposit;

/**
 @brief Remove deposited data.
 @see   `-[WCTDatabase deposit]`
 @return YES if all deposited data is removed.
 */
- (BOOL)removeDeposited;

/**
 @brief Check whether there is deposited data.
 @see   `-[WCTDatabase deposit]`
 @return YES if deposited data exists.
 */
- (BOOL)containsDeposited;

/**
 @brief Recover data from a corruped db.
 If there is a valid backup of this database, most of the uncorrupted data can be recovered, otherwise WCDB will try to read all the data that can still be accessed, starting from the first page of the database.
 In the extreme case where this database is not backed up and the first page is corrupted, no data can be recovered.
 @warning Since repairing a database requires reading all uncorrupted data and writing them to a new database, it may take a lot of time to finish the job. During this period, you'd better display a loading view on the screen and present the processing percentage.
 @param onProgressUpdated block.
 @see   `WCTProgressUpdateBlock`.
 @return Percentage of repaired data. 0 or less then 0 means data recovery failed. 1 means data is fully recovered.
 */
- (double)retrieve:(nullable WCDB_NO_ESCAPE WCTProgressUpdateBlock)onProgressUpdated;

@end

NS_ASSUME_NONNULL_END
