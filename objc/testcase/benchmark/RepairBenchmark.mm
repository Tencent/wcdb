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

#import "BenchmarkCommon.h"
#import "RepairTestCase.h"
#import "RepairTestCaseObject+WCTTableCoding.h"
#import "RepairTestCaseObject.h"

@interface RepairBenchmark : Benchmark

@end

@implementation RepairBenchmark {
    WCTDatabase *_cachedDatabase;
}

- (void)setUp
{
    [super setUp];
    _cachedDatabase = [[WCTDatabase alloc] initWithPath:[self.class.cachedDirectory stringByAppendingPathComponent:self.className]];
}

- (BOOL)isDatabaseLargeEnough:(unsigned long long)fileSize
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDictionary<NSFileAttributeKey, id> *attributes = [fileManager attributesOfItemAtPath:_cachedDatabase.path error:nil];
    return attributes.fileSize > fileSize;
}

- (BOOL)lazyPrepareCachedDatabase:(unsigned long long)size
{
    NSUInteger databaseSize = [_cachedDatabase getFilesSize];
    if (databaseSize > 0) {
        if (size < databaseSize * 0.99 || size > databaseSize * 1.01) {
            XCTAssertTrue([_cachedDatabase removeFiles]);
        }
    }
    int percentage = 0;
    while (YES) {
        databaseSize = [_cachedDatabase getFilesSize];
        if (databaseSize >= size
            && databaseSize % getpagesize() != 0) {
            break;
        }
        int gap = (double) databaseSize / size * 100 - percentage;
        if (gap > 0) {
            percentage += gap;
            NSLog(@"Preparing %d%%", percentage);
        }
        if (![_cachedDatabase runTransaction:^BOOL(WCTHandle *_Nonnull) {
                NSArray<RepairTestCaseObject *> *objects = [RepairTestCaseObject randomObjects];
                NSString *tableName = [NSString stringWithFormat:@"t_%@", [NSString randomString]];
                return [_cachedDatabase createTableAndIndexes:tableName withClass:RepairTestCaseObject.class] && [_cachedDatabase insertOrReplaceObjects:objects intoTable:tableName];
            }]
            || ![_cachedDatabase execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]) {
            return NO;
        }
    }
    NSLog(@"Tests for database with size: %fMB", (double) databaseSize / 1024 / 1024);
    return YES;
}

//Note that since backup will be run concurrently and in the background thread, test is not for the best performance but for a tolerable performance.
- (void)test_backup
{
    NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:@(self.config.databaseSize), @"DatabaseSize", nil];
    [self addAttachment:[XCTAttachment attachmentWithPlistObject:dictionary]];
    __block BOOL result;
    NSString *firstBackupPath = [_cachedDatabase.path stringByAppendingString:@"-first.material"];
    NSString *lastBackupPath = [_cachedDatabase.path stringByAppendingString:@"-last.material"];
    [self
    measure:^{
        result = [_cachedDatabase backup];
    }
    setUp:^{
        XCTAssertTrue([self lazyPrepareCachedDatabase:self.config.databaseSize]);
    }
    tearDown:^{
        if ([self.fileManager fileExistsAtPath:firstBackupPath]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:firstBackupPath error:nil]);
        }
        if ([self.fileManager fileExistsAtPath:lastBackupPath]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:lastBackupPath error:nil]);
        }
    }
    checkCorrectness:^{
        XCTAssertTrue(result);
        XCTAssertGreaterThan([self.fileManager attributesOfItemAtPath:firstBackupPath error:nil].fileSize, self.config.databaseSize * 0.0001);
        XCTAssertLessThan([self.fileManager attributesOfItemAtPath:firstBackupPath error:nil].fileSize, self.config.databaseSize * 0.02);
        XCTAssertFalse([self.fileManager fileExistsAtPath:lastBackupPath]);
    }];
}

//Note that since repair will usually be run in background thread with blocked UI, test is not for the best performance but for a tolerable performance.
- (void)test_repair_without_backup
{
    NSString *firstBackupPath = [_cachedDatabase.path stringByAppendingString:@"-first.material"];
    NSString *lastBackupPath = [_cachedDatabase.path stringByAppendingString:@"-last.material"];

    __block double score;
    [self
    measure:^{
        score = [_cachedDatabase retrieve:nil];
    }
    setUp:^{
        XCTAssertTrue([self lazyPrepareCachedDatabase:self.config.databaseSize]);
    }
    tearDown:^{
        if ([self.fileManager fileExistsAtPath:firstBackupPath]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:firstBackupPath error:nil]);
        }
        if ([self.fileManager fileExistsAtPath:lastBackupPath]) {
            XCTAssertTrue([self.fileManager removeItemAtPath:lastBackupPath error:nil]);
        }
    }
    checkCorrectness:^{
        XCTAssertEqual(score, 1);
    }];
}

- (void)test_repair_with_backup
{
    NSString *firstBackupPath = [_cachedDatabase.path stringByAppendingString:@"-first.material"];
    NSString *lastBackupPath = [_cachedDatabase.path stringByAppendingString:@"-last.material"];

    __block double score;
    [self
    measure:^{
        score = [_cachedDatabase retrieve:nil];
    }
    setUp:^{
        XCTAssertTrue([self lazyPrepareCachedDatabase:self.config.databaseSize]);
        XCTAssertTrue([_cachedDatabase backup]);

        XCTAssertTrue([self.fileManager fileExistsAtPath:firstBackupPath] || [self.fileManager fileExistsAtPath:lastBackupPath]);
    }
    tearDown:nil
    checkCorrectness:^{
        XCTAssertEqual(score, 1);
    }];
}

@end
