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

/// Convenient interface for table related operation
public protocol TableInterfaceForObjc: AnyObject {
    /// Create table, related indexes and constraints with specific type
    ///
    /// Note that it will create defined indexes automatically.
    /// The name of index is `"\(tableName)\(indexSuffixName)"` while `indexSuffixName` is defined by `IndexBinding`.
    /// BUT, it will not drop the undefined indexes. You should drop it manually.
    ///
    /// Note that it will add the newly defined column automatically.
    /// AND, it will skip the undefined column. It can be very developer-friendly while upgrading your database column.
    ///
    /// Note that it will run embedded transaction
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - name: Table name.
    ///   - rootType: Type of table encodable object
    /// - Throws: `Error`
    func create<Root: WCTTableCoding>(table name: String, of rootType: Root.Type) throws

    /// Create virtual table and constraints with specific type
    ///
    /// Note that it will run embedded transaction
    /// The embedded transaction means that it will run a transaction if it's not in other transaction,
    /// otherwise it will be executed within the existing transaction.
    ///
    /// - Parameters:
    ///   - name: Table name.
    ///   - rootType: Type of table encodable object
    /// - Throws: `Error`
    func create<Root: WCTTableCoding>(virtualTable name: String, of rootType: Root.Type) throws
}

extension TableInterfaceForObjc where Self: Database {
    public func create<Root: WCTTableCoding>(
        table name: String,
        of rootType: Root.Type) throws {
            let handle = try getHandle(writeHint: true)
            try withExtendedLifetime(handle) {
                if !WCTAPIBridge.createTable(name, withClass: rootType, with: $0.cppHandle) {
                    throw handle.getError()
                }
            }
        }

    public func create<Root: WCTTableCoding>(virtualTable name: String, of rootType: Root.Type) throws {
        let handle = try getHandle(writeHint: true)
        if !WCTAPIBridge.createVirtualTable(name, withClass: rootType, with: handle.cppHandle) {
            throw handle.getError()
        }
    }
}

#endif
