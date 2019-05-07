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

// MARK: - Syntax
public class Column: SQL {
    public struct Identifier: SyntaxIdentifier {
        public var name: String

        public var type: Syntax.IdentifierType {
            return .Column
        }
        public var description: String {
            return name
        }
    }

    public var syntax: Identifier

    required init(syntax: Identifier) {
        self.syntax = syntax
    }
}

// MARK: - SQL
public extension Column {
    static let rowid: Column = Column("rowid")

    convenience init(_ name: String) {
        self.init(syntax: Identifier(name: name))
    }
}
