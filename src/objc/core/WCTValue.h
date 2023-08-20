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

#import "Macro.h"
#import "WCTDeclaration.h"

//It's tricky here. You can use `WCTValue` as any of `NSNumber`/`NSString`/`NSData`/`NSNull`, but it actually is a `NSProxy<WCTValueProtocol>` proxy.
//Ones have no need to understand this difference unless he/she needs to go deep into these fundamental classes.

NS_ASSUME_NONNULL_BEGIN

@protocol WCTValueProtocol

- (nullable NSString *)stringValue;

- (nullable NSNumber *)numberValue;

- (nullable NSData *)dataValue;

@end

WCDB_API @interface NSNumber(WCTValue)<WCTValueProtocol>

@end

WCDB_API @interface NSData(WCTValue)<WCTValueProtocol>

@end

WCDB_API @interface NSString(WCTValue)<WCTValueProtocol>

@end

WCDB_API @interface NSNull(WCTValue)<WCTValueProtocol>

@end

typedef NSObject<WCTValueProtocol> WCTValue;
typedef NSArray<WCTValue *> WCTOneRow;
typedef NSArray<WCTValue *> WCTOneColumn;
typedef NSArray<NSArray<WCTValue *> *> WCTColumnsXRows;

NS_ASSUME_NONNULL_END
