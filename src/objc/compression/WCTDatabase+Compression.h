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
 If the current table does not need to compress data, you don't need to config WCTCompressionUserInfo.
 */
typedef void (^WCTCompressionFilterBlock)(WCTCompressionUserInfo*);

typedef unsigned char WCTDictId;

/**
 Triggered when a table is compressed completely.
 When a table is compressed successfully, tableInfo will carry the information of the table.
 When a database is totally compressed, tableInfo will .
 */
typedef void (^WCTCompressdNotificationBlock)(WCTDatabase* _Nonnull database, WCTCompressionBaseInfo* _Nullable tableInfo);

WCDB_API @interface WCTDatabase(Compression)

+ (BOOL)registerZSTDDict:(NSData*)dict;

+ (NSData*)trainDictWithStrings:(NSArray<NSString*>*)strings andDictId:(WCTDictId)dictId;

+ (NSData*)trainDictWithDatas:(NSArray<NSData*>*)datas andDictId:(WCTDictId)dictId;

/**
 @brief Configure which tables in the current database need to compress data.
 Once configured, newly written data will be compressed immediately and synchronously,
 and you can use `-[WCTDatabase stepCompression]` and `-[WCTDatabase enableAutoCompression:]` to compress existing data.
 @warning  You need to use this method to configure the compression before excuting any statements on current database.
 @see   `WCTCompressionFilterBlock`
 */
- (void)setCompressionWithFilter:(nullable WCDB_ESCAPE WCTCompressionFilterBlock)filter;

/**
 @brief Manually compress 100 rows of data. You can call this method periodically until all data is compressed.
 @return YES if no error occurred.
 */
- (BOOL)stepCompression;

/**
 @brief Configure the database to automatically step compression every two seconds.
 @param flag to enable auto-compression.
 */
- (void)enableAutoCompression:(BOOL)flag;

/**
 @brief Register a callback for compression notification. The callback will be called when each table completes the compression.
 @param onCompressd block
 @see   `WCTCompressdNotificationBlock`
 */
- (void)setNotificationWhenCompressd:(nullable WCDB_ESCAPE WCTCompressdNotificationBlock)onCompressd;

/**
 @brief Check if all tables in the database has finished compression.
 @note  It only check an internal flag of database.
 @return YES if all tables in the database has finished compression.
 */
- (BOOL)isCompressed;

@end

NS_ASSUME_NONNULL_END
