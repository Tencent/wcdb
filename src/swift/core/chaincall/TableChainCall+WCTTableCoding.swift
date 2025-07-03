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

#if WCDB_SWIFT_BRIDGE_OBJC

import Foundation
import WCDB_Private

/// ChainCall interface for inserting
public protocol TableInsertChainCallInterfaceForObjc: AnyObject {

    /// Prepare chain call for inserting of `WCTTableCoding` object
    ///
    /// - Parameters:
    ///   - cls: Type of table object
    /// - Returns: `Insert`
    func prepareInsert<Root: WCTTableCoding>(of cls: Root.Type) throws -> Insert

    /// Prepare chain call for inserting or replacing of `WCTTableCoding` object
    ///
    /// - Parameters:
    ///   - cls: Type of table object
    /// - Returns: `Insert`
    func prepareInsertOrReplace<Root: WCTTableCoding>(of cls: Root.Type) throws -> Insert

    /// Prepare chain call for inserting or ignoring of `WCTTableCoding` object
    ///
    /// - Parameters:
    ///   - cls: Type of table object
    /// - Returns: `Insert`
    func prepareInsertOrIgnore<Root: WCTTableCoding>(of cls: Root.Type) throws -> Insert
}

/// ChainCall interface for selecting
public protocol TableSelectChainCallInterfaceForObjc: AnyObject {

    /// Prepare chain call for selecting of `TableDecodable` object
    ///
    /// - Parameters:
    ///   - cls: Type of table decodable object
    ///   - isDistinct: Is distinct or not
    /// - Returns: `Select`
    func prepareSelect<Root: WCTTableCoding>(of cls: Root.Type,
                                             isDistinct: Bool) throws -> Select
}

#endif
