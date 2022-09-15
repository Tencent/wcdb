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

import XCTest
import WCDB

class TypedTests: CRUDTestCase {

    class TypedJSONCodableObject: ColumnJSONCodable, Equatable, CustomDebugStringConvertible {
        static func == (lhs: TypedJSONCodableObject, rhs: TypedJSONCodableObject) -> Bool {
            return lhs.variable == rhs.variable
        }

        var variable: Int = 1

        var debugDescription: String {
            return "\(variable)"
        }
    }

    enum TypedCodableObject: Int, ColumnCodable, Equatable, CustomDebugStringConvertible {
        case variable = 0

        static var columnType: ColumnType {
            return .integer64
        }

        func archivedValue() -> Value {
            return Value(Int64(self.rawValue))
        }

        init?(with value: Value) {
            guard let object = TypedCodableObject(rawValue: Int(truncatingIfNeeded: value.int64Value)) else {
                return nil
            }
            self = object
        }

        var debugDescription: String {
            return "\(self.rawValue)"
        }
    }

    class TypedTestObject: TableCodable, Named, Equatable, CustomDebugStringConvertible {
        var int: Int = 0
        var int8: Int8 = 0
        var int16: Int16 = 0
        var int32: Int32 = 0
        var int64: Int64 = 0
        var uint: UInt = 0
        var uint8: UInt8 = 0
        var uint16: UInt16 = 0
        var uint32: UInt32 = 0
        var uint64: UInt64 = 0
        var string: String = ""
        var float: Float = 0
        var double: Double = 0
        var data: Data = Data()
        var jsonCodable: TypedJSONCodableObject = TypedJSONCodableObject()
        var codable: TypedCodableObject = TypedCodableObject.variable
        var bool: Bool = false
        var date: Date = Date()
        var array: [Int] = []
        var dictionary: [Int: String] = [:]
        var set: Set<String> = []
        enum CodingKeys: String, CodingTableKey {
            typealias Root = TypedTestObject
            case int
            case int8
            case int16
            case int32
            case int64
            case uint
            case uint8
            case uint16
            case uint32
            case uint64
            case string
            case float
            case double
            case data
            case jsonCodable
            case codable
            case bool
            case date
            case array
            case dictionary
            case set = "db_set"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }

        enum TestType {
            case upperBoundary
            case lowerBoundary
        }

        init(with testType: TestType) {
            switch testType {
            case .upperBoundary:
                int = Int.max
                int8 = Int8.max
                int16 = Int16.max
                int32 = Int32.max
                int64 = Int64.max
                uint = UInt.max
                uint8 = UInt8.max
                uint16 = UInt16.max
                uint32 = UInt32.max
                uint64 = UInt64.max
                string = String(Array(repeating: "0", count: 1000))
                float = 0.123456789123456789123456789123456789123456789
                double = Double.pi
                data = string.data(using: .ascii)!
                jsonCodable = {
                    let object = TypedJSONCodableObject()
                    object.variable = Int.max
                    return object
                }()
                codable = .variable
                bool = true
                date = Date.distantFuture
                for i in 0..<10000 {
                    array.append(i)
                    dictionary[i] = "\(i)"
                    set.insert("\(i)")
                }
            case .lowerBoundary:
                int = Int.min
                int8 = Int8.min
                int16 = Int16.min
                int32 = Int32.min
                int64 = Int64.min
                uint = UInt.min
                uint8 = UInt8.min
                uint16 = UInt16.min
                uint32 = UInt32.min
                uint64 = UInt64.min
                string = ""
                float = 0
                double = 0
                data = Data()
                jsonCodable = {
                    let object = TypedJSONCodableObject()
                    object.variable = Int.min
                    return object
                }()
                codable = .variable
                bool = false
                date = Date.distantPast
                array.removeAll()
                dictionary.removeAll()
                set.removeAll()
            }
        }

        static func == (lhs: TypedTestObject, rhs: TypedTestObject) -> Bool {
            var result = lhs.int == rhs.int
            result = result && lhs.int8 == rhs.int8
            result = result && lhs.int16 == rhs.int16
            result = result && lhs.int32 == rhs.int32
            result = result && lhs.int64 == rhs.int64
            result = result && lhs.uint == rhs.uint
            result = result && lhs.uint8 == rhs.uint8
            result = result && lhs.uint16 == rhs.uint16
            result = result && lhs.uint32 == rhs.uint32
            result = result && lhs.uint64 == rhs.uint64
            result = result && lhs.string == rhs.string
            result = result && lhs.data == rhs.data
            result = result && lhs.float == rhs.float
            result = result && lhs.double == rhs.double
            result = result && lhs.jsonCodable == rhs.jsonCodable
            result = result && lhs.codable == rhs.codable
            result = result && lhs.bool == rhs.bool
            result = result && lhs.date == rhs.date
            result = result && lhs.array == rhs.array
            result = result && lhs.dictionary == rhs.dictionary
            result = result && lhs.set == rhs.set
            return result
        }
        var debugDescription: String {
            return """
            int: \(int)
            int8: \(int8)
            int16: \(int16)
            int32: \(int32)
            int64: \(int64)
            uint: \(uint)
            uint8: \(uint8)
            uint16: \(uint16)
            uint32: \(uint32)
            uint64: \(uint64)
            float: \(float)
            double: \(double)
            string: \(string)
            data: \(String(describing: data))
            jsonCodable: \(String(describing: jsonCodable))
            codable: \(String(describing: codable))
            bool: \(String(describing: bool))
            date: \(String(describing: date))
            array: \(String(describing: array))
            dictionary: \(String(describing: dictionary))
            set: \(String(describing: set))
            """
        }
    }

    func testUpperBoundary() {
        // Give
        XCTAssertNoThrow(try database.create(table: TypedTestObject.name, of: TypedTestObject.self))
        let object = TypedTestObject(with: .upperBoundary)
        XCTAssertNoThrow(try database.insert(object, intoTable: TypedTestObject.name))
        // When
        let selected: TypedTestObject? = WCDBAssertNoThrowReturned(
            try database.getObject(fromTable: TypedTestObject.name)
        )
        // Then
        XCTAssertNotNil(selected)
        XCTAssertEqual(selected!, object)
    }

    func testLowerBoundary() {
        // Give
        XCTAssertNoThrow(try database.create(table: TypedTestObject.name, of: TypedTestObject.self))
        let object = TypedTestObject(with: .lowerBoundary)
        XCTAssertNoThrow(try database.insert(object, intoTable: TypedTestObject.name))
        // When
        let selected: TypedTestObject? = WCDBAssertNoThrowReturned(
            try database.getObject(fromTable: TypedTestObject.name)
        )
        // Then
        XCTAssertNotNil(selected)
        XCTAssertEqual(selected!, object)
    }

    class TypedNonOverflowTestObject: TableCodable, Named, Equatable, CustomDebugStringConvertible {
        var int: Int = 0
        var int8: Int8 = 0
        var int16: Int16 = 0
        var int32: Int32 = 0
        var int64: Int64 = 0
        var uint: UInt = 0
        var uint8: UInt8 = 0
        var uint16: UInt16 = 0
        var uint32: UInt32 = 0
        var uint64: UInt64 = 0
        var string: String? = ""
        var float: Float = 0
        var double: Double = 0
        var data: Data? = Data()
        var jsonCodable: TypedJSONCodableObject? = TypedJSONCodableObject()
        var codable: TypedCodableObject? = TypedCodableObject.variable
        var bool: Bool? = false
        enum CodingKeys: String, CodingTableKey {
            typealias Root = TypedNonOverflowTestObject
            case int
            case int8
            case int16
            case int32
            case int64
            case uint
            case uint8
            case uint16
            case uint32
            case uint64
            case string
            case float
            case double
            case data
            case jsonCodable
            case codable
            case bool
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }

        enum TestType {
            case upperBoundary
            case lowerBoundary
        }

        static func == (lhs: TypedNonOverflowTestObject, rhs: TypedNonOverflowTestObject) -> Bool {
            var result = lhs.int == rhs.int
            result = result && lhs.int8 == rhs.int8
            result = result && lhs.int16 == rhs.int16
            result = result && lhs.int32 == rhs.int32
            result = result && lhs.int64 == rhs.int64
            result = result && lhs.uint == rhs.uint
            result = result && lhs.uint8 == rhs.uint8
            result = result && lhs.uint16 == rhs.uint16
            result = result && lhs.uint32 == rhs.uint32
            result = result && lhs.uint64 == rhs.uint64
            result = result && lhs.string == rhs.string
            result = result && lhs.data == rhs.data
            result = result && lhs.float == rhs.float
            result = result && lhs.double == rhs.double
            result = result && lhs.jsonCodable == rhs.jsonCodable
            result = result && lhs.codable == rhs.codable
            result = result && lhs.bool == rhs.bool
            return result
        }
        var debugDescription: String {
            return """
            int: \(int)
            int8: \(int8)
            int16: \(int16)
            int32: \(int32)
            int64: \(int64)
            uint: \(uint)
            uint8: \(uint8)
            uint16: \(uint16)
            uint32: \(uint32)
            uint64: \(uint64)
            float: \(float)
            double: \(double)
            string: \(string ?? "")
            data: \(String(describing: data))
            jsonCodable: \(String(describing: jsonCodable))
            codable: \(String(describing: codable))
            bool: \(String(describing: bool))
            """
        }
    }

    class TypedOverflowTestObject: TableCodable, CustomDebugStringConvertible {
        var int64Variable0: Int64 = Int64.max
        var int64Variable1: Int64 = Int64.max
        var int64Variable2: Int64 = Int64.max
        var int64Variable3: Int64 = Int64.max
        var int64Variable4: Int64 = Int64.max
        var int64Variable5: Int64 = Int64.max
        var int64Variable6: Int64 = Int64.max
        var int64Variable7: Int64 = Int64.max
        var int64Variable8: Int64 = Int64.max
        var int64Variable9: Int64 = Int64.max
        var doubleVariable0: Double = Double.pi
        var doubleVariable1: Double = Double.pi
        enum CodingKeys: String, CodingTableKey {
            typealias Root = TypedOverflowTestObject
            case int64Variable0 = "int"
            case int64Variable1 = "int8"
            case int64Variable2 = "int16"
            case int64Variable3 = "int32"
            case int64Variable4 = "int64"
            case int64Variable5 = "uint"
            case int64Variable6 = "uint8"
            case int64Variable7 = "uint16"
            case int64Variable8 = "uint32"
            case int64Variable9 = "uint64"
            case doubleVariable0 = "float"
            case doubleVariable1 = "double"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }

        enum TestType {
            case upperBoundary
            case lowerBoundary
        }

        var debugDescription: String {
            return """
            int64Variable0: \(int64Variable0)
            int64Variable1: \(int64Variable1)
            int64Variable2: \(int64Variable2)
            int64Variable3: \(int64Variable3)
            int64Variable4: \(int64Variable4)
            int64Variable5: \(int64Variable5)
            int64Variable6: \(int64Variable6)
            int64Variable7: \(int64Variable7)
            int64Variable8: \(int64Variable8)
            int64Variable9: \(int64Variable9)
            doubleVariable0: \(doubleVariable0)
            doubleVariable1: \(doubleVariable1)
            """
        }
    }

    func testOverflowed() {
        // Give
        XCTAssertNoThrow(try database.create(table: TypedNonOverflowTestObject.name,
                                             of: TypedNonOverflowTestObject.self))
        let overflow = TypedOverflowTestObject()
        XCTAssertNoThrow(try database.insert(overflow, intoTable: TypedNonOverflowTestObject.name))
        // When
        let selected: TypedNonOverflowTestObject? = WCDBAssertNoThrowReturned(
            try database.getObject(fromTable: TypedNonOverflowTestObject.name)
        )
        // Then
        XCTAssertNotNil(selected)
        let wrapped = selected!
        XCTAssertEqual(wrapped.int, Int(truncatingIfNeeded: overflow.int64Variable0))
        XCTAssertEqual(wrapped.int8, Int8(truncatingIfNeeded: overflow.int64Variable1))
        XCTAssertEqual(wrapped.int16, Int16(truncatingIfNeeded: overflow.int64Variable2))
        XCTAssertEqual(wrapped.int32, Int32(truncatingIfNeeded: overflow.int64Variable3))
        XCTAssertEqual(wrapped.int64, overflow.int64Variable4)
        XCTAssertEqual(wrapped.uint, UInt(truncatingIfNeeded: UInt64(bitPattern: overflow.int64Variable5)))
        XCTAssertEqual(wrapped.uint8, UInt8(truncatingIfNeeded: UInt64(bitPattern: overflow.int64Variable6)))
        XCTAssertEqual(wrapped.uint16, UInt16(truncatingIfNeeded: UInt64(bitPattern: overflow.int64Variable7)))
        XCTAssertEqual(wrapped.uint32, UInt32(truncatingIfNeeded: UInt64(bitPattern: overflow.int64Variable8)))
        XCTAssertEqual(wrapped.uint64, UInt64(bitPattern: overflow.int64Variable9))
        XCTAssertEqual(wrapped.float, Float(overflow.doubleVariable0))
        XCTAssertEqual(wrapped.double, overflow.doubleVariable1)
    }

    class OptionalTypedTestObject: TableCodable, Named, Equatable, CustomDebugStringConvertible {
        var int: Int?
        var int8: Int8?
        var int16: Int16?
        var int32: Int32?
        var int64: Int64?
        var uint: UInt?
        var uint8: UInt8?
        var uint16: UInt16?
        var uint32: UInt32?
        var uint64: UInt64?
        var string: String?
        var float: Float?
        var double: Double?
        var data: Data?
        var jsonCodable: TypedJSONCodableObject?
        var codable: TypedCodableObject?
        var bool: Bool?
        enum CodingKeys: String, CodingTableKey {
            typealias Root = OptionalTypedTestObject
            case int
            case int8
            case int16
            case int32
            case int64
            case uint
            case uint8
            case uint16
            case uint32
            case uint64
            case string
            case float
            case double
            case data
            case jsonCodable
            case codable
            case bool
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }

        enum TestType {
            case null
            case zero
            case normal
        }

        init(with testType: TestType) {
            switch testType {
            case .null:
                int = nil
                int8 = nil
                int16 = nil
                int32 = nil
                int64 = nil
                uint = nil
                uint8 = nil
                uint16 = nil
                uint32 = nil
                uint64 = nil
                string = nil
                float = nil
                double = nil
                data = nil
                jsonCodable = nil
                codable = nil
                bool = nil
            case .zero:
                int = 0
                int8 = 0
                int16 = 0
                int32 = 0
                int64 = 0
                uint = 0
                uint8 = 0
                uint16 = 0
                uint32 = 0
                uint64 = 0
                string = ""
                float = 0
                double = 0
                data = Data()
                jsonCodable = {
                    let object = TypedJSONCodableObject()
                    object.variable = 0
                    return object
                }()
                codable = .variable
                bool = false
            case .normal:
                int = 1
                int8 = 2
                int16 = 3
                int32 = 4
                int64 = 5
                uint = 6
                uint8 = 7
                uint16 = 8
                uint32 = 9
                uint64 = 10
                string = "11"
                float = 12.13
                double = 14.15
                data = "16".data(using: .ascii)
                jsonCodable = {
                    let object = TypedJSONCodableObject()
                    object.variable = 17
                    return object
                }()
                codable = .variable
                bool = true
            }
        }

        static func == (lhs: OptionalTypedTestObject, rhs: OptionalTypedTestObject) -> Bool {
            var result = lhs.int == rhs.int
            result = result && lhs.int8 == rhs.int8
            result = result && lhs.int16 == rhs.int16
            result = result && lhs.int32 == rhs.int32
            result = result && lhs.int64 == rhs.int64
            result = result && lhs.uint == rhs.uint
            result = result && lhs.uint8 == rhs.uint8
            result = result && lhs.uint16 == rhs.uint16
            result = result && lhs.uint32 == rhs.uint32
            result = result && lhs.uint64 == rhs.uint64
            result = result && lhs.string == rhs.string
            result = result && lhs.data == rhs.data
            result = result && lhs.float == rhs.float
            result = result && lhs.double == rhs.double
            result = result && lhs.jsonCodable == rhs.jsonCodable
            result = result && lhs.codable == rhs.codable
            result = result && lhs.bool == rhs.bool
            return result
        }
        var debugDescription: String {
            return """
            int: \(String(describing: int))
            int8: \(String(describing: int8))
            int16: \(String(describing: int16))
            int32: \(String(describing: int32))
            int64: \(String(describing: int64))
            uint: \(String(describing: uint))
            uint8: \(String(describing: uint8))
            uint16: \(String(describing: uint16))
            uint32: \(String(describing: uint32))
            uint64: \(String(describing: uint64))
            float: \(String(describing: float))
            double: \(String(describing: double))
            string: \(String(describing: string))
            data: \(String(describing: data))
            jsonCodable: \(String(describing: jsonCodable))
            codable: \(String(describing: codable))
            bool: \(String(describing: bool))
            """
        }
    }

    func testNull() {
        // Give
        XCTAssertNoThrow(try database.create(table: OptionalTypedTestObject.name, of: OptionalTypedTestObject.self))
        let object = OptionalTypedTestObject(with: .null)
        XCTAssertNoThrow(try database.insert(object, intoTable: OptionalTypedTestObject.name))
        // When
        let selected: OptionalTypedTestObject? = WCDBAssertNoThrowReturned(
            try database.getObject(fromTable: OptionalTypedTestObject.name)
        )
        // Then
        XCTAssertNotNil(selected)
        XCTAssertEqual(selected!, object)
    }

    func testZero() {
        // Give
        XCTAssertNoThrow(try database.create(table: OptionalTypedTestObject.name, of: OptionalTypedTestObject.self))
        let object = OptionalTypedTestObject(with: .zero)
        XCTAssertNoThrow(try database.insert(object, intoTable: OptionalTypedTestObject.name))
        // When
        let selected: OptionalTypedTestObject? = WCDBAssertNoThrowReturned(
            try database.getObject(fromTable: OptionalTypedTestObject.name)
        )
        // Then
        XCTAssertNotNil(selected)
        XCTAssertEqual(selected!, object)
    }

    func testPartialNil() {
        // Give
        XCTAssertNoThrow(try database.create(table: OptionalTypedTestObject.name, of: OptionalTypedTestObject.self))
        let object = OptionalTypedTestObject(with: .normal)
        XCTAssertNoThrow(try database.insert(object, intoTable: OptionalTypedTestObject.name))
        let table = WCDBAssertNoThrowReturned(try database.getTable(named: OptionalTypedTestObject.name,
                                                                    of: OptionalTypedTestObject.self))
        XCTAssertNotNil(table)
        let wrappedTable = table!
        // Then
        do {
            do {
                let result = try wrappedTable.getObject(on: OptionalTypedTestObject.Properties.int)
                XCTAssertNotNil(object)
                let wrappedResult = result!
                XCTAssertEqual(wrappedResult.int, object.int)
                XCTAssertNil(wrappedResult.int8)
                XCTAssertNil(wrappedResult.int16)
                XCTAssertNil(wrappedResult.int32)
                XCTAssertNil(wrappedResult.int64)
                XCTAssertNil(wrappedResult.uint)
                XCTAssertNil(wrappedResult.uint8)
                XCTAssertNil(wrappedResult.uint16)
                XCTAssertNil(wrappedResult.uint32)
                XCTAssertNil(wrappedResult.uint64)
                XCTAssertNil(wrappedResult.string)
                XCTAssertNil(wrappedResult.float)
                XCTAssertNil(wrappedResult.double)
                XCTAssertNil(wrappedResult.data)
                XCTAssertNil(wrappedResult.jsonCodable)
                XCTAssertNil(wrappedResult.codable)
                XCTAssertNil(wrappedResult.bool)
            }
            do {
                let result = try wrappedTable.getObject(on: OptionalTypedTestObject.Properties.int8)
                XCTAssertNotNil(object)
                let wrappedResult = result!
                XCTAssertEqual(wrappedResult.int8, object.int8)
                XCTAssertNil(wrappedResult.int)
                XCTAssertNil(wrappedResult.int16)
                XCTAssertNil(wrappedResult.int32)
                XCTAssertNil(wrappedResult.int64)
                XCTAssertNil(wrappedResult.uint)
                XCTAssertNil(wrappedResult.uint8)
                XCTAssertNil(wrappedResult.uint16)
                XCTAssertNil(wrappedResult.uint32)
                XCTAssertNil(wrappedResult.uint64)
                XCTAssertNil(wrappedResult.string)
                XCTAssertNil(wrappedResult.float)
                XCTAssertNil(wrappedResult.double)
                XCTAssertNil(wrappedResult.data)
                XCTAssertNil(wrappedResult.jsonCodable)
                XCTAssertNil(wrappedResult.codable)
                XCTAssertNil(wrappedResult.bool)
            }
        } catch let error {
            XCTFail(error.localizedDescription)
        }
    }

    func testPartialInsert() {
        // Give
        XCTAssertNoThrow(try database.create(table: TypedTestObject.name, of: TypedTestObject.self))
        let object = TypedTestObject(with: .lowerBoundary)
        XCTAssertNoThrow(try database.insert(object,
                                             on: [TypedTestObject.Properties.int],
                                             intoTable: TypedTestObject.name))
        let table = WCDBAssertNoThrowReturned(
            try database.getTable(named: TypedTestObject.name,
                                  of: OptionalTypedTestObject.self)
        )
        XCTAssertNotNil(table)
        let wrappedTable = table!
        // Then
        do {
            do {
                let result = try wrappedTable.getObject()
                XCTAssertNotNil(object)
                let wrappedResult = result!
                XCTAssertEqual(wrappedResult.int, object.int)
                XCTAssertNil(wrappedResult.int8)
                XCTAssertNil(wrappedResult.int16)
                XCTAssertNil(wrappedResult.int32)
                XCTAssertNil(wrappedResult.int64)
                XCTAssertNil(wrappedResult.uint)
                XCTAssertNil(wrappedResult.uint8)
                XCTAssertNil(wrappedResult.uint16)
                XCTAssertNil(wrappedResult.uint32)
                XCTAssertNil(wrappedResult.uint64)
                XCTAssertNil(wrappedResult.string)
                XCTAssertNil(wrappedResult.float)
                XCTAssertNil(wrappedResult.double)
                XCTAssertNil(wrappedResult.data)
                XCTAssertNil(wrappedResult.jsonCodable)
                XCTAssertNil(wrappedResult.codable)
                XCTAssertNil(wrappedResult.bool)
            }
        } catch let error {
            XCTFail(error.localizedDescription)
        }
    }

    func testPartialInsertOptional() {
        // Give
        XCTAssertNoThrow(try database.create(table: OptionalTypedTestObject.name, of: OptionalTypedTestObject.self))
        let object = OptionalTypedTestObject(with: .normal)
        XCTAssertNoThrow(try database.insert(object,
                                             on: [OptionalTypedTestObject.Properties.int],
                                             intoTable: OptionalTypedTestObject.name))
        let table = WCDBAssertNoThrowReturned(
            try database.getTable(named: OptionalTypedTestObject.name,
                                  of: OptionalTypedTestObject.self)
        )
        XCTAssertNotNil(table)
        let wrappedTable = table!
        // Then
        do {
            do {
                let result = try wrappedTable.getObject()
                XCTAssertNotNil(object)
                let wrappedResult = result!
                XCTAssertEqual(wrappedResult.int, object.int)
                XCTAssertNil(wrappedResult.int8)
                XCTAssertNil(wrappedResult.int16)
                XCTAssertNil(wrappedResult.int32)
                XCTAssertNil(wrappedResult.int64)
                XCTAssertNil(wrappedResult.uint)
                XCTAssertNil(wrappedResult.uint8)
                XCTAssertNil(wrappedResult.uint16)
                XCTAssertNil(wrappedResult.uint32)
                XCTAssertNil(wrappedResult.uint64)
                XCTAssertNil(wrappedResult.string)
                XCTAssertNil(wrappedResult.float)
                XCTAssertNil(wrappedResult.double)
                XCTAssertNil(wrappedResult.data)
                XCTAssertNil(wrappedResult.jsonCodable)
                XCTAssertNil(wrappedResult.codable)
                XCTAssertNil(wrappedResult.bool)
            }
        } catch let error {
            XCTFail(error.localizedDescription)
        }
    }

    class TypedNoMatchJSONCodableObject: ColumnJSONCodable {
        var noMatchVariable: Int = 1
    }

    class TypedMatchTable: TableCodable {
        var variable1 = TypedJSONCodableObject()
        enum CodingKeys: String, CodingTableKey {
            typealias Root = TypedMatchTable
            case variable1
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }
    }

    class TypedNoMatchTable: TableCodable {
        var variable1 = TypedNoMatchJSONCodableObject()
        enum CodingKeys: String, CodingTableKey {
            typealias Root = TypedNoMatchTable
            case variable1
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }
    }

    func testNoMatchJSONCodableObject() {
        // Give
        let tableName = "testNoMatchJSONCodableObject"
        XCTAssertNoThrow(try database.create(table: tableName, of: TypedMatchTable.self))

        let typedMatchTable = TypedMatchTable()
        typedMatchTable.variable1 = TypedJSONCodableObject()
        typedMatchTable.variable1.variable = 1
        XCTAssertNoThrow(try database.insert(typedMatchTable, intoTable: tableName))

        var `catch` = false
        do {
            let _: TypedNoMatchTable? = try database.getObject(fromTable: tableName)
            XCTFail("should not reach")
        } catch {
            `catch` = true
        }
        XCTAssertTrue(`catch`)
    }

    func testEmptyJSONCodableObject() {
        // Give
        let tableName = "testEmptyJSONCodableObject"
        XCTAssertNoThrow(try database.create(table: tableName, of: TypedMatchTable.self))

        let statementInsert = StatementInsert().insert(intoTable: tableName).columns(TypedMatchTable.Properties.variable1)
                                               .values(BindParameter(1))
        let handle = try? database.getHandle()
        XCTAssertNotNil(handle)
        let wrappedHandle = handle!
        XCTAssertNoThrow(try wrappedHandle.prepare(statementInsert))
        wrappedHandle.bind("", toIndex: 1)
        XCTAssertNoThrow(try wrappedHandle.step())
        XCTAssertNoThrow(wrappedHandle.finalize())

        var `catch` = false
        do {
            let _: TypedMatchTable? = try database.getObject(fromTable: tableName)
            XCTFail("should not reach")
        } catch {
            `catch` = true
        }
        XCTAssertTrue(`catch`)
    }
}
