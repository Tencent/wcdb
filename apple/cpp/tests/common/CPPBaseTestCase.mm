//
// Created by qiuwenchen on 2022/8/10.
//

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

#import "CPPBaseTestCase.h"
#import "Dispatch.h"
#import "NSObject+TestCase.h"
#import "Random.h"
#import "TestCaseAssertion.h"
#import "TestCaseLog.h"
#import <Foundation/Foundation.h>
#import <WCDB/WCDB-CPP.h>

@implementation CPPBaseTestCase {
    Random *_random;
    NSString *_className;
    NSString *_testName;
    NSString *_directory;
    Dispatch *_dispatch;
}

+ (void)initialize
{
    if (self.class == CPPBaseTestCase.class) {
        TestCaseAssertTrue([NSThread isMainThread]);
        pthread_setname_np("com.Tencent.WCDB.Main");
    }
}

+ (void)breakpoint
{
}

- (void)setUp
{
    [super setUp];

    self.continueAfterFailure = NO;

    [Random shared].stable = NO;

    WCDB::Database::globalTracePerformance(nullptr);
    WCDB::Database::globalTraceSQL(nullptr);
    Class selfClass = self.class;
    WCDB::Database::globalTraceError([selfClass](const WCDB::Error &error) {
        NSThread *currentThread = [NSThread currentThread];
        NSString *threadName = currentThread.name;
        if (threadName.length == 0) {
            threadName = [NSString stringWithFormat:@"%p", currentThread];
        }
        switch (error.level) {
        case WCDB::Error::Level::Debug:
#ifndef DEBUG
            break;
#endif
            // passthrough
        default:
            TestCaseLog(@"%@ Thread %@: %s", currentThread.isMainThread ? @"*" : @"-", threadName, error.getDescription().data());
            break;
        }

        switch (error.level) {
        case WCDB::Error::Level::Error:
            [selfClass breakpoint];
            break;
        case WCDB::Error::Level::Fatal:
            abort();
            break;
        default:
            break;
        }
    });

#if DEBUG
    [self log:@"debuggable."];
#endif

    NSString *directory = self.directory;
    NSString *abbreviatedPath = directory.stringByAbbreviatingWithTildeInPath;
    if (abbreviatedPath.length > 0) {
        directory = abbreviatedPath;
    }
    [self log:@"run at %@", directory];

    [self refreshDirectory];
}

- (void)tearDown
{
    [Random shared].stable = NO;

    [self.dispatch waitUntilDone];

    WCDB::Database::globalTracePerformance(nullptr);
    WCDB::Database::globalTraceSQL(nullptr);
    WCDB::Database::globalTraceError(nullptr);
    [self cleanDirectory];
    [super tearDown];
}

- (Random *)random
{
    return [Random shared];
}

- (NSString *)testName
{
    @synchronized(self) {
        if (_testName == nil) {
            NSString *name = self.name;
            NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:@"\\[\\w+ (\\w+).*\\]" options:NSRegularExpressionCaseInsensitive error:nil];
            NSTextCheckingResult *match = [regex firstMatchInString:name options:0 range:NSMakeRange(0, [name length])];
            _testName = [name substringWithRange:[match rangeAtIndex:1]];
        }
        return _testName;
    }
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
        NSError *error;
        TestCaseAssertTrue([self.fileManager removeItemAtPath:self.directory error:&error]);
    }
}

- (NSString *)className
{
    @synchronized(self) {
        if (_className == nil) {
            _className = NSStringFromClass(self.class);
        }
        return _className;
    }
}

+ (NSString *)root
{
    return [[[NSTemporaryDirectory() stringByAppendingPathComponent:@"WCDBCpp"] stringByAppendingPathComponent:[NSBundle mainBundle].bundleIdentifier] test_stringByStandardizingPath];
}

+ (NSString *)cacheRoot
{
    return [[[NSTemporaryDirectory() stringByAppendingPathComponent:@"WCDBCpp"] stringByAppendingPathComponent:[[NSBundle mainBundle].bundleIdentifier stringByAppendingString:@".Cache"]] test_stringByStandardizingPath];
}

- (NSString *)directory
{
    @synchronized(self) {
        if (_directory == nil) {
            _directory = [[self.class.root stringByAppendingPathComponent:self.className] stringByAppendingPathComponent:self.testName];
        }
        return _directory;
    }
}

- (NSFileManager *)fileManager
{
    return [NSFileManager defaultManager];
}

- (Dispatch *)dispatch
{
    @synchronized(self) {
        if (_dispatch == nil) {
            _dispatch = [[Dispatch alloc] init];
        }
        return _dispatch;
    }
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

@end
