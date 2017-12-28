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

import Foundation
final class Atomic<Value> {
    var raw: Value
    let spin = Spin()

    init(_ raw: Value) {
        self.raw = raw
    }

    var value: Value {
        spin.lock(); defer { spin.unlock() }
        return raw
    }

    func withValue(_ closure: (Value) -> Value) {
        spin.lock(); defer { spin.unlock() }
        raw = closure(raw)
    }

    func assign(_ newValue: Value) {
        spin.lock(); defer { spin.unlock() }
        self.raw = newValue
    }
}

extension Atomic where Value==Int {
    static func += (left: Atomic, right: Value) {
        left.withValue { (value) -> Value in
            return value + right
        }
    }

    static func -= (left: Atomic, right: Value) {
        left.withValue { (value) -> Value in
            return value - right
        }
    }

    static prefix func ++ (atomic: Atomic) -> Value {
        var newValue: Value = 0
        atomic.withValue { (value) -> Value in
            newValue = value + 1
            return newValue
        }
        return newValue
    }
}

extension Atomic where Value: Equatable {
    static func == (left: Atomic, right: Value) -> Bool {
        return left.value == right
    }
}

extension Atomic where Value: Comparable {
    static func < (left: Atomic, right: Value) -> Bool {
        return left.value < right
    }
    static func > (left: Atomic, right: Value) -> Bool {
        return left.value > right
    }
}

extension Atomic where Value: OptionalRepresentable {
    convenience init() {
        self.init(Value.`nil`)
    }
}

extension Atomic: CustomStringConvertible {
    var description: String {
        return "\(value)"
    }
}
