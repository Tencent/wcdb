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

#ifndef Assert_h
#define Assert_h

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#define WINQAssertEqual(describable, expression)                            \
    XCTAssertTrue(                                                             \
        [(expression)                                                          \
            isEqualToString:@((describable).getDescription().c_str())],        \
        @"different from `%@` for `%@`",                                                       \
        [@((describable).getDescription().c_str())                             \
            commonPrefixWithString:(expression) options                        \
                                  :NSCaseInsensitiveSearch], @((describable).getDescription().c_str()))

#define WTCAssertEqual(raw, expected)                            \
    XCTAssertTrue(                                                             \
        [(expected)                                                          \
            isEqualToString:@(raw)],        \
        @"different from %@ for %@",                                                       \
        [expected commonPrefixWithString:@(raw) options:NSCaseInsensitiveSearch], @(raw))


#endif /* Assert_h */
