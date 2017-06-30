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

#import "WCTBenchmarkRecord.h"
#import "WCTBenchmarkDeviceInfo.h"
#import <mach/mach_time.h>

@implementation WCTBenchmarkRecord

- (instancetype)initWithType:(NSString *)type forDatabase:(NSString *)database
{
    if (self = [super init]) {
        _type = type;
        _database = database;
        _isInDebugMode = [WCTBenchmarkDeviceInfo currentDeviceInfo].isInDebugMode;
    }
    return self;
}

- (void)record:(RecordBlock)block
{
    NSDate *start = [NSDate date];
    _count = block();
    NSDate *stop = [NSDate date];

    _elapsedTime = stop.timeIntervalSince1970 - start.timeIntervalSince1970;
    _rate = _count / _elapsedTime;
}

- (NSString *)description
{
    NSMutableString *desc = [[NSMutableString alloc] init];
    [desc appendFormat:@"Benchmark:"];
    [desc appendFormat:@"\nDatabase: %@", _database];
    [desc appendFormat:@"\nType: %@", _type];
    [desc appendFormat:@"\nCount: %lu ops", (unsigned long) _count];
    [desc appendFormat:@"\nCost: %.2f secnonds", _elapsedTime];
    [desc appendFormat:@"\nRate: %.2f ops per second", _rate];
    if (_isInDebugMode) {
        [desc appendString:@"\nNote: Since this benchmark is done in debug mode, the result can't be trusted."];
    }
    return desc;
}

- (NSDictionary *)encodedResult
{
    NSMutableDictionary *dictionary = [[NSMutableDictionary alloc] init];
    [dictionary setObject:_database forKey:@"Database"];
    [dictionary setObject:_type forKey:@"Type"];
    [dictionary setObject:[NSString stringWithFormat:@"%.2f seconds", _elapsedTime] forKey:@"Elapsed Time"];
    [dictionary setObject:@(_count) forKey:@"Op Count"];
    [dictionary setObject:[NSString stringWithFormat:@"%.2f ops per second", _rate] forKey:@"Rate"];
    if (_isInDebugMode) {
        [dictionary setObject:@"Note: Since this benchmark is done in debug mode, the result can't be trusted." forKey:@"Warning"];
    }
    return dictionary;
}

@end
