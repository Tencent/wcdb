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

class RandomData {
    private var position = 0
    private let capacity = 1024 * 1024
    private lazy var data: Data = {
        var data = Data(capacity: capacity)
        while data.count < capacity {
            var value = arc4random()
            data.append(UnsafeBufferPointer(start: &value, count: 1))
        }
        return data
    }()

    init(withSeed seed: Int) {
        srandom(UInt32(seed))
    }

    func data(withLength length: Int) -> Data {
        guard length < capacity else {
            fatalError()
        }
        if position + length > data.count {
            position = 0
        }
        let begin = data.startIndex.advanced(by: position)
        let end = begin.advanced(by: length)
        let subData = data.subdata(in: begin..<end)
        position += length
        return subData
    }
}
