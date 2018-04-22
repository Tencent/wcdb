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

internal final class TimedQueue<Key: Hashable> {
    private typealias Element = (key: Key, clock: SteadyClock)
    private typealias List = ContiguousArray<Element>
    private typealias Map = [Key: List.Index]

    private let delay: TimeInterval

    private let conditionLock = ConditionLock()

    private var list: List = []
    private var map: Map = [:]

    internal init(withDelay delay: TimeInterval) {
        self.delay = delay
    }

    internal func remove(with key: Key) {
        conditionLock.lock(); defer { conditionLock.unlock() }

        guard let index = map.index(forKey: key) else {
            return
        }
        list.remove(at: map[index].value)
        map.remove(at: index)
    }

    internal func reQueue(with key: Key) {
        conditionLock.lock(); defer { conditionLock.unlock() }

        let signal = list.isEmpty

        if let index = map.index(forKey: key) {
            list.remove(at: map[index].value)
            map.remove(at: index)
        }

        list.append((key, SteadyClock.now()+delay))
        map[key] = list.startIndex
        if signal {
            conditionLock.signal()
        }
    }

    internal func wait(untilExpired onExpired: (Key) -> Void) {
        while true {
            var key: Key!
            do {
                conditionLock.lock(); defer { conditionLock.unlock() }
                guard let element = list.first else {
                    conditionLock.wait()
                    continue
                }
                let now = SteadyClock.now()
                guard now >= element.clock else {
                    conditionLock.wait(timeout: element.clock - now)
                    continue
                }
                key = element.key
                list.removeFirst()
                map.removeValue(forKey: key)
            }
            onExpired(key)
            break
        }
    }
}
