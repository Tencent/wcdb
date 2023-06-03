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

// WCTTable
public extension Database {
    /// Get a wrapper from an existing table.
    ///
    /// - Parameters:
    ///   - name: The name of the table.
    ///   - type: A class conform to TableCodable protocol.
    /// - Returns: Nil for a non-existent table.
    /// - Throws: `Error`
    func getTable<Root: WCTTableCoding>(
        named name: String,
        of type: Root.Type = Root.self) -> Table<Root> {
        return Table<Root>(withDatabase: self, named: name)
    }
}

extension Database: InsertChainCallInterfaceForObjc {}
extension Database: SelectChainCallInterfaceForObjc {}

extension Database: InsertInterfaceForObjc {}
extension Database: UpdateInterfaceForObjc {}
extension Database: SelectInterfaceForObjc {}
extension Database: TableInterfaceForObjc {}

#endif
