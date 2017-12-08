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
import WCDBSwift

class TypedTests: CRUDTestCase {
    
    class TypedJSONCodableObject: ColumnJSONCodable, Equatable, CustomDebugStringConvertible {
        static func ==(lhs: TypedJSONCodableObject, rhs: TypedJSONCodableObject) -> Bool {
            return lhs.variable == rhs.variable
        }
        
        var variable: Int = 1
        
        var debugDescription: String {
            return "\(variable)"
        }
    }

    struct TypedNSCodableObject: ColumnJSONCodable, Equatable, CustomDebugStringConvertible {
        static func ==(lhs: TypedNSCodableObject, rhs: TypedNSCodableObject) -> Bool {
            return lhs.variable == rhs.variable
        }
        
        var variable: Int = 1
        
        var debugDescription: String {
            return "\(variable)"
        }
    }
    
    enum TypedCodableObject: Int, ColumnCodable, Equatable, CustomDebugStringConvertible {
        case variable = 0
        
        typealias FundamentalType = Int64
        
        func archivedValue() -> Int64? {
            return Int64(self.rawValue)
        }

        init?(with value: Int64) {
            guard let object = TypedCodableObject(rawValue: Int(truncatingIfNeeded: value)) else {
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
        var string: String? = ""
        var float: Float = 0
        var double: Double = 0
        var data: Data? = Data()
        var jsonCodable: TypedJSONCodableObject? = TypedJSONCodableObject()
        var nsCodable: TypedNSCodableObject? = TypedNSCodableObject()
        var codable: TypedCodableObject? = TypedCodableObject.variable
        required init() {}
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
            case nsCodable
            case codable
            static let __objectRelationalMapping = TableBinding(CodingKeys.self)
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
                data = string!.data(using: .ascii)!
                jsonCodable = {
                    let object = TypedJSONCodableObject()
                    object.variable = Int.max
                    return object
                }()
                nsCodable = {
                    var object = TypedNSCodableObject()
                    object.variable = Int.max
                    return object
                }()
                codable = .variable
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
                nsCodable = {
                    var object = TypedNSCodableObject()
                    object.variable = Int.min
                    return object
                }()
                codable = .variable
            }
        }
        
        static func ==(lhs: TypedTestObject, rhs: TypedTestObject) -> Bool {
            return lhs.int == rhs.int 
                && lhs.int8 == rhs.int8 
                && lhs.int16 == rhs.int16
                && lhs.int32 == rhs.int32
                && lhs.int64 == rhs.int64
                && lhs.uint == rhs.uint
                && lhs.uint8 == rhs.uint8
                && lhs.uint16 == rhs.uint16
                && lhs.uint32 == rhs.uint32
                && lhs.uint64 == rhs.uint64
                && lhs.string == rhs.string
                && lhs.data == rhs.data
                && lhs.float == rhs.float
                && lhs.double == rhs.double
                && lhs.jsonCodable == rhs.jsonCodable
                && lhs.codable == rhs.codable
                && lhs.nsCodable == rhs.nsCodable
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
            nsCodable: \(String(describing: nsCodable))
            codable: \(String(describing: codable))
            """
        }
    }
    
    func testUpperBoundary() {
        //Give
        XCTAssertNoThrow(try database.create(table: TypedTestObject.name, of: TypedTestObject.self))        
        let object = TypedTestObject(with: .upperBoundary)
        XCTAssertNoThrow(try database.insert(objects: object, intoTable: TypedTestObject.name))
        //When
        let selected: TypedTestObject? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: TypedTestObject.name))
        //Then
        XCTAssertNotNil(selected)
        XCTAssertEqual(selected!, object)
    }
    
    func testLowerBoundary() {
        //Give
        XCTAssertNoThrow(try database.create(table: TypedTestObject.name, of: TypedTestObject.self))        
        let object = TypedTestObject(with: .lowerBoundary)
        XCTAssertNoThrow(try database.insert(objects: object, intoTable: TypedTestObject.name))
        //When
        let selected: TypedTestObject? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: TypedTestObject.name))
        //Then
        XCTAssertNotNil(selected)
        XCTAssertEqual(selected!, object)
    }
    
    class TypedOverflowTestObject: TableCodable, CustomDebugStringConvertible {
        var int64_0: Int64 = Int64.max
        var int64_1: Int64 = Int64.max
        var int64_2: Int64 = Int64.max
        var int64_3: Int64 = Int64.max
        var int64_4: Int64 = Int64.max
        var int64_5: Int64 = Int64.max
        var int64_6: Int64 = Int64.max
        var int64_7: Int64 = Int64.max
        var int64_8: Int64 = Int64.max
        var int64_9: Int64 = Int64.max
        var double_0: Double = Double.pi
        var double_1: Double = Double.pi
        required init() {}
        enum CodingKeys: String, CodingTableKey {
            typealias Root = TypedOverflowTestObject
            case int64_0 = "int"
            case int64_1 = "int8"
            case int64_2 = "int16"
            case int64_3 = "int32"
            case int64_4 = "int64"
            case int64_5 = "uint"
            case int64_6 = "uint8"
            case int64_7 = "uint16"
            case int64_8 = "uint32"
            case int64_9 = "uint64"
            case double_0 = "float"
            case double_1 = "double"
            static let __objectRelationalMapping = TableBinding(CodingKeys.self)
        }
        
        enum TestType {
            case upperBoundary
            case lowerBoundary
        }
        
        var debugDescription: String {
            return """
            int64_0: \(int64_0)
            int64_1: \(int64_1)
            int64_2: \(int64_2)
            int64_3: \(int64_3)
            int64_4: \(int64_4)
            int64_5: \(int64_5)
            int64_6: \(int64_6)
            int64_7: \(int64_7)
            int64_8: \(int64_8)
            int64_9: \(int64_9)
            double_0: \(double_0)
            double_1: \(double_1)
            """
        }
    }

    func testOverflowed() {
        //Give
        XCTAssertNoThrow(try database.create(table: TypedTestObject.name, of: TypedTestObject.self))        
        let overflow = TypedOverflowTestObject()
        XCTAssertNoThrow(try database.insert(objects: overflow, intoTable: TypedTestObject.name))
        //When
        let selected: TypedTestObject? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: TypedTestObject.name))
        //Then
        XCTAssertNotNil(selected)
        let wrapped = selected!
        XCTAssertEqual(wrapped.int, Int(truncatingIfNeeded: overflow.int64_0))
        XCTAssertEqual(wrapped.int8, Int8(truncatingIfNeeded: overflow.int64_1))
        XCTAssertEqual(wrapped.int16, Int16(truncatingIfNeeded: overflow.int64_2))
        XCTAssertEqual(wrapped.int32, Int32(truncatingIfNeeded: overflow.int64_3))
        XCTAssertEqual(wrapped.int64, overflow.int64_4)
        XCTAssertEqual(wrapped.uint, UInt(truncatingIfNeeded: UInt64(bitPattern: overflow.int64_5)))
        XCTAssertEqual(wrapped.uint8, UInt8(truncatingIfNeeded: UInt64(bitPattern: overflow.int64_6)))
        XCTAssertEqual(wrapped.uint16, UInt16(truncatingIfNeeded: UInt64(bitPattern: overflow.int64_7)))
        XCTAssertEqual(wrapped.uint32, UInt32(truncatingIfNeeded: UInt64(bitPattern: overflow.int64_8)))
        XCTAssertEqual(wrapped.uint64, UInt64(bitPattern: overflow.int64_9))
        XCTAssertEqual(wrapped.float, Float(overflow.double_0))
        XCTAssertEqual(wrapped.double, overflow.double_1)
    }
    
    class OptionalTypedTestObject: TableCodable, Named, Equatable, CustomDebugStringConvertible {
        var int: Int? = nil
        var int8: Int8? = nil
        var int16: Int16? = nil
        var int32: Int32? = nil
        var int64: Int64? = nil
        var uint: UInt? = nil
        var uint8: UInt8? = nil
        var uint16: UInt16? = nil
        var uint32: UInt32? = nil
        var uint64: UInt64? = nil
        var string: String? = nil
        var float: Float? = nil
        var double: Double? = nil
        var data: Data? = nil
        var jsonCodable: TypedJSONCodableObject? = nil
        var nsCodable: TypedNSCodableObject? = nil
        var codable: TypedCodableObject? = nil
        required init() {}
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
            case nsCodable
            case codable
            static let __objectRelationalMapping = TableBinding(CodingKeys.self)
        }
        
        enum TestType {
            case null
            case zero
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
                nsCodable = nil
                codable = nil
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
                nsCodable = {
                    var object = TypedNSCodableObject()
                    object.variable = 0
                    return object
                }()
                codable = .variable
            }
        }
        
        static func ==(lhs: OptionalTypedTestObject, rhs: OptionalTypedTestObject) -> Bool {
            return lhs.int == rhs.int 
                && lhs.int8 == rhs.int8 
                && lhs.int16 == rhs.int16
                && lhs.int32 == rhs.int32
                && lhs.int64 == rhs.int64
                && lhs.uint == rhs.uint
                && lhs.uint8 == rhs.uint8
                && lhs.uint16 == rhs.uint16
                && lhs.uint32 == rhs.uint32
                && lhs.uint64 == rhs.uint64
                && lhs.string == rhs.string
                && lhs.data == rhs.data
                && lhs.float == rhs.float
                && lhs.double == rhs.double
                && lhs.jsonCodable == rhs.jsonCodable
                && lhs.codable == rhs.codable
                && lhs.nsCodable == rhs.nsCodable
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
            nsCodable: \(String(describing: nsCodable))
            codable: \(String(describing: codable))
            """
        }
    }

    func testNull() {
        //Give
        XCTAssertNoThrow(try database.create(table: OptionalTypedTestObject.name, of: OptionalTypedTestObject.self))        
        let object = OptionalTypedTestObject(with: .null)
        XCTAssertNoThrow(try database.insert(objects: object, intoTable: OptionalTypedTestObject.name))
        //When
        let selected: OptionalTypedTestObject? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: OptionalTypedTestObject.name))
        //Then
        XCTAssertNotNil(selected)
        XCTAssertEqual(selected!, object)
    }

    func testZero() {
        //Give
        XCTAssertNoThrow(try database.create(table: OptionalTypedTestObject.name, of: OptionalTypedTestObject.self))        
        let object = OptionalTypedTestObject(with: .zero)
        XCTAssertNoThrow(try database.insert(objects: object, intoTable: OptionalTypedTestObject.name))
        //When
        let selected: OptionalTypedTestObject? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: OptionalTypedTestObject.name))
        //Then
        XCTAssertNotNil(selected)
        XCTAssertEqual(selected!, object)
    }    
}

