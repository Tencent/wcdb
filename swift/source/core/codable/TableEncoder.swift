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


class TableEncoder: Encoder {
    private class KeyedEncodingTableContainer<CodingTableKeyType: CodingKey>: KeyedEncodingContainerProtocol {
        typealias Key = CodingTableKeyType
        
        let codingPath: [CodingKey] = []
        
        let coreStatement: CoreStatement
        let indexedCodingTableKeys: [String:Int]
        let primaryKey: CodingTableKeyBase?
        
        init(with indexedCodingTableKeys: [String:Int], on coreStatement: CoreStatement, and primaryKey: CodingTableKeyBase?, keyType: CodingTableKeyType.Type) {
            self.indexedCodingTableKeys = indexedCodingTableKeys
            self.coreStatement = coreStatement
            self.primaryKey = primaryKey
        }
        
        func bindingIndex(by codingTableKey: Key) -> Int? {
            return indexedCodingTableKeys[codingTableKey.stringValue]
        }
        
        func superEncoder() -> Swift.Encoder {
            Error.abort("")
        }
        
        func superEncoder(forKey key: Key) -> Swift.Encoder {
            Error.abort("")
        }
        
        func encodePrimaryKeyIfPresent(forKey key: Key, atIndex index: Int) -> Bool {
            guard key.stringValue == primaryKey?.stringValue else {
                return false
            }
            coreStatement.bind(nil, toIndex: index)
            return true
        }
        
        func encodeNil(forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            coreStatement.bind(nil, toIndex: bindingIndex)
        }
        
        func encode(_ value: Int, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: Bool, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: Float, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: Double, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: String, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encodeConditional<Object>(_ object: Object, forKey key: Key) throws where Object : AnyObject, Object : Encodable {
            try encode(object, forKey: key)
        }
        
        func encode<Object>(_ value: Object, forKey key: Key) throws where Object : Encodable {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                if let encodableColumnValue = value as? ColumnEncodableBase {
                    var bound = false
                    let fundamentalValue = encodableColumnValue.archivedFundamentalValue()
                    switch type(of: encodableColumnValue).columnType {
                    case .Integer32:
                        if let wrappedFundamentalValue = fundamentalValue as? Int32 {
                            coreStatement.bind(wrappedFundamentalValue, toIndex: bindingIndex)
                            bound = true
                        }
                    case .Integer64:
                        if let wrappedFundamentalValue = fundamentalValue as? Int64 {
                            coreStatement.bind(wrappedFundamentalValue, toIndex: bindingIndex)
                            bound = true
                        }
                    case .Float:
                        if let wrappedFundamentalValue = fundamentalValue as? Double {
                            coreStatement.bind(wrappedFundamentalValue, toIndex: bindingIndex)
                            bound = true
                        }
                    case .Text:
                        if let wrappedFundamentalValue = fundamentalValue as? String {
                            coreStatement.bind(wrappedFundamentalValue, toIndex: bindingIndex)
                            bound = true
                        }
                    case .BLOB:
                        if let wrappedFundamentalValue = fundamentalValue as? Data {
                            coreStatement.bind(wrappedFundamentalValue, toIndex: bindingIndex)
                            bound = true
                        }
                    default: break
                    }
                    if !bound {
                        coreStatement.bind(nil, toIndex: bindingIndex)
                    }
                }else {
                    var data: Data? = nil
                    switch Object.defaultTableEncoder {
                    case .NSCoder:
                        data = NSKeyedArchiver.archivedData(withRootObject: value)
                    case .JSON:
                        data = try JSONEncoder().encode(value)
                    }
                    if let wrappedData = data {
                        coreStatement.bind(wrappedData, toIndex: bindingIndex)
                    }else {
                        coreStatement.bind(nil, toIndex: bindingIndex)
                    }
                }
            }
        }
        
        func encode(_ value: Int8, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: Int16, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: Int32, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: Int64, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: UInt, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: UInt8, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: UInt16, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: UInt32, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func encode(_ value: UInt64, forKey key: Key) throws {
            guard let bindingIndex = bindingIndex(by: key) else {
                return
            }
            if !encodePrimaryKeyIfPresent(forKey: key, atIndex: bindingIndex) {
                coreStatement.bind(value, toIndex: bindingIndex)
            }
        }
        
        func nestedContainer<NestedKey>(keyedBy keyType: NestedKey.Type, forKey key: Key) -> KeyedEncodingContainer<NestedKey> where NestedKey : CodingKey {
            Error.abort("")
        }
        
        func nestedUnkeyedContainer(forKey key: Key) -> UnkeyedEncodingContainer {
            Error.abort("")
        }
    }
    
    let codingPath: [CodingKey] = []
    let userInfo: [CodingUserInfoKey:Any] = [:]
    
    private let coreStatement: CoreStatement
    private let indexedCodingTableKeys: [String:Int]
    private var primaryKey: CodingTableKeyBase? = nil
    
    func singleValueContainer() -> SingleValueEncodingContainer {
        Error.abort("")
    }
    
    func unkeyedContainer() -> UnkeyedEncodingContainer {
        Error.abort("")
    }
    
    func container<Key>(keyedBy type: Key.Type) -> KeyedEncodingContainer<Key> where Key : CodingKey {
        guard Key.self is CodingTableKeyBase.Type else {
            Error.abort("")
        }
        return KeyedEncodingContainer(KeyedEncodingTableContainer(with: indexedCodingTableKeys, on: coreStatement, and: primaryKey, keyType: Key.self))
    }
    
    convenience init(_ codingTableKeys: [CodingTableKeyBase], on coreStatement: CoreStatement) {
        var indexedCodingTableKeys: [String:Int] = [:]
        for (index, codingTableKey) in codingTableKeys.enumerated() {
            indexedCodingTableKeys[codingTableKey.stringValue] = index + 1
        }
        self.init(indexedCodingTableKeys, on: coreStatement)
    }
    
    init(_ indexedCodingTableKeys: [String:Int], on coreStatement: CoreStatement) {
        self.indexedCodingTableKeys = indexedCodingTableKeys
        self.coreStatement = coreStatement
    }

    func bind<TableEncodableType: TableEncodable>(_ object: TableEncodableType, isReplace: Bool = false) throws {
        if !isReplace && object.isAutoIncrement {
            primaryKey = TableEncodableType.CodingKeys.__objectRelationalMapping.getPrimaryKey()
        }
        try object.encode(to: self)
        primaryKey = nil
    }
}
