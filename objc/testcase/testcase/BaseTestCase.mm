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

#import <TestCase/BaseTestCase.h>
#import <TestCase/Random.h>
#import <TestCase/Signpost.h>
#import <TestCase/TestCaseAssertion.h>
#import <TestCase/TestCaseLog.h>
#import <WCDB/WCDB.h>

@implementation BaseTestCase {
    Random *_random;
    Signpost *_signpost;
    NSString *_className;
    NSString *_testName;
    NSString *_directory;
}

- (void)setUp
{
    [super setUp];

    self.continueAfterFailure = YES;

#ifdef DEBUG
    WCTDatabase.debuggable = YES;
#else
    WCTDatabase.debuggable = NO;
#endif

    if (WCTDatabase.debuggable) {
        [self log:@"debuggable."];
    }

    [self refreshDirectory];

    NSString *directory = self.directory;
    NSString *abbreviatedPath = directory.stringByAbbreviatingWithTildeInPath;
    if (abbreviatedPath.length > 0) {
        directory = abbreviatedPath;
    }
    [self log:@"run at %@", directory];
}

- (void)tearDown
{
    [self cleanDirectory];
    [super tearDown];
}

- (Random *)random
{
    if (!_random) {
        _random = [[Random alloc] init];
    }
    return _random;
}

- (Signpost *)signpost
{
    if (!_signpost) {
        _signpost = [[Signpost alloc] initWithSystem:[NSBundle mainBundle].bundleIdentifier andCategory:self.className];
    }
    return _signpost;
}

- (NSString *)testName
{
    if (!_testName) {
        NSString *name = self.name;
        NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:@"\\[\\w+ (\\w+).*\\]" options:NSRegularExpressionCaseInsensitive error:nil];
        NSTextCheckingResult *match = [regex firstMatchInString:name options:0 range:NSMakeRange(0, [name length])];
        _testName = [name substringWithRange:[match rangeAtIndex:1]];
    }
    return _testName;
}

- (void)refreshDirectory
{
    [self cleanDirectory];
    TestCaseAssertTrue([self.fileManager createDirectoryAtPath:self.directory
                                   withIntermediateDirectories:YES
                                                    attributes:nil
                                                         error:nil]);
}

- (void)cleanDirectory
{
    if ([self.fileManager fileExistsAtPath:self.directory]) {
        TestCaseAssertTrue([self.fileManager removeItemAtPath:self.directory error:nil]);
    }
}

- (NSString *)className
{
    if (!_className) {
        _className = NSStringFromClass(self.class);
    }
    return _className;
}

+ (NSString *)root
{
    return [[NSTemporaryDirectory() stringByAppendingPathComponent:@"WCDB"] stringByAppendingPathComponent:[NSBundle mainBundle].bundleIdentifier];
}

- (NSString *)directory
{
    if (!_directory) {
        _directory = [[self.class.root stringByAppendingPathComponent:self.className] stringByAppendingPathComponent:self.testName];
    }
    return _directory;
}

- (NSFileManager *)fileManager
{
    return [NSFileManager defaultManager];
}

- (void)log:(NSString *)format, ...
{
    va_list ap;
    va_start(ap, format);
    NSString *description = [[NSString alloc] initWithFormat:format arguments:ap];
    va_end(ap);
    NSString *log = [NSString stringWithFormat:@"Test Case '%@' %@", self.name, description];
    TestCaseLog(@"%@", log);
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
                           if (tearDownBlock) {
                               tearDownBlock();
                           }

                           if (setUpBlock) {
                               setUpBlock();
                           }

                           TestCaseSignpostBegin("measure");

                           [self log:@"%d started.", i];

                           [self startMeasuring];

                           block();

                           [self stopMeasuring];

                           [self log:@"%d passed.", i];

                           TestCaseSignpostEnd("measure");

                           correctnessBlock();

                           if (tearDownBlock) {
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

@end
