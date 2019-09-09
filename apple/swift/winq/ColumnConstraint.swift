//
// Created by sanhuazhang on 2019/05/06
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

// MARK: - Syntax
public class ColumnConstraint: SQL {
    public struct Identifier: SyntaxIdentifier {
        public var type: Syntax.IdentifierType {
            return .ColumnConstraint
        }

        public var name: String
        public enum Switcher {
            case PrimaryKey
            case NotNull
            case Unique
            case Check
            case Default
            case Collate
            case ForeignKey
        }
        public var switcher: Switcher
        public var order: Syntax.Order?
        public var conflict: Syntax.Conflict?
        public var autoIncrement: Bool
        public var expression: Expression.Identifier?
        public var collation: String?
        public var foreignKeyClause: ForeignKeyClause?

        public var description: String {
            var description: String = ""
            if !name.isEmpty {
                description.append("CONSTRAINT \(name) ")
            }
            switch switcher {
            case .PrimaryKey:
                description.append("PRIMARY KEY")
                if let order = self.order {
                    description.append(Syntax.space + order.description)
                }
                if let conflict = self.conflict {
                    description.append(Syntax.space + conflict.description)
                }
                if autoIncrement {
                    description.append(" AUTOINCREMENT")
                }
            case .NotNull:
                description.append("NOT NULL")
                if let conflict = self.conflict {
                    description.append(Syntax.space + conflict.description)
                }
            case .Unique:
                description.append("UNIQUE")
                if let conflict = self.conflict {
                    description.append(Syntax.space + conflict.description)
                }
            case .Check:
                description.append("CHECK(\(expression?.description ?? ""))")
            case .Default:
                description.append("DEFAULT \(expression?.description ?? "")")
            case .Collate:
                description.append("COLLATE \(collation ?? "")")
            case .ForeignKey:
                description.append(foreignKeyClause?.description ?? "")
            }
            return ""
        }
    }

    public var syntax: Identifier

    required init(syntax: Identifier) {
        self.syntax = syntax
    }
}

// MARK: - SQL
