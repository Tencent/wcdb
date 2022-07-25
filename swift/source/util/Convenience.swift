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

internal extension Array {
    internal func joined(_ map: (Element) -> String, separateBy separator: String = ", ") -> String {
        var flag = false
        return reduce(into: "") { (output, element) in
            if flag {
                output.append(separator)
            } else {
                flag = true
            }
            output.append(map(element))
        }
    }
}

internal extension Array where Element: StringProtocol {
    internal func joined(separateBy separator: String = ", ") -> String {
        var flag = false
        return reduce(into: "") { (output, element) in
            if flag {
                output.append(separator)
            } else {
                flag = true
            }
            output.append(String(element))
        }
    }
}

internal extension Array where Element: Describable {
    internal func joined(separateBy separator: String = ", ") -> String {
        return joined({ $0.description }, separateBy: separator)
    }
}

internal extension Array where Element==ColumnResultConvertible {
    internal func joined(separateBy separator: String = ", ") -> String {
        return joined({ $0.asColumnResult().description }, separateBy: separator)
    }
}

internal extension Array where Element==ExpressionConvertible {
    internal func joined(separateBy separator: String = ", ") -> String {
        return joined({ $0.asExpression().description }, separateBy: separator)
    }
}

internal extension Array where Element==ColumnConvertible {
    internal func joined(separateBy separator: String = ", ") -> String {
        return joined({ $0.asColumn().description }, separateBy: separator)
    }
}

internal extension Array where Element==TableOrSubqueryConvertible {
    internal func joined(separateBy separator: String = ", ") -> String {
        return joined({ $0.asTableOrSubquery().description }, separateBy: separator)
    }
}

internal extension Array where Element==OrderConvertible {
    internal func joined(separateBy separator: String = ", ") -> String {
        return joined({ $0.asOrder().description }, separateBy: separator)
    }
}

internal extension Array where Element==ColumnIndexConvertible {
    internal func joined(separateBy separator: String = ", ") -> String {
        return joined({ $0.asIndex().description }, separateBy: separator)
    }

    internal func asIndexes() -> [ColumnIndex] {
        return map { $0.asIndex() }
    }
}

internal extension Array where Element==PropertyConvertible {
    internal func asCodingTableKeys() -> [CodingTableKeyBase] {
        return reduce(into: [CodingTableKeyBase]()) { (result, element) in
            result.append(element.codingTableKey)
        }
    }
}

internal extension Array {
    internal mutating func expand(toNewSize newSize: Int, fillWith value: Iterator.Element) {
        if count < newSize {
            append(contentsOf: repeatElement(value, count: count.distance(to: newSize)))
        }
    }
}

internal extension Array where Iterator.Element: FixedWidthInteger {
    internal mutating func expand(toNewSize newSize: Int) {
        expand(toNewSize: newSize, fillWith: 0)
    }
}

internal extension Dictionary {
    internal func joined(_ map: (Key, Value) -> String, separateBy separator: String = "," ) -> String {
        var flag = false
        return reduce(into: "", { (output, arg) in
            if flag {
                output.append(separator)
            } else {
                flag = true
            }
            output.append(map(arg.key, arg.value))
        })
    }
}

internal extension String {
    internal var lastPathComponent: String {
        return URL(fileURLWithPath: self).lastPathComponent
    }

    internal func stringByAppending(pathComponent: String) -> String {
        return URL(fileURLWithPath: self).appendingPathComponent(pathComponent).path
    }

    internal var cString: UnsafePointer<Int8>? {
        return UnsafePointer<Int8>((self as NSString).utf8String)
    }

    internal init?(bytes: UnsafeRawPointer, count: Int, encoding: String.Encoding) {
        self.init(data: Data(bytes: bytes, count: count), encoding: encoding)
    }

    internal func range(from begin: Int, to end: Int) -> Range<String.Index> {
        return index(startIndex, offsetBy: begin)..<index(startIndex, offsetBy: end)
    }

    internal func range(location: Int, length: Int) -> Range<String.Index> {
        return range(from: location, to: location + length)
    }
}

internal extension Bool {
    @inline(__always) internal func toInt32() -> Int32 {
        return self ? 1 : 0
    }
}

internal extension Int {
    @inline(__always) internal func toInt64() -> Int64 {
        return Int64(self)
    }
}

internal extension Int8 {
    @inline(__always) internal func toInt32() -> Int32 {
        return Int32(self)
    }
}

internal extension Int16 {
    @inline(__always) internal func toInt32() -> Int32 {
        return Int32(self)
    }
}

internal extension Int32 {
    @inline(__always) internal func toBool() -> Bool {
        return self != 0
    }
    @inline(__always) internal func toInt8() -> Int8 {
        return Int8(truncatingIfNeeded: self)
    }
    @inline(__always) internal func toInt16() -> Int16 {
        return Int16(truncatingIfNeeded: self)
    }
    @inline(__always) internal func toUInt8() -> UInt8 {
        return UInt8(bitPattern: Int8(truncatingIfNeeded: self))
    }
    @inline(__always) internal func toUInt16() -> UInt16 {
        return UInt16(bitPattern: Int16(truncatingIfNeeded: self))
    }
    @inline(__always) internal func toUInt32() -> UInt32 {
        return UInt32(bitPattern: self)
    }
}

internal extension Int64 {
    @inline(__always) internal func toInt() -> Int {
        return Int(truncatingIfNeeded: self)
    }
    @inline(__always) internal func toUInt() -> UInt {
        return UInt(bitPattern: Int(truncatingIfNeeded: self))
    }
    @inline(__always) internal func toUInt64() -> UInt64 {
        return UInt64(bitPattern: self)
    }
}

internal extension UInt {
    @inline(__always) internal func toInt64() -> Int64 {
        return Int64(bitPattern: UInt64(self))
    }
}

internal extension UInt8 {
    @inline(__always) internal func toInt32() -> Int32 {
        return Int32(bitPattern: UInt32(self))
    }
}

internal extension UInt16 {
    @inline(__always) internal func toInt32() -> Int32 {
        return Int32(bitPattern: UInt32(self))
    }
}

internal extension UInt32 {
    @inline(__always) internal func toInt32() -> Int32 {
        return Int32(bitPattern: self)
    }
}

internal extension UInt64 {
    @inline(__always) internal func toInt64() -> Int64 {
        return Int64(bitPattern: self)
    }
}

internal extension Float {
    @inline(__always) internal func toDouble() -> Double {
        return Double(self)
    }
}

internal extension Double {
    @inline(__always) internal func toFloat() -> Float {
        return Float(self)
    }
}
