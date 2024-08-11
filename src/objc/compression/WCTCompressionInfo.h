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

#import "WCTCommon.h"

NS_ASSUME_NONNULL_BEGIN

typedef unsigned char WCTDictId;

#define WCTDictDefaultMatchValue INT64_MAX

WCDB_API @interface WCTCompressionBaseInfo : NSObject

- (instancetype)init UNAVAILABLE_ATTRIBUTE;

// The table to be compressed
@property (nonatomic, readonly) NSString *table;

@end

WCDB_API @interface WCTCompressionUserInfo : WCTCompressionBaseInfo

/**
 @brief Configure to compress all data in the specified column with the default zstd compression algorithm.
 */
- (void)addZSTDNormalCompressProperty:(const WCTProperty &)property;

/**
 @brief Configure to compress all data in the specified column with a registered zstd dict.
 */
- (void)addZSTDDictCompressProperty:(const WCTProperty &)property
                         withDictId:(WCTDictId)dictId;

/**
 @brief Configure to compress all data in the specified column with multi registered zstd dict.
 Which dict to use when compressing is based on the value of the specified matching column.
 @note You can use `WCTDictDefaultMatchValue` to specify a default dict.
 @warning The matching column must be an integer column.
 */
- (void)addZSTDDictCompressProperty:(const WCTProperty &)property
                  withMatchProperty:(const WCTProperty &)matchProperty
                      andMatchDicts:(NSDictionary<NSNumber * /* Value of match column */, NSNumber * /* ID of dict */> *)dictIds;

/**
 @brief Enable to replace original compression format.
 After activation, you can use `-[WCTDatabase stepCompression]` or `-[WCTDatabase enableAutoCompression:]` to recompress the existing data with the new compression configuration.
 */
- (void)enableReplaceCompression;
@end

NS_ASSUME_NONNULL_END
