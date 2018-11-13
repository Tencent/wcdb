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

NS_ASSUME_NONNULL_BEGIN

@interface NSNumber (TestCase)

+ (NSInteger)randomInt64;

+ (int)randomInt32;

+ (uint8_t)randomUInt8;

+ (double)randomDouble;

+ (NSNumber*)randomNumber;

@end

@interface NSString (TestCase)

+ (NSString*)randomString;

@end

@interface NSData (TestCase)

+ (NSData*)randomData;

@end

@interface NSArray (TestCase)

- (NSArray*)reversedArray;

@end

NS_ASSUME_NONNULL_END
