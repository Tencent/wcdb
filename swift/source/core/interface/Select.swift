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

public class Select: SelectBase {
    private let keys: [CodingTableKeyBase]

    init(with core: Core, on propertyConvertibleList: [PropertyConvertible], table: String, isDistinct: Bool) throws {
        guard propertyConvertibleList.count > 0 else {
            throw Error.reportInterface(tag: core.tag,
                                        path: core.path,
                                        operation: .select,
                                        code: .misuse,
                                        message: "Selecting nothing from \(table) is invalid")
        }
        //TODO: Use generic to check all coding table keys conform to same root type
        keys = propertyConvertibleList.asCodingTableKeys()
        super.init(with: core)
        statement.select(distinct: isDistinct, propertyConvertibleList).from(table)
    }

    private func extract(from keys: [CodingTableKeyBase], of type: TableDecodableBase.Type) throws -> Any {
        let coreStatement = try self.lazyCoreStatement()
        let decoder = TableDecoder(keys, on: coreStatement)
        return try type.init(from: decoder)
    }

    private func extract<Object: TableDecodable>(from keys: [Object.CodingKeys]) throws -> Object {
        let coreStatement = try self.lazyCoreStatement()
        let decoder = TableDecoder(keys, on: coreStatement)
        return try Object.init(from: decoder)
    }

    public func nextObject() throws -> Any? {
        guard let rootType = keys[0].rootType as? TableDecodableBase.Type else {
            throw Error.reportInterface(tag: tag,
                                        path: path,
                                        operation: .select,
                                        code: .misuse,
                                        message: "")
        }
        guard try next() else {
            return nil
        }
        return try extract(from: keys, of: rootType)
    }

    public func allObjects() throws -> [Any] {
        guard let rootType = keys[0].rootType as? TableDecodableBase.Type else {
            throw Error.reportInterface(tag: tag,
                                        path: path,
                                        operation: .select,
                                        code: .misuse,
                                        message: "")
        }
        var objects: [Any] = []
        while try next() {
            objects.append(try extract(from: keys, of: rootType))
        }
        return objects
    }

    public func nextObject<Object: TableDecodable>(of type: Object.Type = Object.self) throws -> Object? {
        guard let keys = self.keys as? [Object.CodingKeys] else {
            throw Error.reportInterface(tag: tag,
                                        path: path,
                                        operation: .select,
                                        code: .misuse,
                                        message: "")
        }
        guard try next() else {
            return nil
        }
        return try extract(from: keys)
    }

    public func allObjects<Object: TableDecodable>(of type: Object.Type = Object.self) throws -> [Object] {
        guard let keys = self.keys as? [Object.CodingKeys] else {
            throw Error.reportInterface(tag: tag,
                                        path: path,
                                        operation: .select,
                                        code: .misuse,
                                        message: "")
        }
        var objects: [Object] = []
        while try next() {
            objects.append(try extract(from: keys))
        }
        return objects
    }
}
