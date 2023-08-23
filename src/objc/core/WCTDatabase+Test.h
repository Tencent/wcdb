//
// Created by sanhuazhang on 2019/06/03
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

#import "WCDBObjc.h"

typedef NS_OPTIONS(NSUInteger, WCTSimulateIOErrorOptions) {
    WCTSimulateNoneIOError = 0,
    WCTSimulateReadIOError = 1 << 0,
    WCTSimulateWriteIOError = 1 << 1,
};

NS_ASSUME_NONNULL_BEGIN

WCDB_API @interface WCTDatabase(Test)

// Only for test
+ (void)simulateIOError:(WCTSimulateIOErrorOptions)options;

// Only for test
- (void)enableAutoCheckpoint:(BOOL)flag;

/**
 @brief Checkpoint the current database with truncate mode.
 */
- (BOOL)truncateCheckpoint;

/**
 @brief Checkpoint the current database with passive mode.
 */
- (BOOL)passiveCheckpoint;

/**
 @brief Check database is already opened.
 @return isOpened
 */
- (BOOL)isOpened;

/**
 @brief Since WCDB is using lazy initialization, [initWithPath:] never return nil even the database can't open. So you can call this to check whether the database can be opened.
 @return NO if an error occurs during sqlite db handle initialization.
 */
- (BOOL)canOpen;

/**
 @brief This interface is equivalent to -[WCTDatabase close:nil].
 @see `-[WCTDatabase close:]`
 */
- (void)close;

/**
 @brief Check whether database is blockaded.
 @see `-[WCTDatabase close:]`
 @return isBlockaded
 */
- (BOOL)isBlockaded;

/**
 @brief Blockade the database.
 @see `-[WCTDatabase close:]`
 */
- (void)blockade;

/**
 @brief Unblockade the database.
 @see `-[WCTDatabase close:]`
 */
- (void)unblockade;

@end

NS_ASSUME_NONNULL_END
