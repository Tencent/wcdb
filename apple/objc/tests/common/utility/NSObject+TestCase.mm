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

#import "NSObject+TestCase.h"
#import "TestCaseAssertion.h"
#import "TestCaseLog.h"

@implementation NSObject (TestCase)

+ (BOOL)isObject:(NSObject *)left nilEqualToObject:(NSObject *)right
{
    return (left == nil && right == nil) || [left isEqual:right];
}

@end

@implementation NSArray (TestCase)

- (NSArray *)reversedArray
{
    return self.reverseObjectEnumerator.allObjects;
}

@end

@implementation NSString (TestCase)

+ (NSString *)pathByReplacingPath:(NSString *)path withDirectory:(NSString *)directory
{
    return [directory stringByAppendingPathComponent:path.lastPathComponent];
}

+ (NSArray<NSString *> *)pathsByReplacingPaths:(NSArray<NSString *> *)paths withDirectory:(NSString *)directory
{
    NSMutableArray<NSString *> *newPaths = [NSMutableArray arrayWithCapacity:paths.count];
    for (NSString *path in paths) {
        [newPaths addObject:[NSString pathByReplacingPath:path withDirectory:directory]];
    }
    return newPaths;
}

- (NSString *)test_stringByStandardizingPath
{
    NSString *path = [self stringByStandardizingPath];
#if TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR
    /*
     /var is the symlink to /private/var.
     In most cases, -[NSString stringByStandardizingPath] will return the path with the /var prefix,
     while in a small number of cases it will return the path with the /private/var prefix.
     In order to avoid the inconsistency of the path of the same file, remove the /private prefix of path here
     */
    NSRange match = [path rangeOfString:@"/private"];
    if (match.location == 0) {
        path = [path stringByReplacingCharactersInRange:match withString:@""];
    }
#endif
    return path;
}

+ (NSString *)createPreciseStringFromDouble:(double)doubleValue
{
    std::ostringstream stream;
    stream.precision(std::numeric_limits<double>::max_digits10);
    stream << doubleValue;
    return [NSString stringWithUTF8String:stream.str().data()];
}

@end

@implementation NSFileManager (TestCase)

- (unsigned long long)getFileSizeIfExists:(NSString *)path
{
    unsigned long long size = 0;
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if ([fileManager fileExistsAtPath:path]) {
        NSError *error;
        size = [[fileManager attributesOfItemAtPath:path error:&error] fileSize];
        TestCaseAssertNil(error);
    }
    return size;
}

- (void)copyItemsIfExistsAtPath:(NSString *)path toPath:(NSString *)destination
{
    [self removeItemIfExistsAtPath:destination];
    if ([self fileExistsAtPath:path]) {
        NSError *error;
        [self copyItemAtPath:path toPath:destination error:&error];
        TestCaseAssertNil(error);
    }
}

- (void)copyItemsIfExistsAtPaths:(NSArray<NSString *> *)paths toDirectory:(NSString *)directory
{
    for (NSString *path in paths.reversedArray) {
        NSString *newPath = [NSString pathByReplacingPath:path withDirectory:directory];
        // remove existing file
        [self removeItemIfExistsAtPath:newPath];
        if ([self fileExistsAtPath:path]) {
            NSError *error;
            [self copyItemAtPath:path toPath:newPath error:&error];
            TestCaseAssertNil(error);
        }
    }
}

- (void)removeItemIfExistsAtPath:(NSString *)path
{
    if ([self fileExistsAtPath:path]) {
        NSError *error;
        [self removeItemAtPath:path error:&error];
        TestCaseAssertNil(error);
    }
}

- (void)removeItemsIfExistsAtPaths:(NSArray<NSString *> *)paths
{
    for (NSString *path in paths.reversedArray) {
        [self removeItemIfExistsAtPath:path];
    }
}

- (void)setFileImmutable:(BOOL)immutable ofItemsIfExistsAtPath:(NSString *)path
{
    if ([self fileExistsAtPath:path]) {
        NSError *error;
        TestCaseAssertTrue([self setAttributes:@{ NSFileImmutable : @(immutable) } ofItemAtPath:path error:&error]);
        TestCaseAssertNil(error);
    }
}

- (void)setFileImmutable:(BOOL)immutable ofItemsIfExistsAtPaths:(NSArray<NSString *> *)paths
{
    for (NSString *path in paths.reversedArray) {
        [self setFileImmutable:immutable ofItemsIfExistsAtPath:path];
    }
}

- (BOOL)isFileImmutableOfItemAtPath:(NSString *)path
{
    BOOL immutable = NO;
    if ([self fileExistsAtPath:path]) {
        NSError *error;
        immutable = [self attributesOfItemAtPath:path error:&error].fileIsImmutable;
        TestCaseAssertNil(error);
    }
    return immutable;
}

#if TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR
- (void)setFileProtectionOfPath:(NSString *)path to:(NSFileProtectionType)type
{
    if ([self fileExistsAtPath:path]) {
        NSError *error;
        TestCaseAssertTrue([self setAttributes:@{ NSFileProtectionKey : type } ofItemAtPath:path error:&error]);
        TestCaseAssertNil(error);
    }
}

- (NSFileProtectionType)getFileProtection:(NSString *)path
{
    NSError *error;
    NSFileProtectionType type = [self attributesOfItemAtPath:path error:&error][NSFileProtectionKey];
    TestCaseAssertNil(error);
    return type;
}
#endif

@end
