//
// Created by qiuwenchen on 2022/11/13.
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

#if WCDB_SWIFT_BRIDGE_OBJC

import Foundation
import WCDB_Private

public protocol StatementInterfaceForObjc {
    /// The wrapper of `sqlite3_bind_*` for binding property of object to index.
    ///
    /// - Parameters:
    ///   - propertyConvertible: `Property` or `CodingTableKey`
    ///   - object: Table encodable object
    ///   - index: Begin with 1
    /// - Throws: `Error`
    func bind<TableEncodableType: WCTTableCoding>(
        _ propertyConvertible: PropertyConvertible,
        of object: TableEncodableType,
        toIndex index: Int) throws

    /// The wrapper of `sqlite3_bind_*` for binding properties of object to indexes.
    ///
    /// - Parameters:
    ///   - indexedPropertyConvertibleList: Indexed `Property` or `CodingTableKey` list
    ///   - object: Table encodable object
    /// - Throws: Begin with 1
    func bind<TableEncodableType: WCTTableCoding>(
        _ indexedPropertyConvertibleList: [(_: PropertyConvertible, toIndex: Int)],
        of object: TableEncodableType) throws

    /// The wrapper of `sqlite3_bind_*` for binding properties of object.
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - object: Table encodable object
    /// - Throws: Begin with 1
    func bind<TableEncodableType: WCTTableCoding>(
        _ propertyConvertibleList: [PropertyConvertible],
        of object: TableEncodableType) throws

    /// The wrapper of `sqlite3_column_*` for getting  an object.
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - type: Type of table decodable object
    /// - Returns: Table decodable object.
    func extractObject<Object: WCTTableCoding>(on propertyConvertibleList: [PropertyConvertible]?, of type: Object.Type) -> Object

    /// The wrapper of `sqlite3_column_*` for getting  all objects.
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - type: Type of table decodable object
    /// - Returns: Table decodable objects.
    func extractAllObjects<Object: WCTTableCoding>(on propertyConvertibleList: [PropertyConvertible]?, of type: Object.Type) throws-> [Object]
}

extension StatementInterfaceForObjc where Self: RawStatementmentRepresentable {
    public func bind<TableEncodableType: WCTTableCoding>(
        _ propertyConvertible: PropertyConvertible,
        of object: TableEncodableType,
        toIndex index: Int = 1) throws {
        try bind([(propertyConvertible, toIndex: index)], of: object)
    }

    public func bind<TableEncodableType: WCTTableCoding>(
        _ indexedPropertyConvertibleList: [(_: PropertyConvertible, toIndex: Int)],
        of object: TableEncodableType) throws {
        for args in indexedPropertyConvertibleList {
            assert(args.0.wctProperty != nil, "WCTProperty should not be failed. If you think it's a bug, please report an issue to us.")
            WCTAPIBridge.bindProperty(args.0.wctProperty!, ofObject: object, to: Int32(truncatingIfNeeded: args.toIndex), with: getRawStatement())
        }
    }

    public func bind<TableEncodableType: WCTTableCoding>(
        _ propertyConvertibleList: [PropertyConvertible],
        of object: TableEncodableType) throws {
        WCTAPIBridge.bindProperties(propertyConvertibleList.asWCTBridgeProperties(), ofObject: object, with: getRawStatement())
    }

    public func extractObject<Object: WCTTableCoding>(on propertyConvertibleList: [PropertyConvertible]? = nil, of type: Object.Type = Object.self) -> Object {
        let properties: [WCTBridgeProperty] = propertyConvertibleList?.asWCTBridgeProperties() ?? Object.allProperties()
        return WCTAPIBridge.extractObject(onResultColumns: properties, from: getRawStatement()) as! Object
    }

    public func extractAllObjects<Object: WCTTableCoding>(on propertyConvertibleList: [PropertyConvertible]? = nil, of type: Object.Type = Object.self) throws-> [Object] {
        let properties: [WCTBridgeProperty] = propertyConvertibleList?.asWCTBridgeProperties() ?? Object.allProperties()
        var objects: [Object] = []
        let rawStatment = getRawStatement()
        if !WCDBHandleStatementStep(rawStatment) {
            let cppError = WCDBHandleStatementGetError(rawStatment)
            if finalizeWhenError() {
                WCDBHandleStatementFinalize(getRawStatement())
            }
            throw ErrorBridge.getErrorFrom(cppError: cppError)
        }
        while !WCDBHandleStatementIsDone(rawStatment) {
            objects.append(WCTAPIBridge.extractObject(onResultColumns: properties, from: getRawStatement()) as! Object)
            if !WCDBHandleStatementStep(rawStatment) {
                let cppError = WCDBHandleStatementGetError(rawStatment)
                throw ErrorBridge.getErrorFrom(cppError: cppError)
            }
        }
        return objects
    }
}

#endif
