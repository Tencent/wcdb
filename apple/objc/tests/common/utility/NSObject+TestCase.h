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

#import "WCTDatabase+TestCase.h"
#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

@interface NSObject (TestCase)

+ (BOOL)isObject:(NSObject*)left nilEqualToObject:(NSObject*)right;

@end

@interface NSArray (TestCase)

- (NSArray*)reversedArray;

@end

@interface NSNumber (TestCase)

+ (BOOL)value:(double)left almostEqual:(double)right;

- (BOOL)almostEqual:(NSNumber*)number;

@end

@interface NSString (TestCase)

+ (NSArray<NSString*>*)pathsByReplacingPaths:(NSArray<NSString*>*)paths withDirectory:(NSString*)directory;
+ (NSString*)pathByReplacingPath:(NSString*)path withDirectory:(NSString*)directory;
- (NSString*)test_stringByStandardizingPath;

@end

@interface NSFileManager (TestCase)

- (unsigned long long)getFileSizeIfExists:(NSString*)path;
- (void)copyItemsIfExistsAtPath:(NSString*)path toPath:(NSString*)destination;
- (void)removeItemIfExistsAtPath:(NSString*)path;
- (void)removeItemsIfExistsAtPaths:(NSArray<NSString*>*)paths;
- (void)setFileImmutable:(BOOL)immutable ofItemsIfExistsAtPath:(NSString*)path;
- (void)setFileImmutable:(BOOL)immutable ofItemsIfExistsAtPaths:(NSArray<NSString*>*)paths;
- (BOOL)isFileImmutableOfItemAtPath:(NSString*)path;

#if TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR
- (void)setFileProtectionOfPath:(NSString*)path to:(NSFileProtectionType)type;
- (NSFileProtectionType)getFileProtection:(NSString*)path;
#endif

@end
