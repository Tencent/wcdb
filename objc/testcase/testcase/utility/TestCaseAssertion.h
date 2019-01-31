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

#import <Foundation/Foundation.h>

#define __TestCaseHint(left, right)                \
    [NSString stringWithFormat:                    \
              @"\n_left [%@]"                      \
               "\nright [%@]"                      \
               "\n_comm [%@]",                     \
              left,                                \
              right,                               \
              [_left commonPrefixWithString:_right \
                                    options:NSCaseInsensitiveSearch]]

#ifdef DEBUG
#define TestCaseFailure() abort()
#else
#define TestCaseFailure()
#endif

#define TestCaseAssertTrue(cond, ...)     \
    {                                     \
        BOOL test = (cond);               \
        XCTAssertTrue(test, __VA_ARGS__); \
        if (!(test)) {                    \
            TestCaseFailure();            \
        }                                 \
    }

#define TestCaseAssertFalse(cond) \
    TestCaseAssertTrue(!(cond))

#define TestCaseAssertEqual(left, right) \
    TestCaseAssertTrue((left) == (right))

#define TestCaseAssertNotEqual(left, right) \
    TestCaseAssertTrue((left) != (right))

#define TestCaseAssertObjectEqual(left, right) \
    TestCaseAssertTrue([(left) isEqual:(right)])

#define TestCaseAssertSQLEqual(_sql, _expected)               \
    {                                                         \
        NSString* __sql = @((_sql).getDescription().c_str()); \
        NSString* __expected = (_expected);                   \
        TestCaseAssertStringEqual(__sql, __expected);         \
    }

#define TestCaseAssertStringEqual(left, right)                                                    \
    {                                                                                             \
        NSString* _left = (left);                                                                 \
        NSString* _right = (right);                                                               \
        TestCaseAssertTrue([_left isEqualToString:_right], @"%@", __TestCaseHint(_left, _right)); \
    }

#define TestCaseAssertResultYES(result) \
    TestCaseAssertTrue(result.isYES)

#define TestCaseAssertResultNO(result) \
    TestCaseAssertTrue(result.isNO)
