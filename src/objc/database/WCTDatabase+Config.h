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

#import "WCTCommon.h"
#import "WCTDatabase.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(int, WCTConfigPriority) {
    WCTConfigPriorityHighest = INT32_MIN, // Only For cipher config
    WCTConfigPriorityHigh = -100,
    WCTConfigPriorityDefault = 0,
    WCTConfigPriorityLow = 100,
};

/**
 Configuration
 */
typedef BOOL (^WCTConfigBlock)(WCTHandle* _Nonnull);

@interface WCTDatabase (Config)

/**
 @brief This interface is equivalent to `-[WCTDatabase setCipherKey:cipherKey andCipherPageSize:4096]`;
 @param cipherKey Cipher key.
 */
- (void)setCipherKey:(NSData* _Nullable)cipherKey;

/**
 @brief Set cipher key for a database. 
 For an encrypted database, you must call it before all other operation.
 The cipher page size defaults to 4096 in WCDB, but it defaults to 1024 in other databases. So for an existing database created by other database framework, you should set it to 1024. Otherwise, you'd better to use cipher page size with 4096 or simply call setCipherKey: interface to get better performance.
 @param cipherKey Cipher key.
 @param cipherPageSize Cipher Page Size
 */
- (void)setCipherKey:(NSData* _Nullable)cipherKey
   andCipherPageSize:(int)cipherPageSize;

/**
 @brief Set config for this database.  
 @warning Since WCDB is a multi-handle database, an executing handle will not apply this config immediately. Instead, all handles will run this config before its next operation.
 @warning If you want to add cipher config, please use `WCTConfigPriorityHighest`.
 
     [database setConfig:^BOOL(WCTHandle* handle) {
        return [handle execute: WCDB::StatementPragma().pragma(WCDB::Pragma::secureDelete()).to(true)];
     } withUninvocation:nil forName:@"demo" withPriority:WCTConfigPriorityDefault];
 */
- (void)setConfig:(WCDB_ESCAPE WCTConfigBlock)invocation
 withUninvocation:(nullable WCDB_ESCAPE WCTConfigBlock)uninvocation
          forName:(NSString*)name
     withPriority:(WCTConfigPriority)priority;

/**
 @brief This interface is equivalent to `-[WCTDatabase setConfig:config withUninvocation:uninvocation forName:name withPriority:WCTConfigPriorityDefault]`;
 */
- (void)setConfig:(WCDB_ESCAPE WCTConfigBlock)invocation
 withUninvocation:(nullable WCDB_ESCAPE WCTConfigBlock)uninvocation
          forName:(NSString*)name;

/**
 @brief Remove a config.
 */
- (void)removeConfigForName:(NSString*)name;

/**
  @brief These three interfaces are used to access and modify the global ABtest configuration of WCDB.
 */
+ (void)setABTestConfigWithName:(NSString*)name
                       andValue:(NSString*)value;

+ (void)removeABTestConfigWithName:(NSString*)name;

+ (NSString*)getABTestConfigWithName:(NSString*)name;

@end

NS_ASSUME_NONNULL_END
