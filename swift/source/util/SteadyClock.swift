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

struct SteadyClock {
    typealias AbsoluteTime = CFAbsoluteTime 
    
    var absoluteTime: AbsoluteTime
    
    init() {
        self.absoluteTime = CFAbsoluteTimeGetCurrent()
    }
    
    private init(with absoluteTime: AbsoluteTime) {
        self.absoluteTime = absoluteTime
    }
    
    func timeIntervalSince(_ other: SteadyClock) -> AbsoluteTime {
        return absoluteTime - other.absoluteTime
    }
    
    static func + (steadyClock: SteadyClock, timeInterval: AbsoluteTime) -> SteadyClock {
        return SteadyClock(with: steadyClock.absoluteTime + timeInterval)
    }
    
    static func - (steadyClock: SteadyClock, timeInterval: AbsoluteTime) -> SteadyClock {
        return SteadyClock(with: steadyClock.absoluteTime - timeInterval)
    }
    
    static func timeIntervalSince(_ other: SteadyClock) -> AbsoluteTime {
        return SteadyClock.now().timeIntervalSince(other)
    }
    
    static func now() -> SteadyClock {
        return SteadyClock()
    }
}

extension SteadyClock: Comparable {
    static func <(lhs: SteadyClock, rhs: SteadyClock) -> Bool {
        return lhs.absoluteTime < rhs.absoluteTime
    }
    
    static func ==(lhs: SteadyClock, rhs: SteadyClock) -> Bool {
        return lhs.absoluteTime == rhs.absoluteTime
    }
}

