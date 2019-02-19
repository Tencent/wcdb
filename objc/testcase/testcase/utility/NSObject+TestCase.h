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
#import <WCDB/WCDB.h>
#import <WCDB/WCTDatabase+TestCase.h>

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

@end

@interface WCTPerformanceFootprint (TestCase)

@end

@interface NSFileManager (TestCase)

- (unsigned long long)getFileSizeIfExists:(NSString*)path;
- (BOOL)copyItemsIfExistsAtPaths:(NSArray<NSString*>*)paths toDirectory:(NSString*)directory error:(NSError**)error;
- (BOOL)removeItemIfExistsAtPath:(NSString*)path error:(NSError**)error;
- (BOOL)removeItemsIfExistsAtPaths:(NSArray<NSString*>*)paths error:(NSError**)error;
- (BOOL)setFileImmutable:(BOOL)immutable ofItemsIfExistsAtPaths:(NSArray<NSString*>*)paths error:(NSError**)error;
- (BOOL)isFileImmutableOfItemAtPath:(NSString*)path error:(NSError**)error;

- (BOOL)setFileProtectionOfPath:(NSString*)path to:(NSFileProtectionType)type error:(NSError**)error;
- (NSFileProtectionType)getFileProtection:(NSString*)path error:(NSError**)error;

@end
