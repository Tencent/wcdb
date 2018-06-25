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

- (BOOL)lazyPrepareCachedDatabase:(unsigned long long)fileSize
{
    while (![self isDatabaseLargeEnough:fileSize]) {
        if (![_cachedDatabase runTransaction:^BOOL(WCTHandle *_Nonnull) {
              NSArray<RepairTestCaseObject *> *objects = [RepairTestCaseObject randomObjects];
              NSString *tableName = [NSString stringWithFormat:@"t_%@", [NSString randomString]];
              return [_cachedDatabase createTableAndIndexes:tableName withClass:RepairTestCaseObject.class] && [_cachedDatabase insertOrReplaceObjects:objects intoTable:tableName];
            }] ||
            ![_cachedDatabase execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).to("TRUNCATE")]) {
            return NO;
        }
    }
    return YES;
}

//Note that since backup will be run concurrently and in the background thread, test is not for the best performance but for a tolerable performance.
- (void)test_backup
{
    __block BOOL result;
    NSString *firstBackupPath = [_cachedDatabase.path stringByAppendingString:@"-first.material"];
    NSString *lastBackupPath = [_cachedDatabase.path stringByAppendingString:@"-last.material"];
    [self measure:^{
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
          XCTAssertLessThan([self.fileManager attributesOfItemAtPath:firstBackupPath error:nil].fileSize, self.config.databaseSize * 0.01);
          XCTAssertFalse([self.fileManager fileExistsAtPath:lastBackupPath]);
        }];
}

//Note that since repair will usually be run in background thread with blocked UI, test is not for the best performance but for a tolerable performance.
- (void)test_repair
{
}

@end
