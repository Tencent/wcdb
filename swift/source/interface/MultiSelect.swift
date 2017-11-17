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
public class MultiSelect: SelectBase {
    private let properties: [Property]
    
    init(with core: Core, on propertyConvertibleList: [PropertyConvertible], tables: [String], isDistinct: Bool = false) throws {
        guard propertyConvertibleList.count > 0 else {
            throw Error.reportInterface(tag: core.tag, path: core.path, operation: .Select, code: .Misuse, message: "Selecting nothing from \(tables) is invalid")
        }
        guard tables.count > 0 else {
            throw Error.reportInterface(tag: core.tag, path: core.path, operation: .Select, code: .Misuse, message: "Empty table")
        }
        self.properties = propertyConvertibleList.asProperties()
        for property in self.properties { 
            guard property.columnBinding != nil else {
                throw Error.reportInterface(tag: core.tag, path: core.path, operation: .Select, code: .Misuse, message: "This Result does not contain any column binding")
            }
        } 
        super.init(with: core)
        statementSelect.select(distinct: isDistinct, properties).from(tables)
    }
    
    public func nextMultiObject() throws -> [String:CodableTable]? {
        try next()
        return try extractMultiObject(from: properties)
    }
    
    public func allMultiObjects() throws -> [[String:CodableTable]] {
        var multiObjects: [[String:CodableTable]] = []
        while try next() {
            multiObjects.append(try extractMultiObject(from: properties))
        }
        return multiObjects
    }
}
