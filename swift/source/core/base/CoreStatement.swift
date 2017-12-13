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
public final class CoreStatement: CoreRepresentable {
    let core: Core
    let recyclableHandleStatement: RecyclableHandleStatement
    public var tag: Tag? {
        return core.tag
    }
    public var path: String {
        return core.path
    }

    init(with core: Core, and recyclableHandleStatement: RecyclableHandleStatement) {
        self.core = core
        self.recyclableHandleStatement = recyclableHandleStatement
    }

    private var handleStatement: HandleStatement {
        return recyclableHandleStatement.raw
    }

    public func bind<TableEncodableType: TableEncodable>(
        _ propertyConvertible: PropertyConvertible,
        of object: TableEncodableType,
        toIndex index: Int = 1) throws {
        try bind([(propertyConvertible, toIndex: index)], of: object)
    }

    public func bind<TableEncodableType: TableEncodable>(
        _ indexedPropertyConvertibleList: [(_: PropertyConvertible, toIndex: Int)],
        of object: TableEncodableType) throws {
        var indexedCodingTableKeys: [String: Int] = [:]
        for args in indexedPropertyConvertibleList {
            indexedCodingTableKeys[args.0.codingTableKey.stringValue] = args.toIndex
        }
        let encoder = TableEncoder(indexedCodingTableKeys, on: self)
        try encoder.bind(object)
    }

    public func bind<TableEncodableType: TableEncodable>(
        _ propertyConvertibleList: [PropertyConvertible],
        of object: TableEncodableType) throws {
        var indexedCodingTableKeys: [String: Int] = [:]
        for (index, propertyConvertible) in propertyConvertibleList.enumerated() {
            indexedCodingTableKeys[propertyConvertible.codingTableKey.stringValue] = index
        }
        let encoder = TableEncoder(indexedCodingTableKeys, on: self)
        try encoder.bind(object)
    }

    public func bind(_ value: ColumnEncodableBase?, toIndex index: Int) {
        if let wrappedValue = value {
            let fundamentalValue = wrappedValue.archivedFundamentalValue()
            switch type(of: wrappedValue).columnType {
            case .integer32:
                let int32Value = (fundamentalValue as? Int32) ?? 0
                handleStatement.bind(int32Value, toIndex: index)
            case .integer64:
                let int64Value = (fundamentalValue as? Int64) ?? 0
                handleStatement.bind(int64Value, toIndex: index)
            case .text:
                let stringValue = (fundamentalValue as? String) ?? ""
                handleStatement.bind(stringValue, toIndex: index)
            case .float:
                let doubleValue = (fundamentalValue as? Double) ?? 0
                handleStatement.bind(doubleValue, toIndex: index)
            case .BLOB:
                let dataValue = (fundamentalValue as? Data) ?? Data()
                handleStatement.bind(dataValue, toIndex: index)
            case .null:
                handleStatement.bind(nil, toIndex: index)
            }
        } else {
            handleStatement.bind(nil, toIndex: index)
        }
    }

    public func value(atIndex index: Int, of type: ColumnDecodableBase.Type) -> ColumnDecodableBase? {
        guard handleStatement.columnType(atIndex: index) != .null else {
            return nil
        }
        switch type.columnType {
        case .integer32:
            let value: Int32 = handleStatement.columnValue(atIndex: index)
            return type.init(with: value)
        case .integer64:
            let value: Int64 = handleStatement.columnValue(atIndex: index)
            return type.init(with: value)
        case .float:
            let value: Double = handleStatement.columnValue(atIndex: index)
            return type.init(with: value)
        case .text:
            let value: String = handleStatement.columnValue(atIndex: index)
            return type.init(with: value)
        case .BLOB:
            let value: Data = handleStatement.columnValue(atIndex: index)
            return type.init(with: value)
        default: break
        }
        return nil
    }

    public func value<ColumnDecodableType: ColumnDecodable>(
        atIndex index: Int,
        of type: ColumnDecodableType.Type = ColumnDecodableType.self) -> ColumnDecodableType? {
        guard handleStatement.columnType(atIndex: index) != .null else {
            return nil
        }
        var result: ColumnDecodableType? = nil
        switch ColumnDecodableType.columnType {
        case .integer32:
            let value: Int32 = handleStatement.columnValue(atIndex: index)
            result = ColumnDecodableType.init(with: value)
        case .integer64:
            let value: Int64 = handleStatement.columnValue(atIndex: index)
            result = ColumnDecodableType.init(with: value)
        case .float:
            let value: Double = handleStatement.columnValue(atIndex: index)
            result = ColumnDecodableType.init(with: value)
        case .text:
            let value: String = handleStatement.columnValue(atIndex: index)
            result = ColumnDecodableType.init(with: value)
        case .BLOB:
            let value: Data = handleStatement.columnValue(atIndex: index)
            result = ColumnDecodableType.init(with: value)
        default: break
        }
        return result
    }

    public func value(atIndex index: Int) -> FundamentalValue {
        switch handleStatement.columnType(atIndex: index) {
        case .integer32:
            let value: Int32 = handleStatement.columnValue(atIndex: index)
            return FundamentalValue(value)
        case .integer64:
            let value: Int64 = handleStatement.columnValue(atIndex: index)
            return FundamentalValue(value)
        case .float:
            let value: Double = handleStatement.columnValue(atIndex: index)
            return FundamentalValue(value)
        case .text:
            let value: String = handleStatement.columnValue(atIndex: index)
            return FundamentalValue(value)
        case .BLOB:
            let value: Data = handleStatement.columnValue(atIndex: index)
            return FundamentalValue(value)
        case .null:
            return FundamentalValue(nil)
        }
    }

    public func value<ColumnDecodableType: ColumnDecodable>(
        byName name: String,
        of type: ColumnDecodableType.Type = ColumnDecodableType.self) -> ColumnDecodableType? {
        guard let index = index(byName: name) else {
            return nil
        }
        return value(atIndex: index)
    }

    public func index(byName name: String) -> Int? {
        for index in 0..<handleStatement.columnCount() {
            if handleStatement.columnName(atIndex: index)==name {
                return index
            }
        }
        return nil
    }

    @discardableResult
    public func step() throws -> Bool {
        return try handleStatement.step()
    }

    public func reset() throws {
        try handleStatement.reset()
    }

    public func columnType(atIndex index: Int) -> ColumnType {
        return handleStatement.columnType(atIndex: index)
    }

    public func columnType(byName name: String) -> ColumnType {
        guard let index = index(byName: name) else {
            return .null
        }
        return columnType(atIndex: index)
    }

    public func columnCount() -> Int {
        return handleStatement.columnCount()
    }

    public func columnName(atIndex index: Int) -> String {
        return handleStatement.columnName(atIndex: index)
    }

    public func columnTableName(atIndex index: Int) -> String {
        return handleStatement.columnTableName(atIndex: index)
    }

    public func finalize() throws {
        try handleStatement.finalize()
    }

    public var changes: Int {
        return handleStatement.changes
    }

    public var lastInsertedRowID: Int64 {
        return handleStatement.lastInsertedRowID
    }
}
