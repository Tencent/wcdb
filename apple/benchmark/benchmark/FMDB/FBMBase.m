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

#import "FBMBase.h"

@implementation FBMBase

- (void)openDatabase:(FMDatabase *)database
{
    if (![[NSFileManager defaultManager] createDirectoryAtPath:[database.databasePath stringByDeletingLastPathComponent] withIntermediateDirectories:YES attributes:nil error:nil]) {
        abort();
    }
    if (![database open]) {
        abort();
    }

    FMResultSet *resultSet;
    NSError *error;

    resultSet = [database executeQuery:@"PRAGMA cache_size=-2000"];
    [resultSet nextWithError:&error];
    if (error) {
        abort();
    }
    [resultSet close];

    resultSet = [database executeQuery:@"PRAGMA page_size=4096"];
    [resultSet nextWithError:&error];
    if (error) {
        abort();
    }
    [resultSet close];

    resultSet = [database executeQuery:@"PRAGMA locking_mode=NORMAL"];
    [resultSet nextWithError:&error];
    if (error) {
        abort();
    }
    [resultSet close];

    resultSet = [database executeQuery:@"PRAGMA synchronous=NORMAL"];
    [resultSet nextWithError:&error];
    if (error) {
        abort();
    }
    [resultSet close];

    resultSet = [database executeQuery:@"PRAGMA journal_mode=WAL"];
    [resultSet nextWithError:&error];
    if (error) {
        abort();
    }
    [resultSet close];
}

- (instancetype)initWithConfig:(WCTBenchmarkConfig *)config
{
    if (self = [super init]) {
        _console = [WCTBenchmarkConsole console];
        _config = config;
        _baseDirectory = [[_config.baseDirectory stringByAppendingPathComponent:self.class.databaseType] stringByAppendingPathComponent:self.class.benchmarkType];
        _path = [_baseDirectory stringByAppendingPathComponent:@"database"];
        _randomGenerator = [[WCTBenchmarkRandomGenerator alloc] initWithSeed:_config.randomSeed];
        _tableName = @"benchmark";
    }
    return self;
}

+ (NSString *)name
{
    return [self.benchmarkType copy];
}

+ (NSString *)benchmarkType
{
    return nil;
}

+ (NSString *)databaseType
{
    return [WCTBenchmarkDatabaseFMDB copy];
}

- (void)clean
{
    NSFileManager *fm = [NSFileManager defaultManager];
    if ([fm fileExistsAtPath:_path]) {
        BOOL result = [fm removeItemAtPath:_path error:nil];
        if (!result) {
            abort();
        }
    }
}

- (NSUInteger)benchmark
{
    return 0;
}

- (WCTBenchmarkRecord *)run
{
    [_console print:@"Cleaning data..."];
    [self clean];
    [_console print:@"Done"];
    WCTBenchmarkRecord *record = [[WCTBenchmarkRecord alloc] initWithType:[self.class.benchmarkType copy] forDatabase:[self.class.databaseType copy]];
    if ([self respondsToSelector:@selector(prepare)]) {
        [_console print:@"Preparing database..."];
        [self prepare];
        [_console print:@"Done"];
    }
    if ([self respondsToSelector:@selector(preBenchmark)]) {
        [_console print:@"Preparing pre-data..."];
        [self preBenchmark];
        [_console print:@"Done"];
    }
    [_console print:@"Start benchmark..."];
    [record record:^NSUInteger {
      return [self benchmark];
    }];
    [_console print:@"Done"];
    if ([self respondsToSelector:@selector(postBenchmark)]) {
        [_console print:@"Cleaning pre-data..."];
        [self postBenchmark];
        [_console print:@"Done"];
    }
    return record;
}

@end
