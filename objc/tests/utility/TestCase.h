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

#import "Console.h"
#import "NSObject+TestCase.h"
#import "TestCaseObject+WCTTableCoding.h"
#import "TestCaseObject.h"
#import "TestLog.h"
#import <WCDB/WCDB.h>
#import <XCTest/XCTest.h>

#ifdef DEBUG
#define TESTCASE_FAILED abort();
#else
#define TESTCASE_FAILED
#endif

@interface TestCase : XCTestCase

@property (nonatomic, readonly) NSString* root;

@property (nonatomic, readonly) NSString* directory;

@property (nonatomic, readonly) NSString* className;

@property (nonatomic, readonly) NSString* testName;

@property (nonatomic, readonly) NSString* identifier;

@property (nonatomic, readonly) NSFileManager* fileManager;

@property (nonatomic, readonly) Console* console;

- (void)refreshDirectory;

- (void)cleanDirectory;

+ (NSString*)hint:(NSString*)description expecting:(NSString*)expected;

- (void)log:(NSString*)format, ...;

@end

#define SQLAssertEqual(_sql, _expected)                                                                            \
    {                                                                                                              \
        NSString* __sql = @((_sql).getDescription().c_str());                                                      \
        NSString* __expected = (_expected);                                                                        \
        TestCaseAssertTrue([__expected isEqualToString:__sql], @"%@", [TestCase hint:__sql expecting:__expected]); \
    }

#define TestCaseAssertTrue(cond, ...)     \
    {                                     \
        BOOL test = (cond);               \
        if (!(test)) {                    \
            TESTCASE_FAILED               \
        }                                 \
        XCTAssertTrue(test, __VA_ARGS__); \
    }

#define TestCaseAssertFalse(cond, ...)     \
    {                                      \
        BOOL test = (cond);                \
        if (test) {                        \
            TESTCASE_FAILED                \
        }                                  \
        XCTAssertFalse(test, __VA_ARGS__); \
    }

#define TestCaseAssertEqual(left, right, ...)             \
    {                                                     \
        __typeof__(left) testLeft = (left);               \
        __typeof__(right) testRight = (right);            \
        if (testLeft != testRight) {                      \
            TESTCASE_FAILED                               \
        }                                                 \
        XCTAssertEqual(testLeft, testRight, __VA_ARGS__); \
    }

#define TestCaseAssertNotEqual(left, right, ...)             \
    {                                                        \
        __typeof__(left) testLeft = (left);                  \
        __typeof__(right) testRight = (right);               \
        if (testLeft == testRight) {                         \
            TESTCASE_FAILED                                  \
        }                                                    \
        XCTAssertNotEqual(testLeft, testRight, __VA_ARGS__); \
    }
