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

public protocol InsertTableInterfaceForObjc: AnyObject {
    associatedtype Object: WCTTableCoding

    func insert(_ objects: Object..., on propertyConvertibleList: [PropertyConvertible]?) throws
    func insert(_ objects: [Object], on propertyConvertibleList: [PropertyConvertible]?) throws
    func insertOrReplace(_ objects: Object..., on propertyConvertibleList: [PropertyConvertible]?) throws
    func insertOrReplace(_ objects: [Object], on propertyConvertibleList: [PropertyConvertible]?) throws
    func insertOrIgnore(_ objects: Object..., on propertyConvertibleList: [PropertyConvertible]?) throws
    func insertOrIgnore(_ objects: [Object], on propertyConvertibleList: [PropertyConvertible]?) throws
}

public protocol UpdateTableInterfaceForObjc: AnyObject {
    associatedtype Object: WCTTableCoding

    func update(on propertyConvertibleList: PropertyConvertible...,
                with object: Object,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
    func update(on propertyConvertibleList: [PropertyConvertible],
                with object: Object,
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
    func update(on propertyConvertibleList: PropertyConvertible...,
                with row: [ColumnEncodable],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
    func update(on propertyConvertibleList: [PropertyConvertible],
                with row: [ColumnEncodable],
                where condition: Condition?,
                orderBy orderList: [OrderBy]?,
                limit: Limit?,
                offset: Offset?) throws
}

public protocol SelectTableInterfaceForObjc: AnyObject {
    associatedtype Object: WCTTableCoding

    // TODO: Add generic property convertible to fit the type
    func getObjects(on propertyConvertibleList: [PropertyConvertible],
                    where condition: Condition?,
                    orderBy orderList: [OrderBy]?,
                    limit: Limit?,
                    offset: Offset?) throws -> [Object]
    func getObjects(on propertyConvertibleList: PropertyConvertible...,
                    where condition: Condition?,
                    orderBy orderList: [OrderBy]?,
                    limit: Limit?,
                    offset: Offset?) throws -> [Object]

    func getObject(on propertyConvertibleList: [PropertyConvertible],
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   offset: Offset?) throws -> Object?
    func getObject(on propertyConvertibleList: PropertyConvertible...,
                   where condition: Condition?,
                   orderBy orderList: [OrderBy]?,
                   offset: Offset?) throws -> Object?
}

#endif
