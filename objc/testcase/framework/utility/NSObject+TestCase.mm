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

#import <TestCase/NSObject+TestCase.h>
#import <TestCase/TestCaseAssertion.h>
#import <TestCase/TestCaseLog.h>

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

@implementation NSNumber (TestCase)

+ (BOOL)value:(double)left almostEqual:(double)right
{
    // ulp == 10
    return std::abs(left - right) < std::numeric_limits<double>::epsilon() * std::abs(left + right) * 10
           || std::abs(left - right) < std::numeric_limits<double>::min();
}

- (BOOL)almostEqual:(NSNumber *)other
{
    if (other == nil) {
        return NO;
    }
    if (CFNumberIsFloatType((CFNumberRef) self)) {
        return [NSNumber value:self.doubleValue almostEqual:other.doubleValue];
    }
    return [self isEqualToNumber:other];
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

@end

@implementation WCTPerformanceFootprint (TestCase)

- (BOOL)isEqual:(NSObject *)object
{
    if (object.class != self.class) {
        return NO;
    }
    WCTPerformanceFootprint *other = (WCTPerformanceFootprint *) object;
    return self.frequency == other.frequency && [NSObject isObject:self.sql nilEqualToObject:other.sql];
}

@end

@implementation NSFileManager (TestCase)

- (unsigned long long)getFileSizeIfExists:(NSString *)path
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if ([fileManager fileExistsAtPath:path]) {
        return [[fileManager attributesOfItemAtPath:path error:nil] fileSize];
    }
    return 0;
}

- (BOOL)copyItemsIfExistsAtPaths:(NSArray<NSString *> *)paths toDirectory:(NSString *)directory error:(NSError **)error
{
    for (NSString *path in paths.reversedArray) {
        NSString *newPath = [NSString pathByReplacingPath:path withDirectory:directory];
        // remove existing file
        if (![self removeItemIfExistsAtPath:newPath error:error]) {
            return NO;
        }
        if ([self fileExistsAtPath:path]) {
            if (![self copyItemAtPath:path toPath:newPath error:error]) {
                return NO;
            }
        }
    }
    return YES;
}

- (BOOL)removeItemIfExistsAtPath:(NSString *)path error:(NSError **)error
{
    if ([self fileExistsAtPath:path]) {
        return [self removeItemAtPath:path error:error];
    }
    return YES;
}

- (BOOL)removeItemsIfExistsAtPaths:(NSArray<NSString *> *)paths error:(NSError **)error
{
    for (NSString *path in paths.reversedArray) {
        if (![self removeItemIfExistsAtPath:path error:error]) {
            return NO;
        }
    }
    return YES;
}

- (BOOL)setFileImmutable:(BOOL)immutable ofItemsIfExistsAtPaths:(NSArray<NSString *> *)paths error:(NSError **)error
{
    for (NSString *path in paths.reversedArray) {
        if ([self fileExistsAtPath:path]) {
            if (![self setAttributes:@{NSFileImmutable : @(immutable)} ofItemAtPath:path error:error]) {
                return NO;
            }
        }
    }
    return YES;
}

- (BOOL)isFileImmutableOfItemAtPath:(NSString *)path error:(NSError **)error
{
    if ([self fileExistsAtPath:path]) {
        return [self attributesOfItemAtPath:path error:error].fileIsImmutable;
    }
    return NO;
}

#if TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR
- (BOOL)setFileProtectionOfPath:(NSString *)path to:(NSFileProtectionType)type error:(NSError **)error
{
    return [self setAttributes:@{ NSFileProtectionKey : type } ofItemAtPath:path error:error];
}

- (NSFileProtectionType)getFileProtection:(NSString *)path error:(NSError **)error
{
    return [self attributesOfItemAtPath:path error:error][NSFileProtectionKey];
}
#endif

@end
