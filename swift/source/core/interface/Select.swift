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

public final class Select: Selectable {
    var core: Core
    let statement: StatementSelect
    var optionalCoreStatement: CoreStatement?

    /// The tag of the related database.
    public var tag: Tag? {
        return core.tag
    }

    /// The path of the related database.
    public var path: String {
        return core.path
    }

    private let keys: [CodingTableKeyBase]

    init(with core: Core, on propertyConvertibleList: [PropertyConvertible], table: String, isDistinct: Bool) throws {
        //TODO: Use generic to check all coding table keys conform to same root type
        keys = propertyConvertibleList.asCodingTableKeys()
        statement = StatementSelect().select(distinct: isDistinct, propertyConvertibleList).from(table)
        self.core = core
    }

    deinit {
        try? finalize()
    }

    func finalize() throws {
        if let coreStatement = optionalCoreStatement {
            try coreStatement.finalize()
            optionalCoreStatement = nil
        }
    }

    func lazyCoreStatement() throws -> CoreStatement {
        if optionalCoreStatement == nil {
            optionalCoreStatement = try core.prepare(statement)
        }
        return optionalCoreStatement!
    }

    private func extract(from keys: [CodingTableKeyBase], of type: TableDecodableBase.Type) throws -> Any {
        let coreStatement = try lazyCoreStatement()
        let decoder = TableDecoder(keys, on: coreStatement)
        return try type.init(from: decoder)
    }

    private func extract<Object: TableDecodable>(from keys: [Object.CodingKeys]) throws -> Object {
        let coreStatement = try lazyCoreStatement()
        let decoder = TableDecoder(keys, on: coreStatement)
        return try Object.init(from: decoder)
    }

    /// Get next selected object according to the `CodingTableKey`. You can do an iteration using it.
    ///
    /// - Returns: Table decodable object according to the `CodingTableKey`. Nil means the end of iteration.
    /// - Throws: `Error`
    public func nextObject() throws -> Any? {
        let rootType = keys[0].rootType as? TableDecodableBase.Type
        Error.assert(rootType != nil, message: "\(keys[0].rootType) must conform to TableDecodable protocol.")
        guard try next() else {
            return nil
        }
        return try extract(from: keys, of: rootType!)
    }

    /// Get all selected objects according to the `CodingTableKey`.
    ///
    /// - Returns: Table decodable objects according to the `CodingTableKey`
    /// - Throws: `Error`
    public func allObjects() throws -> [Any] {
        let rootType = keys[0].rootType as? TableDecodableBase.Type
        Error.assert(rootType != nil, message: "\(keys[0].rootType) must conform to TableDecodable protocol.")
        var objects: [Any] = []
        while try next() {
            objects.append(try extract(from: keys, of: rootType!))
        }
        return objects
    }

    /// Get next selected object with type. You can do an iteration using it.
    ///
    /// - Parameter type: Type of table decodable object
    /// - Returns: Table decodable object. Nil means the end of iteration.
    /// - Throws: `Error`
    public func nextObject<Object: TableDecodable>(of type: Object.Type = Object.self) throws -> Object? {
        let keys = self.keys as? [Object.CodingKeys]
        Error.assert(keys != nil, message: "Properties must belong to \(Object.self).CodingKeys.")
        guard try next() else {
            return nil
        }
        return try extract(from: keys!)
    }

    /// Get all selected objects.
    ///
    /// - Parameter type: Type of table decodable object
    /// - Returns: Table decodable objects.
    /// - Throws: `Error`
    public func allObjects<Object: TableDecodable>(of type: Object.Type = Object.self) throws -> [Object] {
        let keys = self.keys as? [Object.CodingKeys]
        Error.assert(keys != nil, message: "Properties must belong to \(Object.self).CodingKeys.")
        var objects: [Object] = []
        while try next() {
            objects.append(try extract(from: keys!))
        }
        return objects
    }
}
