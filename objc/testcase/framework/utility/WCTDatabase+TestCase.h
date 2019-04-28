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

#import <WCDB/WCDB.h>

typedef NS_OPTIONS(NSUInteger, WCTSimulateIOErrorOptions) {
    WCTSimulateNoneIOError = 0,
    WCTSimulateReadIOError = 1 << 0,
    WCTSimulateWriteIOError = 1 << 1,
};

NS_ASSUME_NONNULL_BEGIN

@interface WCTDatabase (TestCase)

@property (nonatomic, readonly) NSString* walPath;
@property (nonatomic, readonly) NSString* factoryPath;
@property (nonatomic, readonly) NSString* firstMaterialPath;
@property (nonatomic, readonly) NSString* lastMaterialPath;
@property (nonatomic, readonly) NSString* journalPath;
@property (nonatomic, readonly) NSString* shmPath;
@property (nonatomic, readonly) NSString* factoryRestorePath;

@property (nonatomic, readonly) int headerSize;
@property (nonatomic, readonly) int pageSize;
@property (nonatomic, readonly) int walHeaderSize;
@property (nonatomic, readonly) int walFrameHeaderSize;
@property (nonatomic, readonly) int walFrameSize;

+ (void)simulateIOError:(WCTSimulateIOErrorOptions)options;

+ (void)resetGlobalErrorTracer;

- (void)removeCheckpointConfig;

- (WCTOptionalSize)getNumberOfWalFrames;

@end

NS_ASSUME_NONNULL_END
