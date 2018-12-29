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

#import "BenchmarkDatabaseFactory.h"
#import "Console.h"
#import "Random.h"
#import "TestLog.h"

@implementation BenchmarkDatabaseFactory

- (instancetype)initWithDirectory:(NSString*)directory
{
    if (self = [super init]) {
        _directory = directory;
        _tolerance = 0.01;
    }
    return self;
}

- (void)setFileSizeInMB:(NSUInteger)fileSizeInMB
{
    self.fileSize = fileSizeInMB * 1024 * 1024;
}

- (NSString*)getSlotedDatabasePath
{
    NSMutableString* fileName = [NSMutableString stringWithString:@"slot_"];
    if (self.multiTables) {
        [fileName appendString:@"multiTables_"];
    }
    [fileName appendFormat:@"_%lu", (unsigned long) self.fileSize];
    return [self.directory stringByAppendingPathComponent:fileName];
}

- (BOOL)isFileSizeTolerable:(unsigned long long)fileSize
{
    return fileSize > self.fileSize * (1.0f - self.tolerance)
           && fileSize < self.fileSize * (1.0f + self.tolerance);
}

- (BOOL)production:(NSString*)destination
{
    NSString* path = [self getSlotedDatabasePath];
    NSFileManager* fileManager = [NSFileManager defaultManager];

    do {
        NSDictionary<NSFileAttributeKey, id>* attributes = [fileManager attributesOfItemAtPath:path error:nil];
        if (attributes.fileIsImmutable
            && [self isFileSizeTolerable:attributes.fileSize]) {
            TestLog(@"database size: %.2fMB", (double) attributes.fileSize / 1024 / 1024);
            break;
        } else {
            if (![self prepare:path]
                || ![fileManager setAttributes:@{NSFileImmutable : @(YES)} ofItemAtPath:path error:nil]) {
                return NO;
            }
        }
    } while (YES);

    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:destination];
    if (![database removeFiles]) {
        return NO;
    }
    [database invalidate];
    database = nil;

    return [fileManager copyItemAtPath:path toPath:destination error:nil]
           && [fileManager setAttributes:@{NSFileImmutable : @(NO)} ofItemAtPath:destination error:nil];
}

- (BOOL)prepare:(NSString*)path
{
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:path];

    Random* random = [[Random alloc] init];
    random.seed = 0; // make sure the production is always same.

    if (![database removeFiles]) {
        return NO;
    }

    __block NSString* tableName = nil;
    int step = int(self.fileSize / 1024 / 1024 * 100);
    double progress = 0;
    for (NSUInteger size = [database getFilesSize]; size < self.fileSize; size = [database getFilesSize]) {
        double newProgress = (double) size / self.fileSize;
        if (newProgress - progress > 0.05) {
            progress = newProgress;
            TestLog(@"Preparing %.2f%%", progress * 100);
        }

        NSMutableArray* objects = [NSMutableArray arrayWithCapacity:step];
        for (int i = 0; i < step; ++i) {
            BenchmarkObject* object = [[BenchmarkObject alloc] init];
            object.identifier = i;
            object.content = random.string;
            [objects addObject:object];
        }

        if (![database runTransaction:^BOOL(WCTHandle* handle) {
                if (tableName == nil || self.multiTables) {
                    tableName = [NSString stringWithFormat:@"t_%@", random.string];
                    if (![database createTableAndIndexes:tableName withClass:BenchmarkObject.class]) {
                        return NO;
                    }
                }
                return [handle insertObjects:objects intoTable:tableName];
            }]) {
            return NO;
        }

        if (![database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).with("TRUNCATE")]) {
            return NO;
        }
    }
    return YES;
}

@end
