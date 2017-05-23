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

#import <Foundation/Foundation.h>
#import <WCDB/WCTDataBase.h>
#import <WCDB/WCTDeclare.h>

typedef void (^WCTCloseBlock)(void);

/**
 Thread-safe
 */
@interface WCTDataBase(DataBase)

/**
 Init a database from path.
 Note that all database objects with same path share the same core. So you can create multiple database objects. WCDB will manage them automatically.
 Note that WCDB will not generate a sqlite handle until the first operation, which is also called as lazy initialization.

 @param path Path to your database
 @return WCTDataBase
 */
- (instancetype)initWithPath:(NSString*)path;

/**
 Set cipher key for a database. 
 For an unencrypted database, you do not need to call it.
 For an encrypted database, you must call it before all other operation.

 @param data Cipher key.
 */
- (void)setCipherKey:(NSData*)data;

/**
 Set the tag of the database. 
 Note that [WCTCore] objects with same path share this tag, even they are not the same object.

 @param tag Default to 0. 
 */
- (void)setTag:(WCDB::Tag)tag;

/**
 Since WCDB is using lazy initialization, [initWithPath:] never return nil even the database can't open.
 So you can call this to check whether the database can be opened. 

 @return NO if an error occurs during sqlite handle initialization.
 */
- (BOOL)canOpen;

/**
 Check database is already opened.

 @return isOpened
 */
- (BOOL)isOpened;

/**
 Since Multi-threaded operation is supported in WCDB, other operations in different thread can open the closed database.
 So this [close:] can make sure database is closed in the [onClosed] block. All other operations will be blocked until [close] returns.
 
 A close operation consists of 4 steps:
 1. [blockade], which blocks all other operations.
 2. [close], which closes all sqlite handles.
 3. [onClosed], which trigger the callback.
 4. [unblokade], which unblocks all other opreations.
 
 You can simply call [close:] to do all steps above or call these separately.
 [database blockade];
 [database close];
 //...
 [database unblockade];

 @param onClosed Trigger on database closed.
 */
- (void)close:(WCTCloseBlock)onClosed;

/**
 This interface is equivalent to [database close:nil].
 */
- (void)close;

/**
 Blockade the database. See [close:] also.
 */
- (void)blockade;

/**
 Unblockade the database. See [close:] also.
 */
- (void)unblockade;

/**
 Check whether database is blockaded. See [close:] also.

 @return isBlockaded
 */
- (BOOL)isBlockaded;

/**
 Purge all free handles of this database.
 WCDB will cache and reuse some sqlite handles to improve performance. 
 The max count of free sqlite handles is same as the number of concurrent threads supported by the hardware implementation.
 You can call it to save some memory.
 */
- (void)purgeFreeHandles;

/**
 Purge all free handles of all databases. See [purgeFreeHandles] also.
 Note that WCDB will call this interface automatically while it receives memory warning on iOS.
 */
+ (void)PurgeFreeHandlesInAllDataBases;

@end
