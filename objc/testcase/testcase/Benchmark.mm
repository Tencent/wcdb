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

#import <TestCase/Benchmark.h>
#import <TestCase/Random.h>
#import <TestCase/Signpost.h>
#import <TestCase/TestCaseLog.h>

@implementation Benchmark {
    Signpost *_signpost;
}

- (void)setUp
{
    [super setUp];
    [self.random enableStable];
}

- (void)doMeasure:(void (^)(void))block
            setUp:(void (^)(void))setUpBlock
         tearDown:(void (^)(void))tearDownBlock
 checkCorrectness:(void (^)(void))correctnessBlock
{
    __block int i = 1;
    [self measureMetrics:self.class.defaultPerformanceMetrics
    automaticallyStartMeasuring:false
                       forBlock:^{
                           if (tearDownBlock != nil) {
                               tearDownBlock();
                           }

                           if (setUpBlock != nil) {
                               setUpBlock();
                           }

                           if (@available(iOS 12.0, macOS 10.14, *)) {
                               BenchmarkSignpostBegin("measure");
                           }

                           [self log:@"%d started.", i];

                           [self startMeasuring];

                           block();

                           [self stopMeasuring];

                           if (@available(iOS 12.0, macOS 10.14, *)) {
                               BenchmarkSignpostEnd("measure");
                           }

                           [self log:@"%d passed.", i];

                           correctnessBlock();

                           if (tearDownBlock != nil) {
                               tearDownBlock();
                           }

                           ++i;
                       }];

    BOOL untrusted = WCTDatabase.debuggable;
#if DEBUG || TARGET_IPHONE_SIMULATOR
    untrusted = YES;
#endif
    if (untrusted) {
        TestCaseLog(@"Benchmark is run in debug mode or simulator. The result may be untrusted.");
    }
}

- (Signpost *)signpost
{
    @synchronized(self) {
        if (_signpost == nil) {
            _signpost = [[Signpost alloc] initWithSystem:[NSBundle mainBundle].bundleIdentifier andCategory:self.className];
        }
        return _signpost;
    }
}

@end
