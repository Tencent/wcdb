//
// Created by qiuwenchen on 2023/3/14.
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

/// Convenient interface for getting result with statement
public protocol StatementSelectInterface: AnyObject {

    /// Get value by specific statement.
    ///
    /// - Parameters:
    ///   - statement: Specific statement
    /// - Returns: `Value`
    /// - Throws: `Error`
    func getValue(from statement: Statement) throws -> Value?

    /// Get a column of values by specific statement.
    ///
    /// - Parameters:
    ///   - statement: Specific statement
    /// - Returns: `OneColumnValue`
    /// - Throws: `Error`
    func getColumn(from statement: Statement) throws -> OneColumnValue

    /// Get a row of values by specific statement.
    ///
    /// - Parameters:
    ///   - statement: Specific statement
    /// - Returns: `OneRowValue`
    /// - Throws: `Error`
    func getRow(from statement: Statement) throws -> OneRowValue?

    /// Get multi rows of values by specific statement.
    ///
    /// - Parameters:
    ///   - statement: Specific statement
    /// - Returns: `OneRowValue`
    /// - Throws: `Error`
    func getRows(from statement: Statement) throws -> MultiRowsValue
}

extension StatementSelectInterface where Self: HandleRepresentable {

    public func getValue(from statement: Statement) throws -> Value? {
        let handle = try getHandle(writeHint: false)
        try handle.prepare(statement)
        guard try handle.step() else {
            return nil
        }
        let result = handle.value(atIndex: 0)
        handle.finalize()
        return result
    }

    public func getColumn(from statement: Statement) throws -> OneColumnValue {
        let handle = try getHandle(writeHint: false)
        try handle.prepare(statement)
        let result = try handle.oneColumnValue(atIndex: 0)
        handle.finalize()
        return result
    }

    public func getRow(from statement: Statement) throws -> OneRowValue? {
        let handle = try getHandle(writeHint: false)
        try handle.prepare(statement)
        guard try handle.step() else {
            return nil
        }
        let result = handle.oneRowValue()
        handle.finalize()
        return result
    }

    public func getRows(from statement: Statement) throws -> MultiRowsValue {
        let handle = try getHandle(writeHint: false)
        try handle.prepare(statement)
        let result = try handle.multiRowsValue()
        handle.finalize()
        return result
    }
}
