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
    private let properties: [Property]
    
    init(with core: Core, on propertyConvertibleList: [PropertyConvertible], table: String, isDistinct: Bool) throws {
        guard propertyConvertibleList.count > 0 else {
            throw Error.reportInterface(tag: core.tag, path: core.path, operation: .Select, code: .Misuse, message: "Selecting nothing from \(table) is invalid")
        }
        self.properties = propertyConvertibleList.asProperties()
        super.init(with: core)
        statement.select(distinct: isDistinct, properties).from(table)
    }
    
    public func nextObject<Object: CodableTable>(of type: Object.Type = Object.self) throws -> Object? {
        guard properties.first!.columnBinding.`class` is Object.Type else {
            Error.abort("")
        }
        guard try next() else {
            return nil
        }        
        return try extract(from: properties)
    }
    
    public func nextObject() throws -> Any? {
        guard try next() else {
            return nil
        }         
        return try extract(from: properties)
    }
    
    public func allObjects() throws -> [Any] {
        var objects: [Any] = []
        while try next() {
            let object = try extract(from: properties)
            objects.append(object)
        }
        return objects
    }
    
    public func allObjects<Object: CodableTable>(of type: Object.Type = Object.self) throws -> [Object] {
        assert((properties.first!.columnBinding.`class` == Object.self))
        var objects: [Object] = []
        while try next() {
            let object: Object = try extract(from: properties)
            objects.append(object)
        }
        return objects
    }
}

