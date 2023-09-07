//
// Created by qiuwenchen on 2023/8/15.
//

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

class Random {
    public static func string(withLength length: Int) -> String {
        let letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        return String((0..<length).map { _ in letters.randomElement()! })
    }

    public static func data(withLength length: Int, andSeed seed: Int = 0) -> Data {
        srandom(UInt32(seed))
        var data = Data(capacity: length)
        while data.count < length {
            var value = arc4random()
            withUnsafePointer(to: &value) {
                data.append(UnsafeBufferPointer(start: $0, count: 1))
            }
        }
        return data
    }

    public static func testObjects(startWith id: Int, count: Int) -> [TestObject] {
        var objects: [TestObject] = []
        for i in 0..<count {
            let object = TestObject()
            object.variable1 = id + i
            object.variable2 = string(withLength: 100)
            objects.append(object)
        }
        return objects
    }
}
