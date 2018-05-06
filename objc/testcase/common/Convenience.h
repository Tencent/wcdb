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

#pragma mark - NSArray
@interface NSArray (Reverse)
- (NSArray *)reversed;
- (NSArray *)sorted;
- (BOOL)isEqualToObjects:(NSArray *)objects
          withComparator:(BOOL (^)(id, id))comparator;
@end

#pragma mark - NSObject
@interface NSObject (Comparator)
+ (NSComparator)Comparator;
@end

#pragma mark - NSMutableArray
@interface NSMutableArray (Reverse)
- (NSMutableArray *)reversed;
- (NSMutableArray *)sorted;
@end

#pragma mark - NSData
@interface NSData (Random)
+ (NSData *)randomData;
+ (NSData *)randomDataWithLength:(NSUInteger)length;
+ (NSData *)randomDataOtherThan:(NSData *)other;
@end

#pragma mark - NSString
@interface NSString (Random)
+ (NSString *)randomString;
@end
