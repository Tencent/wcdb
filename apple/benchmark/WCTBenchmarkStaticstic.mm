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

#import "WCTBenchmarkStaticstic.h"
#import <mach/mach_time.h>

@implementation WCTBenchmarkStaticstic {
    NSMutableArray<NSNumber*>* _ticks;
}

- (instancetype)init
{
    if (self = [super init]) {
        _ticks = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)start
{
    [self tick];
}

- (void)tick
{
    NSNumber* time = [NSNumber numberWithLongLong:mach_absolute_time()];
    [_ticks addObject:time];
}

- (void)stop
{
    [self tick];
}

- (void)report
{
    for (NSNumber* elapse in [self getElapseTimes]) {
        NSLog(@"Tick cost : %f ms", elapse.doubleValue);
    }
}

- (NSArray<NSNumber*>*)getElapseTimes
{
    NSMutableArray<NSNumber*>* elapseTimes = [[NSMutableArray alloc] init];
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    
    uint64_t lastTick = 0;
    for (NSUInteger i = 0; i < _ticks.count; ++i) {
        uint64_t currentTick = _ticks[i].longLongValue;
        if (i>0) {
            uint64_t elapse = currentTick-lastTick;
            double ns = (double)elapse * (double)info.numer / (double)info.denom;
            double ms = ns/1000/1000;
            [elapseTimes addObject:[NSNumber numberWithDouble:ms]];
        }
        lastTick = currentTick;
    }
    return elapseTimes;
}

@end
