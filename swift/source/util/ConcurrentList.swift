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

final class ConcurrentList<Value> {
    var values: ContiguousArray<Value> = []
    let capacityCap: Int
    let spin = Spin()

    init(withCapacityCap capacityCap: Int) {
        self.capacityCap = capacityCap
    }

    func pushBack(_ value: Value) -> Bool {
        spin.lock(); defer { spin.unlock() }
        if values.count < capacityCap {
            values.append(value)
            return true
        }
        return false
    }

    func popBack() -> Value? {
        spin.lock(); defer { spin.unlock() }
        if !values.isEmpty {
            return values.removeLast()
        }
        return nil
    }

    func clear() -> Int {
        spin.lock(); defer { spin.unlock() }
        let count = values.count
        values.removeAll()
        return count
    }
}
