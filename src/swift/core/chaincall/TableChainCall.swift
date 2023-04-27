//
// Created by qiuwenchen on 2023/4/27.
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

/// ChainCall interface for inserting in Table
public protocol TableInsertChainCallInterface: AnyObject {

    /// Prepare chain call for inserting of `TableEncodable` object
    ///
    /// - Parameters:
    ///   - cls: Type of table object
    /// - Returns: `Insert`
    func prepareInsert<Root: TableEncodable>(of cls: Root.Type) throws -> Insert

    /// Prepare chain call for inserting or replacing of `TableEncodable` object
    ///
    /// - Parameters:
    ///   - cls: Type of table object
    /// - Returns: `Insert`
    func prepareInsertOrReplace<Root: TableEncodable>(of cls: Root.Type) throws -> Insert

    /// Prepare chain call for inserting or ignoring of `TableEncodable` object
    ///
    /// - Parameters:
    ///   - cls: Type of table object
    /// - Returns: `Insert`
    func prepareInsertOrIgnore<Root: TableEncodable>(of cls: Root.Type) throws -> Insert

    /// Prepare chain call for inserting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Insert`
    func prepareInsert(on propertyConvertibleList: PropertyConvertible...) throws -> Insert

    /// Prepare chain call for inserting or replacing on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Insert`
    func prepareInsertOrReplace(on propertyConvertibleList: PropertyConvertible...) throws -> Insert

    /// Prepare chain call for inserting or ignoring on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Insert`
    func prepareInsertOrIgnore(on propertyConvertibleList: PropertyConvertible...) throws -> Insert

    /// Prepare chain call for inserting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Insert`
    func prepareInsert(on propertyConvertibleList: [PropertyConvertible]) throws -> Insert

    /// Prepare chain call for inserting or replacing on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Insert`
    func prepareInsertOrReplace(on propertyConvertibleList: [PropertyConvertible]) throws -> Insert

    /// Prepare chain call for inserting or ignoring on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Insert`
    func prepareInsertOrIgnore(on propertyConvertibleList: [PropertyConvertible]) throws -> Insert
}

/// ChainCall interface for deleting in Table
public protocol TableDeleteChainCallInterface: AnyObject {

    /// Prepare chain call for deleting on specific properties
    ///
    /// - Returns: `Delete`
    func prepareDelete() throws -> Delete
}

/// ChainCall interface for updating in Table
public protocol TableUpdateChainCallInterface: AnyObject {

    /// Prepare chain call for updating on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Update`
    func prepareUpdate(on propertyConvertibleList: PropertyConvertible...) throws -> Update

    /// Prepare chain call for updating on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    /// - Returns: `Update`
    func prepareUpdate(on propertyConvertibleList: [PropertyConvertible]) throws -> Update
}

/// ChainCall interface for row-selecting in Table
public protocol TableRowSelectChainCallInterface: AnyObject {
    /// Prepare chain call for row-selecting on specific column results
    ///
    /// - Parameters:
    ///   - resultColumnConvertibleList: `ResultColumn` list
    ///   - isDistinct: Is distinct or not
    /// - Returns: `RowSelect`
    func prepareRowSelect(on resultColumnConvertibleList: ResultColumnConvertible...,
                          isDistinct: Bool) throws -> RowSelect

    /// Prepare chain call for row-selecting on specific column results
    ///
    /// - Parameters:
    ///   - resultColumnConvertibleList: `ResultColumn` list
    ///   - isDistinct: Is distinct or not
    /// - Returns: `RowSelect`
    func prepareRowSelect(on resultColumnConvertibleList: [ResultColumnConvertible],
                          isDistinct: Bool) throws -> RowSelect
}

/// ChainCall interface for selecting in Table
public protocol TableSelectChainCallInterface: AnyObject {

    /// Prepare chain call for selecting of `TableDecodable` object
    ///
    /// - Parameters:
    ///   - cls: Type of table decodable object
    ///   - isDistinct: Is distinct or not
    /// - Returns: `Select`
    func prepareSelect<Root: TableDecodable>(of cls: Root.Type,
                                             isDistinct: Bool) throws -> Select

    /// Prepare chain call for selecting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - isDistinct: Is distinct or not
    /// - Returns: `Select`
    func prepareSelect(on propertyConvertibleList: PropertyConvertible...,
                       isDistinct: Bool) throws -> Select

    /// Prepare chain call for selecting on specific properties
    ///
    /// - Parameters:
    ///   - propertyConvertibleList: `Property` or `CodingTableKey` list
    ///   - isDistinct: Is distinct or not
    /// - Returns: `Select`
    func prepareSelect(on propertyConvertibleList: [PropertyConvertible],
                       isDistinct: Bool) throws -> Select
}
