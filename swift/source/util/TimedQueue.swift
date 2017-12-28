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

final class TimedQueue<Key: Hashable> {
    typealias Element = (key: Key, clock: SteadyClock)
    typealias List = ContiguousArray<Element>
    typealias Map = [Key: List.Index]

    let delay: TimeInterval

    let conditionLock = ConditionLock()

    var list: List = []
    var map: Map = [:]

    init(withDelay delay: TimeInterval) {
        self.delay = delay
    }

    func remove(with key: Key) {
        conditionLock.lock(); defer { conditionLock.unlock() }

        guard let index = map.index(forKey: key) else {
            return
        }
        list.remove(at: map[index].value)
        map.remove(at: index)
    }

    func reQueue(with key: Key) {
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

    func wait(untilExpired onExpired: (Key) -> Void) {
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
