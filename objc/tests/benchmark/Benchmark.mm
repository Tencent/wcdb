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

#import "Benchmark.h"

@implementation Benchmark

- (void)setUp
{
    [super setUp];

    WCTDatabase.debuggable = NO;
    [Console disableSQLTrace];

    _factory = [[BenchmarkFactory alloc] initWithDirectory:[self.root stringByAppendingPathComponent:@"factory"]];
}

- (void)tearDown
{
    BOOL untrusted = WCTDatabase.debuggable;
#if DEBUG || TARGET_IPHONE_SIMULATOR
    untrusted = YES;
#endif
    if (untrusted) {
        TestLog(@"Benchmark is run in debug mode or simulator. The result may be untrusted.");
    }

    [super tearDown];
}

- (void)measure:(void (^)(void))block
           setUp:(void (^)(void))setUpBlock
        tearDown:(void (^)(void))tearDownBlock
checkCorrectness:(void (^)(void))correctnessBlock
{
    __block int i = 1;
    [self measureMetrics:self.class.defaultPerformanceMetrics
    automaticallyStartMeasuring:false
                       forBlock:^{
                           if (tearDownBlock) {
                               tearDownBlock();
                           }

                           if (setUpBlock) {
                               setUpBlock();
                           }

                           [NSThread sleepForTimeInterval:0.5f];

                           [self log:@"%d started.", i];

                           [self startMeasuring];

                           block();

                           [self stopMeasuring];

                           [self log:@"%d passed.", i];

                           [NSThread sleepForTimeInterval:0.5f];

                           correctnessBlock();

                           if (tearDownBlock) {
                               tearDownBlock();
                           }

                           ++i;
                       }];
}

@end
