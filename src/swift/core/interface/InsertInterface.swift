//
// Created by qiuwenchen on 2022/9/12.
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

import Foundation
import WCDB_Private

/// Convenient interface for inserting
public protocol InsertInterface: AnyObject {

    /// Execute inserting with `TableEncodable` object on specific(or all) properties
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insert<Object: TableEncodable>(
        _ objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws

    /// Execute inserting with `TableEncodable` object on specific(or all) properties
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insert<Object: TableEncodable>(
        _ objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws

    /// Execute inserting or replacing with `TableEncodable` object on specific(or all) properties.
    /// It will replace the original row while they have same primary key or row id.
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insertOrReplace<Object: TableEncodable>(
        _ objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws

    /// Execute inserting or replacing with `TableEncodable` object on specific(or all) properties.
    /// It will replace the original row while they have same primary key or row id.
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insertOrReplace<Object: TableEncodable>(
        _ objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws

    /// Execute inserting or ignoring with `TableEncodable` object on specific(or all) properties.
    /// It will ignore the object while there already exists the same primary key or row id in current table.
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insertOrIgnore<Object: TableEncodable>(
        _ objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws

    /// Execute inserting or ignoring with `TableEncodable` object on specific(or all) properties.
    /// It will ignore the object while there already exists the same primary key or row id in current table.
    ///
    /// Note that it will run embedded transaction while objects.count>1.
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - objects: Table encodable object
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - table: Table name
    /// - Throws: `Error`
    func insertOrIgnore<Object: TableEncodable>(
        _ objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]?,
        intoTable table: String) throws
}

extension InsertInterface where Self: HandleRepresentable {
    public func insert<Object: TableEncodable>(
        _ objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        let insert = Insert(with: try getHandle(writeHint: true), named: table, on: propertyConvertibleList)
        return try insert.execute(with: objects)
    }

    public func insertOrReplace<Object: TableEncodable>(
        _ objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
            let insert = Insert(with: try getHandle(writeHint: true), named: table, on: propertyConvertibleList, onConflict: .Replace)
        return try insert.execute(with: objects)
    }

    public func insertOrIgnore<Object: TableEncodable>(
        _ objects: [Object],
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
            let insert = Insert(with: try getHandle(writeHint: true), named: table, on: propertyConvertibleList, onConflict: .Ignore)
        return try insert.execute(with: objects)
    }

    public func insert<Object: TableEncodable>(
        _ objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        return try insert(objects, on: propertyConvertibleList, intoTable: table)
    }

    public func insertOrReplace<Object: TableEncodable>(
        _ objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        return try insertOrReplace(objects, on: propertyConvertibleList, intoTable: table)
    }

    public func insertOrIgnore<Object: TableEncodable>(
        _ objects: Object...,
        on propertyConvertibleList: [PropertyConvertible]? = nil,
        intoTable table: String) throws {
        return try insertOrIgnore(objects, on: propertyConvertibleList, intoTable: table)
    }
}
