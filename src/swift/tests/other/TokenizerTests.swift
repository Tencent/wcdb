//
// Created by qiuwenchen on 2022/11/6.
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

import XCTest
#if TEST_WCDB_SWIFT
import WCDBSwift
#else
import WCDB
#endif

class TokenizerTests: DatabaseTestCase {

    class CustomTokenizer: Tokenizer {

        var hasCheckParameter: Bool = false
        var input: UnsafePointer<Int8>?
        var inputLength: Int = 0
        var lastLocation: Int = 0
        var position: Int = 0

        static let fts3Name = "fts3_custom_tokenizer"
        static let fts5Name = "fts5_custom_tokenizer"
        static let testParameter1 = "testParameter1"
        static let testParameter2 = "testParameter2"

        required init(args: [String]) {
            if args.contains(where: { arg in
                return arg == CustomTokenizer.testParameter1
            }), args.contains(where: { arg in
                return arg == CustomTokenizer.testParameter2
            }) {
                hasCheckParameter = true
            }
        }

        func load(input: UnsafePointer<Int8>?, length: Int, flags: Int) {
            self.input = input
            self.inputLength = length
            self.lastLocation = 0
            self.position = 0
        }

        func nextToken(ppToken: UnsafeMutablePointer<UnsafePointer<Int8>?>,
                       pnBytes: UnsafeMutablePointer<Int32>,
                       piStart: UnsafeMutablePointer<Int32>,
                       piEnd: UnsafeMutablePointer<Int32>,
                       pFlags: UnsafeMutablePointer<Int32>?,
                       piPosition: UnsafeMutablePointer<Int32>?) -> TokenizerErrorCode {
            guard hasCheckParameter, inputLength > lastLocation, let input = input else {
                return .Done
            }
            var start: Int = lastLocation
            var end: Int = lastLocation

            for location in lastLocation..<inputLength {
                if input.advanced(by: location).pointee == 32 /* space */ {
                    if start == end {
                        start = location + 1
                        end = start
                    } else {
                        break
                    }
                } else {
                    end = location + 1
                }
            }
            if start == end {
                return .Done
            }
            lastLocation = end
            ppToken.pointee = input.advanced(by: start)
            pnBytes.pointee = Int32(end - start)
            piStart.pointee = Int32(start)
            piEnd.pointee = Int32(end)
            position += 1
            if let piPosition = piPosition {
                piPosition.pointee = Int32(position)
            }
            return.OK
        }
    }

    final class FTS3Object: TableCodable, Named {
        var content: String = ""

        enum CodingKeys: String, CodingTableKey {
            typealias Root = FTS3Object
            case content
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindVirtualTable(withModule: .FTS3, and: CustomTokenizer.fts3Name, CustomTokenizer.testParameter1, CustomTokenizer.testParameter2)
            }
        }
    }

    func testCustomFTS3Tokenizer() {
        // Give
        Database.register(tokenizer: CustomTokenizer.self, of: CustomTokenizer.fts3Name, of: .FTS3)
        self.database.add(tokenizer: CustomTokenizer.fts3Name)
        let tableName = FTS3Object.name
        let obj = FTS3Object()
        obj.content = "  test content wcdb custom tokenizer  "
        // When
        XCTAssertNoThrow(try self.database.create(virtualTable: tableName, of: FTS3Object.self))
        XCTAssertNoThrow(try self.database.insert(obj, intoTable: tableName))
        // Then
        let matchObj: FTS3Object? = WCDBAssertNoThrowReturned(try self.database.getObject(on: FTS3Object.Properties.all, fromTable: tableName, where: FTS3Object.Properties.content.match("custom content")))
        XCTAssertNotNil(matchObj)
        XCTAssertEqual(matchObj!.content, obj.content)
    }

    final class FTS5Object: TableCodable, Named {
        var content: String = ""

        enum CodingKeys: String, CodingTableKey {
            typealias Root = FTS5Object
            case content
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindVirtualTable(withModule: .FTS5, and: CustomTokenizer.fts5Name, CustomTokenizer.testParameter1, CustomTokenizer.testParameter2)
            }
        }
    }

    func testCustomFTS5Tokenizer() {
        // Give
        Database.register(tokenizer: CustomTokenizer.self, of: CustomTokenizer.fts5Name, of: .FTS5)
        self.database.add(tokenizer: CustomTokenizer.fts5Name)
        let tableName = FTS5Object.name
        let obj = FTS5Object()
        obj.content = "  test content wcdb custom tokenizer  "
        // When
        XCTAssertNoThrow(try self.database.create(virtualTable: tableName, of: FTS5Object.self))
        XCTAssertNoThrow(try self.database.insert(obj, intoTable: tableName))
        // Then
        let matchObj: FTS5Object? = WCDBAssertNoThrowReturned(try self.database.getObject(on: FTS5Object.Properties.all, fromTable: tableName, where: FTS5Object.Properties.content.match("custom content")))
        XCTAssertNotNil(matchObj)
        XCTAssertEqual(matchObj!.content, obj.content)
    }
}
