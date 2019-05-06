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

protocol SyntaxIdentifier {
    var type: SyntaxType {get}
    var description: String {get}
    typealias Iterator = (SyntaxIdentifier, inout Bool) -> Void
    func iterate(_ iterator: Iterator)
    func iterate(iterator: Iterator, stop: inout Bool)
}

extension SyntaxIdentifier {
    public func iterate(_ iterator: Iterator) {
        var stop = false
        iterate(iterator: iterator, stop: &stop)
    }
    func iterate(iterator: Iterator, stop: inout Bool) {
        iterator(self, &stop)
    }
}

public enum SyntaxType: Int {
    case Column = 0x00000001
    case Schema
    case ColumnDef
    case ColumnConstraint
    case Expression
    case LiteralValue
    case ForeignKeyClause
    case BindParameter
    case RaiseFunction
    case WindowDef
    case Filter
    case IndexedColumn
    case TableConstraint
    case CTETableName
    case WithClause
    case RecursiveCTE
    case CommonTableExpression
    case QualifiedTableName
    case OrderingTerm
    case UpsertClause
    case Pragma
    case JoinClause
    case TableOrSubquery
    case JoinConstraint
    case SelectCore
    case ResultColumn
    case FrameSpec
    case FunctionInvocation
    case WindowFunctionInvocation

    case AlterTableSTMT = 0x00000101
    case AnalyzeSTMT
    case AttachSTMT
    case BeginSTMT
    case CommitSTMT
    case RollbackSTMT
    case SavepointSTMT
    case ReleaseSTMT
    case CreateIndexSTMT
    case CreateTableSTMT
    case CreateTriggerSTMT
    case SelectSTMT
    case InsertSTMT
    case DeleteSTMT
    case UpdateSTMT
    case CreateViewSTMT
    case CreateVirtualTableSTMT
    case DetachSTMT
    case DropIndexSTMT
    case DropTableSTMT
    case DropTriggerSTMT
    case DropViewSTMT
    case PragmaSTMT
    case ReindexSTMT
    case VacuumSTMT
}

protocol SQL {
    associatedtype Identifier: SyntaxIdentifier
    init(syntax: Identifier)
    var syntax: Identifier { get set }

    var description: String {get}
}

extension SQL {
    public var description: String {
        return syntax.description
    }
}

extension String {
    static let space = " "
}
