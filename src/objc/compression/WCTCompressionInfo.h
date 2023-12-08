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

#define WCTDictDefaultMatchValue ((uint64_t) -1)

WCDB_API @interface WCTCompressionBaseInfo : NSObject

- (instancetype)init UNAVAILABLE_ATTRIBUTE;

// Table of compression
@property (nonatomic, readonly) NSString *table;

@end

WCDB_API @interface WCTCompressionUserInfo : WCTCompressionBaseInfo

- (void)addZSTDNormalCompressProperty:(const WCTProperty &)property;

- (void)addZSTDDictCompressProperty:(const WCTProperty &)property
                         withDictId:(WCTDictId)dictId;

- (void)addZSTDDictCompressProperty:(const WCTProperty &)property
                  withMatchProperty:(const WCTProperty &)matchProperty
                      andMatchDicts:(NSDictionary<NSNumber *, NSNumber *> *)dictIds;

@end

NS_ASSUME_NONNULL_END
