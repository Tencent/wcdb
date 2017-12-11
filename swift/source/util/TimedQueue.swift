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

class TimedQueue<Key: Hashable> {
    typealias Time = SteadyClock.AbsoluteTime
    typealias Element = (Key, SteadyClock)
    typealias List = [Element]
    typealias Map = [Key: List.Index]

    let delay: Time

    let conditionLock = ConditionLock()

    var list: List = []
    var map: Map = [:]

    init(withDelay delay: Time) {
        self.delay = delay
    }

    func reQueue(with key: Key) {
        conditionLock.lock(); defer { conditionLock.unlock() }

        let signal = list.isEmpty

        if let index = map.index(forKey: key) {
            list.remove(at: map[index].value)
            map.remove(at: index)
        }

        //delay
        list.append((key, SteadyClock.now()+delay))
        map[key] = list.startIndex
        if signal {
            conditionLock.signal()
        }
    }

    func wait(untilExpired onExpired: (Key) -> Void, forever: Bool = false) {
        do {
            conditionLock.lock(); defer { conditionLock.unlock() }
            while list.isEmpty {
                guard forever else {
                    return
                }
                conditionLock.wait()
            }
        }
        var get = false
        while !get {
            var element: Element!
            let now = SteadyClock.now()
            do {
                conditionLock.lock(); defer { conditionLock.unlock() }
                element = list.first!
                if now > element.1 {
                    list.removeFirst()
                    map.removeValue(forKey: element.0)
                    get = true
                }
            }
            if get {
                onExpired(element.0)
            } else {
                sleep(UInt32(element.1.timeIntervalSince(now)))
            }
        }
    }
}
