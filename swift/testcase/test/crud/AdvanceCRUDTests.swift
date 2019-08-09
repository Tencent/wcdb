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

class AdvanceCRUDTests: CRUDTestCase {

    func testSubthreadCheckpoint() {
        //Give
        let fileManager = self.fileManager
        let walFile = database.path+"-wal"
        let object = CRUDObject()
        object.variable2 = "testSubthreadCheckpoint"
        object.isAutoIncrement = true
        //When
        for _ in 0..<1000 {
            XCTAssertNoThrow(try database.insert(objects: object, intoTable: CRUDObject.name))
        }
        var expectedSize: Int64!
        do {
            let walSize = WCDBAssertNoThrowReturned(try fileManager.attributesOfItem(atPath: walFile)[.size])
            XCTAssertNotNil(walSize)
            let typedWalSize = walSize! as? Int64
            XCTAssertNotNil(typedWalSize)
            XCTAssertGreaterThan(typedWalSize!, 0)
            expectedSize = typedWalSize!
        }
        sleep(5)
        //Then
        do {
            let walSize = WCDBAssertNoThrowReturned(try fileManager.attributesOfItem(atPath: walFile)[.size])
            XCTAssertNotNil(walSize)
            let typedWalSize = walSize! as? Int64
            XCTAssertNotNil(typedWalSize)
            XCTAssertEqual(typedWalSize!, expectedSize)
        }
    }

    struct EncodableColumn: ColumnEncodable {
        static var columnType: ColumnType {
            return .integer64
        }

        func archivedValue() -> FundamentalValue {
            return FundamentalValue(Int64(variable1))
        }

        var variable1: Int = 0
    }

    struct DecodableColumn: ColumnDecodable {
        static var columnType: ColumnType {
            return .integer64
        }

        init?(with value: FundamentalValue) {
            variable1 = Int(truncatingIfNeeded: value.int64Value)
        }

        init() {}

        var variable1: Int = 0
    }

    class EncodableObject: TableEncodable {
        var variable1: EncodableColumn = EncodableColumn()
        enum CodingKeys: String, CodingTableKey {
            typealias Root = EncodableObject
            case variable1
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }
    }

    class DecodableObject: TableDecodable {
        var variable1: DecodableColumn = DecodableColumn()

        enum CodingKeys: String, CodingTableKey {
            typealias Root = DecodableObject
            case variable1
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }
    }

    func testPartialCodable() {
        let tableName = "testPartialCodable"
        XCTAssertNoThrow(try database.create(table: tableName,
                                             of: DecodableObject.self))

        let object = EncodableObject()
        XCTAssertNoThrow(try database.insert(objects: object,
                                             intoTable: tableName))

        let decodableObject: DecodableObject? = WCDBAssertNoThrowReturned(
            try database.getObject(fromTable: tableName)
        )
        XCTAssertNotNil(decodableObject)
        XCTAssertEqual(decodableObject!.variable1.variable1, object.variable1.variable1)
    }
}
