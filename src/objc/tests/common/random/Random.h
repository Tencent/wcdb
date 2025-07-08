//
// Created by sanhuazhang on 2019/07/05
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

#import <Foundation/Foundation.h>

typedef enum : NSUInteger {
    RandomStringType_Default = 0,
    RandomStringType_English,
    RandomStringType_Chinese,
} RandomStringType;

@interface Random : NSObject

+ (instancetype)shared;

- (void)setStable:(BOOL)stable;
- (void)setStringType:(RandomStringType)type;

- (void)reset;

- (uint64_t)uint64;
- (uint32_t)uint32;
- (uint16_t)uint16;
- (uint8_t)uint8;

- (int64_t)int64;
- (int32_t)int32;
- (int16_t)int16;
- (int8_t)int8;

- (double)double_;
- (float)float_;
- (float)float_0_1;

- (BOOL)boolean;

- (NSNumber *)number;

- (NSString *)string;
- (NSString *)stringWithLength:(UInt32)length;
- (NSString *)chineseString;
- (NSString *)chineseStringWithLength:(UInt32)length;
- (NSString *)englishString;
- (NSString *)englishStringWithLength:(UInt32)length;

- (NSData *)data;
- (NSData *)dataWithLength:(NSInteger)length;
- (NSData *)dataOtherThan:(NSData *)other;

- (NSDate *)date;

@end
