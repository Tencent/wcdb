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
public class Insert: CoreRepresentable {
    let core: Core
    private var properties: [Property]?
    private let name: String
    private let isReplace: Bool
    
    init(with core: Core, named name: String, on propertyConvertibleList: [PropertyConvertible]?, isReplace: Bool = false) throws {
        guard name.count > 0 else {
            throw Error.reportInterface(tag: core.tag, path: core.path, operation: .Insert, code: .Misuse, message: "Empty table name")
        }
        self.name = name
        if propertyConvertibleList != nil {
            self.properties = propertyConvertibleList!.asProperties()
        }else {
            self.properties = nil
        }
        self.isReplace = isReplace
        self.core = core
    }
    
    private lazy var statement: StatementInsert = StatementInsert().insert(intoTable: name, with: properties!, onConflict: isReplace ? Conflict.Replace : Conflict.Fail).values(Array(repeating: Expression.bindingParameter, count: properties!.count))
    
    public func execute(with objects: CodableTable...) throws {
        try execute(with: objects)
    }
    
    public func execute(with objects: [CodableTable]) throws {
        guard objects.count > 0 else {
            Error.warning("Inserting with an empty/nil object")
            return
        }
        properties = properties ?? type(of: objects.first!).allProperties
        let cls = type(of: objects.first!)
        let orm = cls.objectRelationalMapping
        func doInsertObject() throws {
            let coreStatement = try core.prepare(statement) 
            for var object in objects {
                var fillLastInsertedRowID = false
                for (index, property) in properties!.enumerated() {
                    let bindingIndex = index + 1 
                    let columnBinding = property.columnBinding!
                    if !isReplace && columnBinding.isPrimary && columnBinding.isAutoIncrement && object.isAutoIncrement {
                        fillLastInsertedRowID = true
                        coreStatement.bind(nil, toIndex: bindingIndex)
                    }else {
                        coreStatement.bind(columnBinding, of: object, toIndex: bindingIndex)
                    }
                }
                try coreStatement.step()
                if fillLastInsertedRowID {
                    object.lastInsertedRowID = coreStatement.lastInsertedRowID
                }
                try coreStatement.reset()
            }
        }
        return objects.count == 1 ? try doInsertObject() : try core.run(embeddedTransaction: doInsertObject )
    }
}


