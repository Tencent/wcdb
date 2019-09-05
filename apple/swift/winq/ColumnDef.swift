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
public class ColumnDef: SQL {
    public struct Identifier: SyntaxIdentifier {
        public var type: Syntax.IdentifierType {
            return .ColumnDef
        }

        public var column: Column.Identifier
        public var columnType: ColumnType?
        public var constraints: [ColumnConstraint.Identifier]
        public var description: String {
            var description = column.description
            if let columnType = self.columnType {
                description.append(Syntax.space + columnType.description)
            }
            for constraint in self.constraints {
                description.append(Syntax.space + constraint.description)
            }
            return description
        }

        private func iterate(iterator: (SyntaxIdentifier, inout Bool) -> Void, stop: inout Bool) {
            #warning("TODO")
//            void ColumnDef::iterate(const Iterator& iterator, bool& stop)
//            {
//                Identifier::iterate(iterator, stop);
//                recursiveIterate(column, iterator, stop);
//                listIterate(constraints, iterator, stop);
//            }

        }
    }

    public var syntax: Identifier

    required init(syntax: Identifier) {
        self.syntax = syntax
    }
}

// MARK: - SQL
public extension ColumnDef {
    convenience init(column: Column) {
        self.init(syntax: Identifier(column: column.syntax, columnType: nil, constraints: []))
    }

    convenience init(column: Column, columnType: ColumnType) {
        self.init(syntax: Identifier(column: column.syntax, columnType: columnType, constraints: []))
    }

    func constraint(_ constraint: ColumnConstraint) -> Self {
        self.syntax.constraints.append(constraint.syntax)
        return self
    }
}
