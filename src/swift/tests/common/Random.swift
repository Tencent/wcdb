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

    static let wordArray: [String] = {
        let wordFrequency: [String: Int] = [
            "a": 408,
            "he": 121,
            "but": 86,
            "my": 58,
            "if": 51,
            "just": 44,
            "people": 33,
            "think": 28,
            "because": 25,
            "two": 21,
            "very": 19,
            "should": 17,
            "work": 15,
            "day": 14,
            "oh": 13,
            "use": 11,
            "lot": 10,
            "mr": 10,
            "part": 9,
            "without": 8
        ]
        var wordArray: [String] = []
        wordArray.reserveCapacity(1000)
        for (word, frequency) in wordFrequency {
            for _ in 0..<frequency {
                wordArray.append(word)
            }
        }
        return wordArray
    }()

    public static func englishString(withLength length: Int) -> String {
        var ret = ""
        for _ in 0..<length {
            ret += wordArray[Int(arc4random_uniform(UInt32(1000)))]
            ret += " "
        }
        return ret
    }

    nonisolated(unsafe) public static var useEnglishString = false

    public static func testObjects(startWith id: Int, count: Int) -> [TestObject] {
        var objects: [TestObject] = []
        for i in 0..<count {
            let object = TestObject()
            object.variable1 = id + i
            if !useEnglishString {
                object.variable2 = string(withLength: 100)
            } else {
                object.variable2 = englishString(withLength: 100)
            }
            objects.append(object)
        }
        return objects
    }
}
