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

#import "WCTBenchmark.h"
#import "WCTBenchmarkConfig.h"
#import "WCTBenchmarkConsole.h"
#import "WCTBenchmarkDeviceInfo.h"
#import "WCTBenchmarkProtocol.h"
#import "WCTBenchmarkRecord.h"
#import "WCTBenchmarkResult.h"
#import <objc/runtime.h>

@implementation WCTBenchmark {
    NSDictionary<NSString *, Class> *_benchmarks;
    WCTBenchmarkConsole *_console;
}

+ (instancetype)benchmark
{
    static WCTBenchmark *s_benchmark;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
      s_benchmark = [[WCTBenchmark alloc] init];
    });
    return s_benchmark;
}

- (void)registerBenchmarks
{
    NSMutableDictionary *benchmarks = [[NSMutableDictionary alloc] init];
    Class *classes = NULL;
    int num = objc_getClassList(NULL, 0);
    if (num > 0) {
        classes = (Class *) malloc(sizeof(Class) * num);
        num = objc_getClassList(classes, num);
        for (int i = 0; i < num; ++i) {
            Class cls = classes[i];
            if (class_conformsToProtocol(cls, @protocol(WCTBenchmarkProtocol))) {
                NSString *benchmarkType = [cls benchmarkType];
                if (benchmarkType) {
                    [benchmarks setObject:[cls copy] forKey:benchmarkType];
                }
            }
        }
        free(classes);
    }
    _benchmarks = benchmarks;
}

- (void)runAll
{
    for (NSString *benchmark in _benchmarks.allKeys) {
        [self runBenchmark:benchmark];
    }
}

- (void)run
{
    if ([_config.benchmark isEqualToString:@"All"]) {
        [self runAll];
    } else {
        [self runBenchmark:_config.benchmark];
    }
}

- (void)runBenchmark:(NSString *)benchmarkType
{
    Class cls = [_benchmarks objectForKey:benchmarkType];
    if (cls) {
        id<WCTBenchmarkProtocol> benchmark = [[cls alloc] initWithConfig:_config];
        WCTBenchmarkRecord *record = [benchmark run];
        [_console print:record.description];
        [self saveRecord:record];
    } else {
        [_console print:[NSString stringWithFormat:@"Error: unavailable benchmark \"%@\"", benchmarkType]];
    }
}

- (void)saveRecord:(WCTBenchmarkRecord *)record
{
    NSString *document = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES)[0];
    NSString *directory = [document stringByAppendingPathComponent:@"WCDBBenchmark"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:directory]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:directory withIntermediateDirectories:YES attributes:nil error:nil];
    }

    NSDateFormatter *formater = [[NSDateFormatter alloc] init];
    [formater setLocale:[NSLocale currentLocale]];
    [formater setDateFormat:@"yyyy_MM_dd_HH_mm_ss"];
    NSString *now = [formater stringFromDate:[NSDate date]];
    NSString *filename = [NSString stringWithFormat:@"%@_%@_%@.plist", record.database, record.type, now];

    NSString *path = [directory stringByAppendingPathComponent:filename];
    NSString *abbreviatingPath = [path stringByAbbreviatingWithTildeInPath];
    WCTBenchmarkResult *result = [[WCTBenchmarkResult alloc] init];
    [result addResult:record forKey:@"Record"];
    [result addResult:[WCTBenchmarkDeviceInfo currentDeviceInfo] forKey:@"Device Info"];
    if ([result writeToFile:path]) {
        [_console print:[NSString stringWithFormat:@"Record can be found at %@", abbreviatingPath]];
    } else {
        [_console print:[NSString stringWithFormat:@"Error: failed to export record to %@", abbreviatingPath]];
    }
}

- (instancetype)init
{
    if (self = [super init]) {
        _console = [WCTBenchmarkConsole console];
        _config = [[WCTBenchmarkConfig alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"Config" ofType:@"plist"]];
        _config.baseDirectory = NSTemporaryDirectory();
        [self registerBenchmarks];
    }
    return self;
}

@end
