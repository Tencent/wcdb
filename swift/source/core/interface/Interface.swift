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

public protocol InsertInterface {
    func insert<Object: TableEncodable>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws
    func insert<Object: TableEncodable>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws
    func insertOrReplace<Object: TableEncodable>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws
    func insertOrReplace<Object: TableEncodable>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws
}

extension InsertInterface where Self: Core {
    public func insert<Object: TableEncodable>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        let insert = try Insert(with: self, named: table, on: propertyConvertibleList, isReplace: false)
        return try insert.execute(with: objects)
    }

    public func insertOrReplace<Object: TableEncodable>(
        objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        let insert = try Insert(with: self, named: table, on: propertyConvertibleList, isReplace: true)
        return try insert.execute(with: objects)
    }

    public func insert<Object: TableEncodable>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        return try insert(objects: objects, on: propertyConvertibleList, intoTable: table)
    }

    public func insertOrReplace<Object: TableEncodable>(
        objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        return try insertOrReplace(objects: objects, on: propertyConvertibleList, intoTable: table)
    }
}

public protocol UpdateInterface {
    func update<Object: TableEncodable>(
        table: String,
        on propertyConvertibleList: PropertyConvertible...,
        with object: Object,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws

    func update<Object: TableEncodable>(
        table: String,
        on propertyConvertibleList: [PropertyConvertible],
        with object: Object,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws

    func update(table: String,
                on propertyConvertibleList: PropertyConvertible...,
                with row: [ColumnEncodableBase],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws

    func update(table: String,
                on propertyConvertibleList: [PropertyConvertible],
                with row: [ColumnEncodableBase],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
}

extension UpdateInterface where Self: Core {
    public func update<Object: TableEncodable>(
        table: String,
        on propertyConvertibleList: [PropertyConvertible],
        with object: Object,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws {
        let update = try Update(with: self, on: propertyConvertibleList, andTable: table)
        if condition != nil {
            update.where(condition!)
        }
        if orderList != nil {
            update.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                update.limit(limit!, offset: offset!)
            } else {
                update.limit(limit!)
            }
        }
        return try update.execute(with: object)
    }

    public func update<Object: TableEncodable>(
        table: String,
        on propertyConvertibleList: PropertyConvertible...,
        with object: Object,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws {
        return try update(table: table,
                          on: propertyConvertibleList,
                          with: object,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    public func update(table: String,
                       on propertyConvertibleList: PropertyConvertible...,
                       with row: [ColumnEncodableBase],
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        return try update(table: table,
                          on: propertyConvertibleList,
                          with: row,
                          where: condition,
                          orderBy: orderList,
                          limit: limit,
                          offset: offset)
    }

    public func update(table: String,
                       on propertyConvertibleList: [PropertyConvertible],
                       with row: [ColumnEncodableBase],
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        let update = try Update(with: self, on: propertyConvertibleList, andTable: table)
        if condition != nil {
            update.where(condition!)
        }
        if orderList != nil {
            update.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                update.limit(limit!, offset: offset!)
            } else {
                update.limit(limit!)
            }
        }
        return try update.execute(with: row)
    }
}

public protocol DeleteInterface {
    func delete(fromTable table: String,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
}

extension DeleteInterface where Self: Core {
    public func delete(fromTable table: String,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       limit: Limit? = nil,
                       offset: Offset? = nil) throws {
        let delete = try Delete(with: self, andTableName: table)
        if condition != nil {
            delete.where(condition!)
        }
        if orderList != nil {
            delete.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                delete.limit(limit!, offset: offset!)
            } else {
                delete.limit(limit!)
            }
        }
        return try delete.execute()
    }
}

public protocol RowSelectInterface {
    func getRows(on columnResultConvertibleList: [ColumnResultConvertible],
                 fromTable table: String,
                 where condition: Condition?,
                 orderBy orderList: [OrderBy]?,
                 limit: Limit?,
                 offset: Offset?) throws -> FundamentalRowXColumn
    func getRows(on columnResultConvertibleList: ColumnResultConvertible...,
                 fromTable table: String,
                 where condition: Condition?,
                 orderBy orderList: [OrderBy]?,
                 limit: Limit?, offset: Offset?) throws -> FundamentalRowXColumn

    func getRow(on columnResultConvertibleList: ColumnResultConvertible...,
                fromTable table: String,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                offset: Offset?) throws -> FundamentalRow?
    func getRow(on columnResultConvertibleList: [ColumnResultConvertible],
                fromTable table: String,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                offset: Offset?) throws -> FundamentalRow?

    func getColumn(on result: ColumnResultConvertible,
                   fromTable table: String,
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   limit: Limit?,
                   offset: Offset?) throws -> FundamentalColumn
    func getDistinctColumn(on result: ColumnResultConvertible,
                           fromTable table: String,
                           where condition: Condition?,
                           orderBy orderList: [OrderBy]?,
                           limit: Limit?,
                           offset: Offset?) throws -> FundamentalColumn

    func getValue(on result: ColumnResultConvertible,
                  fromTable table: String,
                  where condition: Condition?,
                  orderBy orderList: [OrderBy]?,
                  limit: Limit?,
                  offset: Offset?) throws -> FundamentalValue
    func getDistinctValue(on result: ColumnResultConvertible,
                          fromTable table: String,
                          where condition: Condition?,
                          orderBy orderList: [OrderBy]?,
                          limit: Limit?,
                          offset: Offset?) throws -> FundamentalValue
}

extension RowSelectInterface where Self: Core {
    public func getRows(on columnResultConvertibleList: [ColumnResultConvertible],
                        fromTable table: String,
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws -> FundamentalRowXColumn {
        let rowSelect = try RowSelect(with: self,
                                      results: columnResultConvertibleList,
                                      tables: [table],
                                      isDistinct: false)
        if condition != nil {
            rowSelect.where(condition!)
        }
        if orderList != nil {
            rowSelect.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                rowSelect.limit(limit!, offset: offset!)
            } else {
                rowSelect.limit(limit!)
            }
        }
        return try rowSelect.allRows()
    }

    public func getRows(on columnResultConvertibleList: ColumnResultConvertible...,
                        fromTable table: String,
                        where condition: Condition? = nil,
                        orderBy orderList: [OrderBy]? = nil,
                        limit: Limit? = nil,
                        offset: Offset? = nil) throws -> FundamentalRowXColumn {
        return try getRows(
            on: columnResultConvertibleList.isEmpty ? [Column.any] : columnResultConvertibleList,
            fromTable: table,
            where: condition,
            orderBy: orderList,
            limit: limit,
            offset: offset)
    }

    public func getRow(on columnResultConvertibleList: ColumnResultConvertible...,
                       fromTable table: String,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       offset: Offset? = nil) throws -> FundamentalRow? {
        return try getRow(
            on: columnResultConvertibleList.isEmpty ? [Column.any] : columnResultConvertibleList,
            fromTable: table,
            where: condition,
            orderBy: orderList,
            offset: offset)
    }

    public func getRow(on columnResultConvertibleList: [ColumnResultConvertible],
                       fromTable table: String,
                       where condition: Condition? = nil,
                       orderBy orderList: [OrderBy]? = nil,
                       offset: Offset? = nil) throws -> FundamentalRow? {
        return try getRows(on: columnResultConvertibleList,
                           fromTable: table,
                           where: condition,
                           orderBy: orderList,
                           limit: 1,
                           offset: offset).first
    }

    public func getColumn(on result: ColumnResultConvertible,
                          fromTable table: String,
                          where condition: Condition? = nil,
                          orderBy orderList: [OrderBy]? = nil,
                          limit: Limit? = nil,
                          offset: Offset? = nil) throws -> FundamentalColumn {
        let rowSelect = try RowSelect(with: self, results: [result], tables: [table], isDistinct: false)
        if condition != nil {
            rowSelect.where(condition!)
        }
        if orderList != nil {
            rowSelect.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                rowSelect.limit(limit!, offset: offset!)
            } else {
                rowSelect.limit(limit!)
            }
        }
        return try rowSelect.allValues()
    }

    public func getDistinctColumn(on result: ColumnResultConvertible,
                                  fromTable table: String,
                                  where condition: Condition? = nil,
                                  orderBy orderList: [OrderBy]? = nil,
                                  limit: Limit? = nil,
                                  offset: Offset? = nil) throws -> FundamentalColumn {
        let rowSelect = try RowSelect(with: self, results: [result], tables: [table], isDistinct: true)
        if condition != nil {
            rowSelect.where(condition!)
        }
        if orderList != nil {
            rowSelect.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                rowSelect.limit(limit!, offset: offset!)
            } else {
                rowSelect.limit(limit!)
            }
        }
        return try rowSelect.allValues()
    }

    public func getValue(on result: ColumnResultConvertible,
                         fromTable table: String,
                         where condition: Condition? = nil,
                         orderBy orderList: [OrderBy]? = nil,
                         limit: Limit? = nil,
                         offset: Offset? = nil) throws -> FundamentalValue {
        return (try getRows(on: result,
                            fromTable: table,
                            where: condition,
                            orderBy: orderList,
                            limit: 1,
                            offset: offset).first?.first) ?? FundamentalValue(nil)
    }

    public func getDistinctValue(on result: ColumnResultConvertible,
                                 fromTable table: String,
                                 where condition: Condition? = nil,
                                 orderBy orderList: [OrderBy]? = nil,
                                 limit: Limit? = nil,
                                 offset: Offset? = nil) throws -> FundamentalValue {
        return (try getDistinctColumn(on: result, fromTable: table).first) ?? FundamentalValue(nil)
    }
}

public protocol SelectInterface {
    //TODO: Add generic property convertible to fit the type
    func getObjects<Object: TableDecodable>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: String,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws -> [Object]
    func getObjects<Object: TableDecodable>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: String,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        limit: Limit?,
        offset: Offset?) throws -> [Object]

    func getObject<Object: TableDecodable>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: String,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        offset: Offset?) throws -> Object?
    func getObject<Object: TableDecodable>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: String,
        where condition: Condition?,
        orderBy orderList: [OrderBy]?,
        offset: Offset?) throws -> Object?
}

extension SelectInterface where Self: Core {
    public func getObjects<Object: TableDecodable>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: String,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws -> [Object] {
        let select = try Select(with: self, on: propertyConvertibleList, table: table, isDistinct: false)
        if condition != nil {
            select.where(condition!)
        }
        if orderList != nil {
            select.order(by: orderList!)
        }
        if limit != nil {
            if offset != nil {
                select.limit(limit!, offset: offset!)
            } else {
                select.limit(limit!)
            }
        }
        return try select.allObjects()
    }

    public func getObjects<Object: TableDecodable>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: String,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        limit: Limit? = nil,
        offset: Offset? = nil) throws -> [Object] {
        return try getObjects(on: propertyConvertibleList.isEmpty ? Object.Properties.all : propertyConvertibleList,
                              fromTable: table,
                              where: condition,
                              orderBy: orderList,
                              limit: limit,
                              offset: offset)
    }

    public func getObject<Object: TableDecodable>(
        on propertyConvertibleList: [PropertyConvertible],
        fromTable table: String,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        offset: Offset? = nil) throws -> Object? {
        return try getObjects(on: propertyConvertibleList,
                              fromTable: table,
                              where: condition,
                              orderBy: orderList,
                              limit: 1,
                              offset: offset).first
    }

    public func getObject<Object: TableDecodable>(
        on propertyConvertibleList: PropertyConvertible...,
        fromTable table: String,
        where condition: Condition? = nil,
        orderBy orderList: [OrderBy]? = nil,
        offset: Offset? = nil) throws -> Object? {
        return try getObject(on: propertyConvertibleList.isEmpty ? Object.Properties.all : propertyConvertibleList,
                             fromTable: table,
                             where: condition,
                             orderBy: orderList,
                             offset: offset)
    }
}

public protocol TableInterface {
    func create<Root: TableEncodable>(table name: String, of class: Root.Type) throws
    func create<Root: TableEncodable>(virtualTable name: String, of class: Root.Type) throws

    func create(table name: String, with columnDefList: [ColumnDef], and constraintList: [TableConstraint]?) throws
    func create(table name: String, with columnDefList: ColumnDef..., and constraintList: [TableConstraint]?) throws

    func addColumn(with columnDef: ColumnDef, forTable table: String) throws
    func drop(table name: String) throws

    func create(index name: String,
                with columnIndexConvertibleList: [ColumnIndexConvertible],
                forTable table: String) throws
    func create(index name: String,
                with columnIndexConvertibleList: ColumnIndexConvertible...,
                forTable table: String) throws
    func drop(index name: String) throws
}

extension TableInterface where Self: Core {
    public func create<Root: TableEncodable>(
        table name: String,
        of rootType: Root.Type) throws {
        try run(embeddedTransaction: {
            let orm = Root.CodingKeys.objectRelationalMapping
            if try isTableExists(name) {
                var columnNames: [String] = []
                do {
                    let coreStatement = try prepare(StatementPragma().pragma(.tableInfo, to: name))
                    while try coreStatement.step() {
                        let columnName: String = coreStatement.value(atIndex: 1) ?? ""
                        columnNames.append(columnName)
                    }
                }
                var keys = orm.allKeys
                for columnName in columnNames {
                    if let index = keys.index(where: { (key) -> Bool in
                        return key.stringValue.caseInsensitiveCompare(columnName) == ComparisonResult.orderedSame
                    }) {
                        keys.remove(at: index)
                    } else {
                        Error.warning("Skip column named [\(columnName)] for table [\(name)]")
                    }
                }
                try keys.forEach {
                    try exec(StatementAlterTable().alter(table: name).addColumn(with: orm.generateColumnDef(with: $0)))
                }
            } else {
                try exec(orm.generateCreateTableStatement(named: name))
            }
            try orm.generateCreateIndexStatements(onTable: name)?.forEach { try exec($0) }
        })
    }

    public func create<Root: TableEncodable>(virtualTable name: String, of class: Root.Type) throws {
        try run(transaction: {
            try exec(`class`.CodingKeys.objectRelationalMapping.generateCreateVirtualTableStatement(named: name))
        })
    }

    public func create(table name: String,
                       with columnDefList: ColumnDef...,
                       and constraintList: [TableConstraint]? = nil) throws {
        try create(table: name, with: columnDefList, and: constraintList)
    }

    public func create(table name: String,
                       with columnDefList: [ColumnDef],
                       and constraintList: [TableConstraint]? = nil) throws {
        try exec(StatementCreateTable().create(table: name, with: columnDefList, and: constraintList))
    }

    public func addColumn(with columnDef: ColumnDef, forTable table: String) throws {
        try exec(StatementAlterTable().alter(table: table).addColumn(with: columnDef))
    }

    public func drop(table name: String) throws {
        try exec(StatementDropTable().drop(table: name))
    }

    public func create(index name: String,
                       with columnIndexConvertibleList: ColumnIndexConvertible...,
                       forTable table: String) throws {
        try create(index: name, with: columnIndexConvertibleList, forTable: table)
    }

    public func create(index name: String,
                       with columnIndexConvertibleList: [ColumnIndexConvertible],
                       forTable table: String) throws {
        try exec(StatementCreateIndex().create(index: name).on(table: table, indexesBy: columnIndexConvertibleList))
    }

    public func drop(index name: String) throws {
        try exec(StatementDropIndex().drop(index: name))
    }
}
