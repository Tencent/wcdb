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
#import <TestCase/TestCaseLog.h>
#import <WCDB/SQLite.h>

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
    // ulp == 3
    return std::abs(left - right) < std::numeric_limits<double>::epsilon() * std::abs(left + right) * 3
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

@implementation WCTPerformanceFootprint (TestCase)

- (BOOL)isEqual:(NSObject *)object
{
    if (object.class != self.class) {
        return NO;
    }
    WCTPerformanceFootprint *other = (WCTPerformanceFootprint *) object;
    if (self.frequency != other.frequency) {
        return NO;
    }
    if (self.sql != nil) {
        return [other.sql isEqualToString:self.sql];
    } else {
        return other.sql == nil;
    }
}

@end

@implementation NSFileManager (TestCase)

- (unsigned long long)getFileSize:(NSString *)path
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if ([fileManager fileExistsAtPath:path]) {
        return [[fileManager attributesOfItemAtPath:path error:nil] fileSize];
    }
    return 0;
}

@end

ssize_t illPwrite(int, const void *, size_t, off_t)
{
    return -1;
}

@implementation WCTDatabase (TestCase)

+ (void)enableSQLiteWrite
{
    NSLog(@"Will enable SQLite write");
    sqlite3_vfs *vfs = sqlite3_vfs_find(nullptr);
    vfs->xSetSystemCall(vfs, "pwrite", (sqlite3_syscall_ptr) pwrite);
    NSLog(@"Did enable SQLite write");
}

+ (void)disableSQLiteWrite
{
    NSLog(@"Will disable SQLite write");
    sqlite3_vfs *vfs = sqlite3_vfs_find(nullptr);
    vfs->xSetSystemCall(vfs, "pwrite", (sqlite3_syscall_ptr) illPwrite);
    NSLog(@"Did disable SQLite write");
}

- (void)removeSQLRelatedConfigs
{
    NSArray<NSString *> *configNames = @[ WCTConfigNameBasic, WCTConfigNameBackup, WCTConfigNameCheckpoint, WCTConfigNameTokenize, WCTConfigNameCipher ];
    for (NSString *configName in configNames) {
        [self removeConfigForName:configName];
    }
}

@end
