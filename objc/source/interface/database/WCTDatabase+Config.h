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

/**
 Configuration
 */
typedef BOOL (^WCTConfigCallback)(WCTUnsafeHandle *);

@interface WCTDatabase (Config)

/**
 @brief This interface is equivalent to [database setCipherKey:cipherKey andCipherPageSize:4096];
 @param cipherKey Cipher key.
 */
- (void)setCipherKey:(NSData *)cipherKey;

/**
 @brief Set cipher key for a database. 
 For an encrypted database, you must call it before all other operation.
 The cipher page size defaults to 4096 in WCDB, but it defaults to 1024 in other databases. So for an existing database created by other database framework, you should set it to 1024. Otherwise, you'd better to use cipher page size with 4096 or simply call setCipherKey: interface to get better performance.
 @param cipherKey Cipher key.
 @param cipherPageSize Cipher Page Size
 */
- (void)setCipherKey:(NSData *)cipherKey
    andCipherPageSize:(int)cipherPageSize;

/**
 @brief Set config for this database.  
 @warning Since WCDB is a multi-handle database, an executing handle will not apply this config immediately. Instead, all handles will run this config before its next operation.  
 
 [database setConfig:^BOOL(std::shared_ptr<WCDB::Handle> &handle, WCDB::Error& error) {
 return handle->execute(WCDB::StatementPragma().pragma(WCDB::Pragma::SecureDelete, YES));
 } forName:@"demo" withOrder:1];
 
 @param config config
 @param name The Identifier for this config
 @param order The smaller number is called first
 */
- (void)setConfig:(WCTConfigCallback)config
          forName:(NSString *)name
        withOrder:(int)order;

/**
 @brief This interface is equivalent to [database setConfig:config forName:name withOrder:INT_MAX];
 @see setConfig:forName:withOrder:
 @param config config
 @param name The Identifier for this config
 */
- (void)setConfig:(WCTConfigCallback)config
          forName:(NSString *)name;

@end
