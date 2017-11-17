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
public class CoreStatement: CoreRepresentable {
    let core: Core
    let recyclableHandleStatement: RecyclableHandleStatement
    
    init(with core: Core, and recyclableHandleStatement: RecyclableHandleStatement) {
        self.core = core
        self.recyclableHandleStatement = recyclableHandleStatement
    }
    
    private var handleStatement: HandleStatement {
        return recyclableHandleStatement.raw
    }
    
    public func bind(_ value: ColumnCodingBase?, toIndex index: Int) {
        if value != nil {
            let cls = Swift.type(of: value!)
            let fundamentalValue = value!.archivedFundamentalValue()
            switch cls.columnType {
            case .Integer32:
                handleStatement.bind(fundamentalValue as! Int32, toIndex: index)
            case .Integer64:
                handleStatement.bind(fundamentalValue as! Int64, toIndex: index)
            case .Text:
                handleStatement.bind(fundamentalValue as! String, toIndex: index)
            case .Float:
                handleStatement.bind(fundamentalValue as! Double, toIndex: index)
            case .BLOB:
                handleStatement.bind(fundamentalValue as! Data, toIndex: index)
            case .Null:
                handleStatement.bind(nil, toIndex: index)
            }
        }else {
            handleStatement.bind(nil, toIndex: index)
        }
    }
       
    func bind(_ columnBinding: AnyColumnBinding, of object: TableCoding, toIndex index: Int) {
        switch columnBinding.columnType {
        case .Integer32:
            let value: Int32? = columnBinding.access(getFundamentalValueFromObject: object)
            if value != nil {
                handleStatement.bind(value!, toIndex: index)
            }else {
                handleStatement.bind(nil, toIndex: index)
            }
        case .Integer64:
            let value: Int64? = columnBinding.access(getFundamentalValueFromObject: object)
            if value != nil {
                handleStatement.bind(value!, toIndex: index)
            }else {
                handleStatement.bind(nil, toIndex: index)
            }
        case .Text:
            let value: String? = columnBinding.access(getFundamentalValueFromObject: object)
            if value != nil {
                handleStatement.bind(value!, toIndex: index)
            }else {
                handleStatement.bind(nil, toIndex: index)
            }
        case .Float:
            let value: Double? = columnBinding.access(getFundamentalValueFromObject: object)
            if value != nil {
                handleStatement.bind(value!, toIndex: index)
            }else {
                handleStatement.bind(nil, toIndex: index)
            }
        case .BLOB:
            let value: Data? = columnBinding.access(getFundamentalValueFromObject: object)
            if value != nil {
                handleStatement.bind(value!, toIndex: index)
            }else {
                handleStatement.bind(nil, toIndex: index)
            }
        case .Null:
            handleStatement.bind(nil, toIndex: index)
        }
    }
    
    public func value(atIndex index: Int) -> Int32 {
        return handleStatement.columnValue(atIndex: index)
    }
    
    public func value(atIndex index: Int) -> Int64 {
        return handleStatement.columnValue(atIndex: index)
    }
    
    public func value(atIndex index: Int) -> String {
        return handleStatement.columnValue(atIndex: index)
    }
    public func value(atIndex index: Int) -> Double {
        return handleStatement.columnValue(atIndex: index)
    }
    public func value(atIndex index: Int) -> Data {
        return handleStatement.columnValue(atIndex: index)
    }
    
    public func value(atIndex index: Int) -> OneValue {
        switch handleStatement.columnType(atIndex: index) {
        case .Integer32:
            let value: Int32 = handleStatement.columnValue(atIndex: index) 
            return value
        case .Integer64:
            let value: Int64 = handleStatement.columnValue(atIndex: index) 
            return value
        case .Text:
            let value: String = handleStatement.columnValue(atIndex: index) 
            return value
        case .Float:
            let value: Double = handleStatement.columnValue(atIndex: index) 
            return value
        case .BLOB:
            let value: Data = handleStatement.columnValue(atIndex: index) 
            return value
        case .Null:
            return nil
        }        
    }
    
    public func value(byName name: String) -> OneValue {
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
    
    public func type(atIndex index: Int) -> ColumnType {
        return handleStatement.columnType(atIndex: index)
    }
    
    public func type(byName name: String) -> ColumnType {
        guard let index = index(byName: name) else {
            return .Null
        }
        return handleStatement.columnType(atIndex: index)
    }
    
    public func columnCount() -> Int{
        return handleStatement.columnCount()
    }
    
    public func columnName(atIndex index: Int) -> String{
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

