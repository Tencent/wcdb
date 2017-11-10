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
class Atomic<T> {
    var raw: T
    let spin = Spin()
    
    init(_ raw: T) {
        self.raw = raw
    }
    
    var value: T {
        get {
            spin.lock(); defer { spin.unlock() }
            return raw
        }
        set {
            spin.lock(); defer { spin.unlock() }
            raw = newValue
        }
    }

    func withValue(_ closure: (T)->T) {
        spin.lock(); defer { spin.unlock() }
        raw = closure(raw)
    }
    
    func assign(_ newValue: T) {
        spin.lock(); defer { spin.unlock() }
        self.raw = newValue
    }
}

extension Atomic where T==Int {
    static func +=(left: Atomic, right: T) {
        left.withValue { (value) -> T in
            return value+right
        }
    }
    
    static func -=(left: Atomic, right: T) {
        left.withValue { (value) -> T in
            return value-right
        }
    }
    
    static prefix func ++(atomic: Atomic) -> T {
        var newValue: T = 0
        atomic.withValue { (value) -> T in
            newValue = value-1
            return newValue
        }
        return newValue
    }
}

extension Atomic where T: Equatable {
    static func == (left: Atomic, right: T) -> Bool {
        return left.value == right
    }
}

extension Atomic where T: Comparable {
    static func < (left: Atomic, right: T) -> Bool {
        return left.value < right
    }
    static func <= (left: Atomic, right: T) -> Bool {
        return left.value <= right
    }
    static func > (left: Atomic, right: T) -> Bool {
        return left.value > right
    }
    static func >= (left: Atomic, right: T) -> Bool {
        return left.value >= right
    }
}
