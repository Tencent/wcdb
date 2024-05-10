//
// Created by qiuwenchen on 2023/12/2.
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

#import "WCTCompressionInfo.h"
#import "WCTDatabase.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Triggered at any time when WCDB needs to know whether a table in the current database needs to compress data,
 mainly including creating a new table, reading and writing a table,and starting to compress a new table.
 If the current table does not need to compress data, you don't need to config `WCTCompressionUserInfo`.
 */
typedef void (^WCTCompressionFilterBlock)(WCTCompressionUserInfo*);

typedef unsigned char WCTDictId;

/**
 Triggered when a table is compressed completely.
 When a table is compressed successfully, tableInfo will carry the information of the table.
 When a database is totally compressed, tableInfo will be nil.
 */
typedef void (^WCTCompressdNotificationBlock)(WCTDatabase* _Nonnull database, WCTCompressionBaseInfo* _Nullable tableInfo);

WCDB_API @interface WCTDatabase(Compression)

/**
 @brief Train a zstd formalized dict with a set of sample strings.
 @Warning The total size of all samples cannot exceed 4G.
 @param strings samples.
 @param dictId spercified id of the result dict. It can not be zero.
 @return a dict of 100KB if succeed.
 */
+ (NSData*)trainDictWithStrings:(NSArray<NSString*>*)strings andDictId:(WCTDictId)dictId;

/**
 @brief Train a zstd formalized dict with a set of sample datas.
 @Warning The total size of all samples cannot exceed 4G.
 @param datas samples.
 @param dictId spercified id of the result dict. It can not be zero.
 @return a dict of 100KB if succeed.
 */
+ (NSData*)trainDictWithDatas:(NSArray<NSData*>*)datas andDictId:(WCTDictId)dictId;

/**
 @brief Register a zstd dict in to WCDB.
 @Note You must register a dict before using it.
 @param dict dict data.
 @param dictId id of the dict. It can not be zero.
 @return YES if the dictionary can be successfully decoded and the dictId does not conflict with a registered dict.
 */
+ (BOOL)registerZSTDDict:(NSData*)dict andDictId:(WCTDictId)dictId;

/**
 @brief Configure which tables in the current database need to compress data.
 Once configured, newly written data will be compressed immediately and synchronously,
 and you can use `-[WCTDatabase stepCompression]` and `-[WCTDatabase enableAutoCompression:]` to compress existing data.
 @see   `WCTCompressionFilterBlock`
 */
- (void)setCompressionWithFilter:(nullable WCDB_ESCAPE WCTCompressionFilterBlock)filter;

/**
 @brief Configure not to compress new data written to the current database.
 This configuration is mainly used to deal with some emergency scenarios.
 It allows already compressed data to be read normally, but new data is no longer compressed.
 @param disable disable compression or not.
 */
- (void)disableCompresssNewData:(BOOL)disable;

/**
 @brief Manually compress 100 rows of existing data.
 You can call this method periodically until all data is compressed.
 @return YES if no error occurred.
 */
- (BOOL)stepCompression;

/**
 @brief Configure the database to automatically compress 100 rows of existing data every two seconds.
 @param flag to enable auto-compression.
 */
- (void)enableAutoCompression:(BOOL)flag;

/**
 @brief Register a callback for compression notification. The callback will be called when each table completes the compression.
 @param onCompressd block
 @see   `WCTCompressdNotificationBlock`
 */
- (void)setNotificationWhenCompressed:(nullable WCDB_ESCAPE WCTCompressdNotificationBlock)onCompressd;

/**
 @brief Check if all tables in the database has finished compression.
 @note  It only check an internal flag of database.
 @return YES if all tables in the database has finished compression.
 */
- (BOOL)isCompressed;

/**
 @brief Decompress all compressed data in the database and resave them.
 @note  It will clear all compression status and progress, and disables automatic compression.
 @warning If the rollback process is interrupted or failed, the data may be in a mixed state of compressed and uncompressed.
 @return YES if all operation succeed.
 */
- (BOOL)rollbackCompression:(nullable WCDB_ESCAPE WCTProgressUpdateBlock)onProgressUpdated;

@end

NS_ASSUME_NONNULL_END
