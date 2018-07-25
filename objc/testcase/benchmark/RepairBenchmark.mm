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

@implementation RepairBenchmark

- (void)setUpDatabase
{
    [super setUpDatabase];
    self.database = self.cachedDatabase;
}

- (BOOL)isDatabaseLargeEnough:(unsigned long long)fileSize
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDictionary<NSFileAttributeKey, id> *attributes = [fileManager attributesOfItemAtPath:self.database.path error:nil];
    return attributes.fileSize > fileSize;
}

- (BOOL)lazyPrepareCachedDatabase:(unsigned long long)size
{
    NSUInteger databaseSize = [self.database getFilesSize];
    if (databaseSize > size * 1.03) {
        XCTAssertTrue([self.database removeFiles]);
    }
    int percentage = 0;
    BOOL wait = NO;
    while (YES) {
        databaseSize = [self.database getFilesSize];
        if (databaseSize >= size) {
            break;
        }
        int gap = (double) databaseSize / size * 100 - percentage;
        if (gap > 0) {
            percentage += gap;
            NSLog(@"Preparing %d%%", percentage);
        }
        wait = YES;
        if (![self.database runTransaction:^BOOL(WCTHandle *_Nonnull) {
                NSArray<RepairTestCaseObject *> *objects = [RepairTestCaseObject randomObjects];
                NSString *tableName = [NSString stringWithFormat:@"t_%@", [NSString randomString]];
                return [self.database createTableAndIndexes:tableName withClass:RepairTestCaseObject.class] && [self.database insertOrReplaceObjects:objects intoTable:tableName];
            }]
            || ![self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]) {
            return NO;
        }
    }
    NSLog(@"Tests for database with size: %fMB", (double) databaseSize / 1024 / 1024);
    if (wait) {
        // wait until the checkpoint thread end
        [NSThread sleepForTimeInterval:5];
    }
    return YES;
}

//Note that since backup will be run concurrently and in the background thread, test is not for the best performance but for a tolerable performance.
- (void)test_backup
{
    [self setUpDatabase];
    __block BOOL result;
    NSString *firstBackupPath = [self.database.path stringByAppendingString:@"-first.material"];
    NSString *lastBackupPath = [self.database.path stringByAppendingString:@"-last.material"];
    [self
    measure:^{
        result = [self.database backup];
    }
    setUp:^{
        XCTAssertTrue([self lazyPrepareCachedDatabase:self.config.databaseSizeForBackup]);
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
        XCTAssertGreaterThan([self.fileManager attributesOfItemAtPath:firstBackupPath error:nil].fileSize, self.config.databaseSizeForBackup * 0.0001);
        XCTAssertLessThan([self.fileManager attributesOfItemAtPath:firstBackupPath error:nil].fileSize, self.config.databaseSizeForBackup * 0.02);
        XCTAssertFalse([self.fileManager fileExistsAtPath:lastBackupPath]);
    }];
}

//Note that since repair will usually be run in background thread with blocked UI, test is not for the best performance but for a tolerable performance.
- (void)test_repair_without_backup
{
    [self setUpDatabase];
    NSString *firstBackupPath = [self.database.path stringByAppendingString:@"-first.material"];
    NSString *lastBackupPath = [self.database.path stringByAppendingString:@"-last.material"];
    __block double score;
    [self
    measure:^{
        score = [self.database retrieve:nil];
    }
    setUp:^{
        XCTAssertTrue([self lazyPrepareCachedDatabase:self.config.databaseSizeForRepair]);
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
    [self setUpDatabase];
    NSString *firstBackupPath = [self.database.path stringByAppendingString:@"-first.material"];
    NSString *lastBackupPath = [self.database.path stringByAppendingString:@"-last.material"];
    __block double score;
    [self
    measure:^{
        score = [self.database retrieve:nil];
    }
    setUp:^{
        XCTAssertTrue([self lazyPrepareCachedDatabase:self.config.databaseSizeForRepair]);
        XCTAssertTrue([self.database backup]);
        XCTAssertTrue([self.fileManager fileExistsAtPath:firstBackupPath] || [self.fileManager fileExistsAtPath:lastBackupPath]);
    }
    tearDown:nil
    checkCorrectness:^{
        XCTAssertEqual(score, 1);
    }];
}

@end
