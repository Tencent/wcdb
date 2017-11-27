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

public protocol InsertTableInterface {
    associatedtype Object: CodableTable
    
    func insert(objects: Object..., on propertyConvertibleList: [PropertyConvertible]?) throws
    func insert(objects: [Object], on propertyConvertibleList: [PropertyConvertible]?) throws
    func insertOrReplace(objects: Object..., on propertyConvertibleList: [PropertyConvertible]?) throws 
    func insertOrReplace(objects: [Object], on propertyConvertibleList: [PropertyConvertible]?) throws 
}

extension InsertTableInterface where Self: TableBase {
    public func insert(objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        let insert = try Insert(with: self.database, named: self.name, on: propertyConvertibleList, isReplace: false)
        return try insert.execute(with: objects)
    }
    
    public func insertOrReplace(objects: [Object], on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        let insert = try Insert(with: self.database, named: self.name, on: propertyConvertibleList, isReplace: true)
        return try insert.execute(with: objects)
    }
    
    public func insert(objects: Object..., on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        return try insert(objects: objects, on: propertyConvertibleList)
    }
    
    public func insertOrReplace(objects: Object..., on propertyConvertibleList: [PropertyConvertible]? = nil) throws {
        return try insertOrReplace(objects: objects, on: propertyConvertibleList)
    }
}

public protocol UpdateTableInterface {
    associatedtype Object: CodableTable

    func update(on propertyConvertibleList: PropertyConvertible..., with object: Object, where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws
    func update(on propertyConvertibleList: [PropertyConvertible], with object: Object, where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws 
    func update(on propertyConvertibleList: PropertyConvertible..., with row: [CodableColumnBase], where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws 
    func update(on propertyConvertibleList: [PropertyConvertible], with row: [CodableColumnBase], where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws 
}

extension UpdateTableInterface where Self: TableBase {
    public func update(on propertyConvertibleList: [PropertyConvertible], with object: Object, where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws {
        let update = try Update(with: self.database, on: propertyConvertibleList, andTable: self.name)
        if condition != nil {
            update.where(condition!)
        }
        if orderList != nil {
            update.order(by: orderList!)
        }
        if limit != nil {
            update.limit(limit!)
        }
        if offset != nil {
            update.offset(offset!)
        }
        return try update.execute(with: object)
    }
    
    public func update(on propertyConvertibleList: PropertyConvertible..., with object: Object, where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws {
        return try update(on: propertyConvertibleList, with: object, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }
    
    public func update(on propertyConvertibleList: PropertyConvertible..., with row: [CodableColumnBase], where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws {
        return try update(on: propertyConvertibleList, with: row, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }
    
    public func update(on propertyConvertibleList: [PropertyConvertible], with row: [CodableColumnBase], where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws {
        let update = try Update(with: self.database, on: propertyConvertibleList, andTable: self.name)
        if condition != nil {
            update.where(condition!)
        }
        if orderList != nil {
            update.order(by: orderList!)
        }
        if limit != nil {
            update.limit(limit!)
        }
        if offset != nil {
            update.offset(offset!)
        }
        return try update.execute(with: row)
    }
}

public protocol DeleteTableInterface {
    func delete(where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws
}

extension DeleteTableInterface where Self: TableBase {
    public func delete(where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws {
        let delete = try Delete(with: self.database, andTableName: self.name)
        if condition != nil {
            delete.where(condition!)
        }
        if orderList != nil {
            delete.order(by: orderList!)
        }
        if limit != nil {
            delete.limit(limit!)
        }
        if offset != nil {
            delete.offset(offset!)
        }
        return try delete.execute()
    }
}

public protocol RowSelectTableInterface {
    func getRows(on columnResultConvertiableList: [ColumnResultConvertible], where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws -> FundamentalRowXColumn     
    func getRows(on columnResultConvertiableList: ColumnResultConvertible..., where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws -> FundamentalRowXColumn 
    
    func getRow(on columnResultConvertibleList: ColumnResultConvertible..., where condition: Condition?, orderBy orderList: [OrderBy]?, offset: Offset?) throws -> FundamentalRow?
    func getRow(on columnResultConvertibleList: [ColumnResultConvertible], where condition: Condition?, orderBy orderList: [OrderBy]?, offset: Offset?) throws -> FundamentalRow? 
    
    func getColumn(on result: ColumnResultConvertible, where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws -> FundamentalColumn 
    func getDistinctColumn(on result: ColumnResultConvertible, where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws -> FundamentalColumn 
    
    func getValue(on result: ColumnResultConvertible, where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws -> FundamentalValue 
    func getDistinctValue(on result: ColumnResultConvertible, where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws -> FundamentalValue 
}

extension RowSelectTableInterface where Self: TableBase {
    public func getRows(on columnResultConvertiableList: [ColumnResultConvertible], where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws -> FundamentalRowXColumn {
        let rowSelect = try RowSelect(with: self.database, results: columnResultConvertiableList, tables: [self.name], isDistinct: false)
        if condition != nil {
            rowSelect.where(condition!)
        }
        if orderList != nil {
            rowSelect.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                rowSelect.limit(limit!, offset: offset!)
            }else {
                rowSelect.limit(limit!)                
            }
        }
        return try rowSelect.allRows()
    }
    
    public func getRows(on columnResultConvertiableList: ColumnResultConvertible..., where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws -> FundamentalRowXColumn {
        return try getRows(on: columnResultConvertiableList, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }
    
    public func getRow(on columnResultConvertibleList: ColumnResultConvertible..., where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, offset: Offset? = nil) throws -> FundamentalRow? {
        return try getRows(on: columnResultConvertibleList, where: condition, orderBy: orderList, limit: 1, offset: offset).first
    }
    
    public func getRow(on columnResultConvertibleList: [ColumnResultConvertible], where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, offset: Offset? = nil) throws -> FundamentalRow? {
        return try getRows(on: columnResultConvertibleList, where: condition, orderBy: orderList, limit: 1, offset: offset).first
    }
    
    public func getColumn(on result: ColumnResultConvertible, where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws -> FundamentalColumn {
        let rowSelect = try RowSelect(with: self.database, results: [result], tables: [self.name], isDistinct: false)
        if condition != nil {
            rowSelect.where(condition!)
        }
        if orderList != nil {
            rowSelect.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                rowSelect.limit(limit!, offset: offset!)                
            }else {
                rowSelect.limit(limit!)
            }
        }
        return try rowSelect.allValues()
    }
    
    public func getDistinctColumn(on result: ColumnResultConvertible, where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws -> FundamentalColumn {
        let rowSelect = try RowSelect(with: self.database, results: [result], tables: [self.name], isDistinct: true)
        if condition != nil {
            rowSelect.where(condition!)
        }
        if orderList != nil {
            rowSelect.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                rowSelect.limit(limit!, offset: offset!)
            }else {
                rowSelect.limit(limit!)
            }
        }
        return try rowSelect.allValues()
    }
    
    public func getValue(on result: ColumnResultConvertible, where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws -> FundamentalValue {
        return (try getRows(on: result, where: condition, orderBy: orderList, limit: 1, offset: offset).first?.first) ?? nil
    }
    
    public func getDistinctValue(on result: ColumnResultConvertible, where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws -> FundamentalValue {
        return (try getDistinctColumn(on: result).first) ?? nil
    }   
}

public protocol SelectTableInterface {
    associatedtype Object: CodableTable
    
    //TODO: Add generic property convertible to fit the type
    func getObjects(on propertyConvertibleList: [PropertyConvertible], where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws -> [Object] 
    func getObjects(on propertyConvertibleList: PropertyConvertible..., where condition: Condition?, orderBy orderList: [OrderBy]?, limit: Limit?, offset: Offset?) throws -> [Object] 
    
    func getObject(on propertyConvertibleList: [PropertyConvertible], where condition: Condition?, orderBy orderList: [OrderBy]?, offset: Offset?) throws -> Object? 
    func getObject(on propertyConvertibleList: PropertyConvertible..., where condition: Condition?, orderBy orderList: [OrderBy]?, offset: Offset?) throws -> Object? 
}

extension SelectTableInterface where Self: TableBase {
    public func getObjects(on propertyConvertibleList: [PropertyConvertible], where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws -> [Object] {
        let select = try Select(with: self.database, on: propertyConvertibleList, table: self.name, isDistinct: false)
        if condition != nil {
            select.where(condition!)
        }
        if orderList != nil {
            select.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                select.limit(limit!, offset: offset!)
            }else {
                select.limit(limit!)
            }
        }
        return try select.allObjects()
    }
    
    public func getObjects(on propertyConvertibleList: PropertyConvertible..., where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, limit: Limit? = nil, offset: Offset? = nil) throws -> [Object] {
        return try getObjects(on: propertyConvertibleList.isEmpty ? Object.allProperties : propertyConvertibleList, where: condition, orderBy: orderList, limit: limit, offset: offset)
    }
    
    public func getObject(on propertyConvertibleList: [PropertyConvertible], where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, offset: Offset? = nil) throws -> Object? {
        return try getObjects(on: propertyConvertibleList, where: condition, orderBy: orderList, limit: 1, offset: offset).first
    }
    
    public func getObject(on propertyConvertibleList: PropertyConvertible..., where condition: Condition? = nil, orderBy orderList: [OrderBy]? = nil, offset: Offset? = nil) throws -> Object? {
        return try getObjects(on: propertyConvertibleList.isEmpty ? Object.allProperties : propertyConvertibleList, where: condition, orderBy: orderList, limit: 1, offset: offset).first
    }
}

