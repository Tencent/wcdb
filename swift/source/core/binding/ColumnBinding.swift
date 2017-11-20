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

public protocol ColumnBindingRepresentable {
    var columnBinding: AnyColumnBinding {get}
}

public class AnyColumnBinding : AnyBinding {
    typealias AnyAccessor = AnyKeyPath
    
    let `class`: AnyClass
    let columnType: ColumnType
    let columnName: String
    let isPrimary: Bool = false
    let isAutoIncrement: Bool = false
    let accessor: AnyAccessor
    lazy var expression = columnName
    let columnDef: ColumnDef
    
    init(accessor: AnyAccessor, class: AnyClass, columnName: String, columnType: ColumnType, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        self.columnName = columnName
        self.columnType = columnType
        var columnDef = ColumnDef(with: Column(named: columnName), and: columnType)
        if isPrimary {
            columnDef = columnDef.makePrimary(orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict)
        }
        if isNotNull {
            columnDef = columnDef.makeNotNull()
        }
        if isUnique {
            columnDef = columnDef.makeUnique()
        }
        if defaultValue != nil {
            columnDef = columnDef.makeDefault(to: defaultValue!)
        }
        self.columnDef = columnDef
        self.`class` = `class`
        self.accessor = accessor
        super.init(with: .Column)
    }
    
    convenience init(accessor: AnyAccessor, class: AnyClass, columnName: String, columnType: ColumnType, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: CodableColumnBase?) {
        self.init(accessor: accessor, class: `class`, columnName: columnName, columnType: columnType, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: AnyColumnBinding.defaultType(from: defaultValue))
    }
    
    static func defaultType(from defaultValue: CodableColumnBase?) -> ColumnDef.DefaultType {
        var defaultType = ColumnDef.DefaultType.Null
        if defaultValue != nil {
            let cls = Swift.type(of: defaultValue!)
            if let fundamentalValue = defaultValue!.archivedFundamentalValue() {
                switch cls.columnType {
                case .Integer32:
                    defaultType = .Int32(fundamentalValue as! Int32)
                case .Integer64:
                    defaultType = .Int64(fundamentalValue as! Int64)
                case .Float:
                    defaultType = .Float(fundamentalValue as! Double)
                case .Text:
                    defaultType = .Text(fundamentalValue as! String)
                case .BLOB:
                    defaultType = .BLOB(fundamentalValue as! Data)
                case .Null:
                    defaultType = .Null
                }
            }
        }
        return defaultType
    }

    func access<FundamentalType: CodableFundamentalValue>(getFundamentalValueFromObject object: CodableTable) -> FundamentalType? {
        Error.abort("This function is not implemented")
    }
    
    func access<FundamentalType: CodableFundamentalValue>(setFundamentalValue value: FundamentalType?, forObject object: inout CodableTable) {
        Error.abort("This function is not implemented")
    }
}

public class PartialColumnBinding<ModelType: CodableTable> : AnyColumnBinding {
    typealias PartialAccessor = PartialKeyPath<ModelType>
    
    init(accessor: PartialAccessor, columnName: String, columnType: ColumnType, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        super.init(accessor: accessor, class: ModelType.self as! AnyClass, columnName: columnName, columnType: columnType, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: defaultValue)
    }
    
    convenience init(accessor: PartialAccessor, columnName: String, columnType: ColumnType, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: CodableColumnBase?) {
        self.init(accessor: accessor, columnName: columnName, columnType: columnType, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: AnyColumnBinding.defaultType(from: defaultValue))
    }
    
    func access<PropertyType: CodableColumn>(setValue value: PropertyType?, forObject object: inout ModelType) {
        guard !(value == nil && accessor is WritableKeyPath<ModelType, PropertyType>) else {
            return
        }
        if accessor is WritableKeyPath<ModelType, PropertyType> {
            guard value != nil else {
                return
            }
            object[keyPath: accessor as! WritableKeyPath<ModelType, PropertyType>] = value! 
        }else {
            object[keyPath: accessor as! WritableKeyPath<ModelType, PropertyType?>] = value
        }
    }
    
    func access<PropertyType: CodableColumn>(getValueFromObject object: ModelType) -> PropertyType? {
        if accessor is WritableKeyPath<ModelType, PropertyType> {
            return object[keyPath: accessor as! WritableKeyPath<ModelType, PropertyType>] 
        }else {
            return object[keyPath: accessor as! WritableKeyPath<ModelType, PropertyType?>]
        }
    }    
}

public class ColumnBinding<ModelType: CodableTable, PropertyType: CodableColumn> : PartialColumnBinding<ModelType> {
    public typealias Accessor = WritableKeyPath<ModelType, PropertyType>
    public typealias OptionalAccessor = WritableKeyPath<ModelType, PropertyType?>
    
    public init(_ accessor: OptionalAccessor, alias columnName: String, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        super.init(accessor: accessor, columnName: columnName, columnType: PropertyType.columnType, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: defaultValue)
    }
    
    public init(_ accessor: Accessor, alias columnName: String, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        super.init(accessor: accessor, columnName: columnName, columnType: PropertyType.columnType, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: defaultValue)
    }
    
    public convenience init(_ accessor: OptionalAccessor, alias columnName: String, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: CodableColumnBase?) {
        self.init(accessor, alias: columnName, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: AnyColumnBinding.defaultType(from: defaultValue))
    }
    
    public convenience init(_ accessor: Accessor, alias columnName: String, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: CodableColumnBase?) {
        self.init(accessor, alias: columnName, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: AnyColumnBinding.defaultType(from: defaultValue))
    }

    override func access<FundamentalType: CodableFundamentalValue>(getFundamentalValueFromObject object: CodableTable) -> FundamentalType? {
        let propertyValue: PropertyType? = access(getValueFromObject: (object as! ModelType))
        return propertyValue?.archivedFundamentalValue() as! FundamentalType?
    }
    
    override func access<FundamentalType: CodableFundamentalValue>(setFundamentalValue value: FundamentalType?, forObject object: inout CodableTable) {
        let fundamentalValue = value as! PropertyType.FundamentalType
        var objectAsModelType = object as! ModelType
        access(setValue: PropertyType(withTypedValue: fundamentalValue), forObject: &objectAsModelType)
    }
}

extension ColumnBinding where PropertyType: CodableClassColumn {
    public convenience init(_ accessor: OptionalAccessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: defaultValue)
    }
    
    public convenience init(_ accessor: Accessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: defaultValue)
    }
    
    public convenience init(_ accessor: OptionalAccessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: CodableColumnBase?) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: AnyColumnBinding.defaultType(from: defaultValue))
    }
    
    public convenience init(_ accessor: Accessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: CodableColumnBase?) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: AnyColumnBinding.defaultType(from: defaultValue))
    }
}

extension ColumnBinding where PropertyType: CodableEnumColumn {
    public convenience init(_ accessor: OptionalAccessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: defaultValue)
    }
    
    public convenience init(_ accessor: Accessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: defaultValue)
    }
    
    public convenience init(_ accessor: OptionalAccessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: CodableColumnBase?) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: AnyColumnBinding.defaultType(from: defaultValue))
    }
    
    public convenience init(_ accessor: Accessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: CodableColumnBase?) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: AnyColumnBinding.defaultType(from: defaultValue))
    }
}

extension ColumnBinding where PropertyType: CodableStructColumn {
    public convenience init(_ accessor: OptionalAccessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: defaultValue)
    }
    
    public convenience init(_ accessor: Accessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: defaultValue)
    }
    
    public convenience init(_ accessor: OptionalAccessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: CodableColumnBase?) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: AnyColumnBinding.defaultType(from: defaultValue))
    }
    
    public convenience init(_ accessor: Accessor, alias columnName: String? = nil, isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: CodableColumnBase?) {
        self.init(accessor, alias: columnName ?? accessor.name, isPrimary: isPrimary, orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict, isNotNull: isNotNull, isUnique: isUnique, defaultTo: AnyColumnBinding.defaultType(from: defaultValue))
    }
}

