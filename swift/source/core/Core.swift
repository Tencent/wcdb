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

public protocol Core {
    var tag: Tag? {get}
    var path: String {get}

    func prepare(_ statement: Statement) throws -> CoreStatement
    func exec(_ statement: Statement) throws
    func isTableExists(_ table: String) throws -> Bool
    
    func begin() throws
    func commit() throws
    func rollback() throws
    typealias TransactionClosure = () throws -> ()
    typealias ControlableTransactionClosure = () throws -> Bool
    func run(transaction: TransactionClosure) throws
    func run(transaction: ControlableTransactionClosure) throws
    func run(embeddedTransaction: TransactionClosure) throws
}

protocol CoreRepresentable {
    var core: Core {get}
    var tag: Tag? {get}
    var path: String {get}
}

extension CoreRepresentable {
    public var tag: Tag? {
        return core.tag
    }
    public var path: String {
        return core.path
    }
}
