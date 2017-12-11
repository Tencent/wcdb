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
#import <WCDB/WCTDatabase.h>
#import <WCDB/WCTDeclare.h>

/**
 Close block
 */
typedef void (^WCTCloseBlock)(void);

@interface WCTDatabase (Database)

/**
 @brief Init a database from path.
        Note that all database objects with same path share the same core. So you can create multiple database objects. WCDB will manage them automatically.
        Note that WCDB will not generate a sqlite handle until the first operation, which is also called as lazy initialization.
 @param path Path to your database
 @return WCTDatabase
 */
- (instancetype)initWithPath:(NSString *)path;

/**
 @brief Init a database from existing tag.
        Note that all database objects with same path share the same core. So you can create multiple database objects. WCDB will manage them automatically.
        Note that WCDB will not generate a sqlite handle until the first operation, which is also called as lazy initialization.
 @param tag a tag already exists. Note that 0 is not a valid tag.
 @return nil if the tag do not exists
 */
- (instancetype)initWithExistingTag:(WCTTag)tag;

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
- (void)setCipherKey:(NSData *)cipherKey andCipherPageSize:(int)cipherPageSize;

/**
 @brief Set the tag of the database. 
        Note that WCTCore objects with same path share this tag, even they are not the same object.
 @param tag Default to 0. 
 */
- (void)setTag:(WCTTag)tag;

/**
 @brief Since WCDB is using lazy initialization, [initWithPath:] never return nil even the database can't open. So you can call this to check whether the database can be opened. 
 @return NO if an error occurs during sqlite handle initialization.
 */
- (BOOL)canOpen;

/**
 @brief Check database is already opened.
 @return isOpened
 */
- (BOOL)isOpened;

/**
 @brief close the database.
        Since Multi-threaded operation is supported in WCDB, other operations in different thread can open the closed database. So this method can make sure database is closed in the "onClosed" block. All other operations will be blocked until this method returns.
        A close operation consists of 4 steps:
            1. blockade, which blocks all other operations.
            2. close, which waits until all sqlite handles return and closes them.
            3. onClosed, which trigger the callback.
            4. unblokade, which unblocks all other opreations.
        You can simply call close: to do all steps above or call these separately.
        Since this method will wait until all sqlite handles return, it may lead to deadlock in some bad practice. The key to avoid deadlock is to make sure all WCDB objects in current thread is dealloced. In detail:
            1. You should not keep WCDB objects, including WCTInsert, WCTDelete, WCTUpdate, WCTSelect, WCTRowSelect, WCTMultiSelect, WCTStatement, WCTTransaction. These objects should not be kept. You should get them, use them, then release them(set to nil) right away.
            2. WCDB objects may not be out of its' scope.
            3. Further more, those WCDB objects may be kept by NSAutoReleasePool, which is done by ARC automatically. So you should make sure that all WCDB objects in NSAutoReleasePool is drained. 
            The best practice is to call close: in sub-thread and display a loading animation in main thread.
 
     //close directly
     [database close:^(){
        //do something on this closed database
     }];
     //close separately
     [database blockade];
     [database close];
     //do something on this closed database
     [database unblockade];

 @param onClosed Trigger on database closed.
 */
- (void)close:(WCTCloseBlock)onClosed;

/**
 @brief This interface is equivalent to [database close:nil].
 @see close:
 */
- (void)close;

/**
 @brief Blockade the database.
 @see close:
 */
- (void)blockade;

/**
 @brief Unblockade the database.
 @see close:
 */
- (void)unblockade;

/**
 @brief Check whether database is blockaded.
 @see close:
 @return isBlockaded
 */
- (BOOL)isBlockaded;

/**
 @brief Purge all free handles of this database.
        WCDB will cache and reuse some sqlite handles to improve performance. 
        The max count of free sqlite handles is same as the number of concurrent threads supported by the hardware implementation.
        You can call it to save some memory.
 */
- (void)purgeFreeHandles;

/**
 @brief Purge all free handles of all databases.
        Note that WCDB will call this interface automatically while it receives memory warning on iOS.
 @see purgeFreeHandles
 */
+ (void)PurgeFreeHandlesInAllDatabases;

@end
