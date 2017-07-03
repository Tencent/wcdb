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

#import "WBMBase.h"

@implementation WBMBase

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

+ (const NSString *)benchmarkType
{
    return nil;
}

+ (const NSString *)databaseType
{
    return WCTBenchmarkDatabaseWCDB;
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
