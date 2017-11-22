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

import XCTest
import WCDB

func WINQAssertEqual(_ expression1: @autoclosure () -> String, _ expression2: @autoclosure () -> String, file: StaticString, line: UInt) {
    let description1 = expression1()
    let description2 = expression2()
    XCTAssertEqual(description1, description2, {
        return "different from \"\(description1.commonPrefix(with: description2))\""
    }(), file: file, line: line)
}

func WINQAssertEqual<T>(_ expression1: @autoclosure () -> T, _ expression2: @autoclosure () -> String, file: StaticString = #file, line: UInt = #line) where T : Describable {
    WINQAssertEqual(expression1().description, expression2(), file: file, line: line)
}

func WINQAssertEqual<T>(_ expression1: @autoclosure () -> T, _ expression2: @autoclosure () -> String, _ message: @autoclosure () -> String = "", file: StaticString = #file, line: UInt = #line) where T : CustomStringConvertible {
    WINQAssertEqual(expression1().description, expression2(), file: file, line: line)
}

