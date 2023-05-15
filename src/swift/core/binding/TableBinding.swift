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
import WCDB_Private

public class TableBindingBase {
    private let recyclableBinding: Recyclable<CPPBinding>
    internal let cppBinding: CPPBinding
    public init() {
        self.cppBinding = WCDBBindingCreate()
        self.recyclableBinding = ObjectBridge.createRecyclableCPPObject(cppBinding)
    }
}

public final class TableBinding<CodingTableKeyType: CodingTableKey>: TableBindingBase {
    private let properties: [CodingTableKeyType: Property]
    private var configBuilder: (() -> [TableConfiguration])?

    public init(_ type: CodingTableKeyType.Type) {
        var allProperties: [Property] = []
        var properties: [CodingTableKeyType: Property] = [:]
        var allKeys: [CodingTableKeyType] = []
        var i = 0
        while true {
            guard let key = (withUnsafePointer(to: &i) {
                return $0.withMemoryRebound(to: CodingTableKeyType?.self, capacity: 1, { return $0.pointee })
            }) else {
                break
            }
            allKeys.append(key)
            i += 1
        }

        for key in allKeys {
            let property = Property(with: key)
            properties[key] = property
            allProperties.append(property)
        }

        self.allKeys = allKeys
        self.properties = properties
        self.allProperties = allProperties
        self.configBuilder = nil

        super.init()

        for property in allProperties {
            property.tableBinding = WCDBBindingGetBaseBinding(cppBinding)
        }

        if let tableDecodableType = CodingTableKeyType.Root.self as? TableDecodableBase.Type {
            let types = ColumnTypeDecoder.types(of: tableDecodableType)
            for key in self.allKeys {
                let columnType = types[key.stringValue]
                assert(columnType != nil, "It should not be failed. If you think it's a bug, please report an issue to us.")
                let columnDef = ColumnDef(named: key.rawValue, and: columnType!)
                withExtendedLifetime(columnDef) {
                    WCDBBindingAddColumnDef(cppBinding, $0.cppObj)
                }
            }

            #if DEBUG
            let keys = allKeys.filter({ (key) -> Bool in
                return types.index(forKey: key.stringValue) == nil
            })
            assert(keys.count == 0,
                   """
                   The following keys: \(keys) can't be decoded. \
                   1. Try to change their definition from `let` to `var` or report an issue to us. \
                   2. Try to rename the `static` variable with same name.
                   """)
            #endif
        }
    }

    @resultBuilder
    public struct TableConfigurationBuilder {
        public static func buildBlock(_ configs: TableConfiguration...) -> [TableConfiguration] {
            return configs
        }
        public static func buildBlock() -> [TableConfiguration] {
            return []
        }
    }

    public convenience init(_ type: CodingTableKeyType.Type, @TableConfigurationBuilder _ configBuilder: @escaping () -> [TableConfiguration]) {
        self.init(type)
        self.configBuilder = configBuilder
    }

    let allProperties: [Property]
    let allKeys: [CodingTableKeyType]

    private lazy var primaryKey: CodingTableKeyType? = {
        let filtered = allKeys.filter({ key in
            WCDBBindingIsPrimaryKey(cppBinding, key.rawValue.cString)
        })
        guard filtered.count == 1 else {
            assert(filtered.count == 0, "Only one primary key is supported. Use MultiPrimaryBinding instead")
            return nil
        }
        return filtered.first!
    }()

    internal lazy var innerBinding: CPPBinding = {
        if let configBuilder = configBuilder {
            let configs = configBuilder()
            for config in configs {
                config.config(with: self)
            }
        }
        return cppBinding
    }()

    typealias TypedCodingTableKeyType = CodingTableKeyType
    func property<CodingTableKeyType: CodingTableKey>(from codingTableKey: CodingTableKeyType) -> Property {
        let typedCodingTableKey = codingTableKey as? TypedCodingTableKeyType
        assert(typedCodingTableKey != nil, "[\(codingTableKey)] must conform to CodingTableKey protocol.")
        let typedProperty = properties[typedCodingTableKey!]
        assert(typedProperty != nil, "It should not be failed. If you think it's a bug, please report an issue to us.")
        return typedProperty!
    }

    func getPrimaryKey() -> CodingTableKeyBase? {
        return primaryKey
    }
}
