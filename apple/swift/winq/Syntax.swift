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

#error("Combine class/conditional protocol to fit this.")
protocol SyntaxIdentifier {
    var type: Syntax.IdentifierType {get}
    var description: String {get}
    typealias Iterator = (SyntaxIdentifier, inout Bool) -> Void
    func iterate(_ iterator: Iterator)
}

extension SyntaxIdentifier {
    public func iterate(_ iterator: Iterator) {
        var stop = false
        iterate(iterator: iterator, stop: &stop)
    }
    private func iterate(iterator: Iterator, stop: inout Bool) {
        iterator(self, &stop)
    }
}

public class Syntax {
    static let space: String = " "

    public enum IdentifierType: Int {
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
}

// MARK: - Const
public extension Syntax {
    enum ColumnType: Int {
        case Null = 0
        case Integer32
        case Integer64
        case Float
        case Text
        case BLOB

        var description: String {
            switch self {
            case .Integer32:
                fallthrough
            case .Integer64:
                return "INTEGER"
            case .Float:
                return "FLOAT"
            case .Text:
                return "TEXT"
            case .BLOB:
                return "BLOB"
            default:
                assert(self == .Null)
                return "NULL"
            }
        }

        static func isIntegerColumnType(type: String) -> Bool {
            return type.caseInsensitiveCompare("INTEGER") == .orderedSame
                || type.caseInsensitiveCompare("INT") == .orderedSame
                || type.caseInsensitiveCompare("TINYINT") == .orderedSame
                || type.caseInsensitiveCompare("SMALLINT") == .orderedSame
                || type.caseInsensitiveCompare("MEDIUMINT") == .orderedSame
                || type.caseInsensitiveCompare("BIGINT") == .orderedSame
                || type.caseInsensitiveCompare("UNSIGNED BIG INT") == .orderedSame
                || type.caseInsensitiveCompare("INT2") == .orderedSame
                || type.caseInsensitiveCompare("INT8") == .orderedSame
        }
    }

    enum CompoundOperator {
        case Union
        case UnionAll
        case Intersect
        case Except

        var description: String {
            switch self {
            case .Union:
                return "UNION"
            case .UnionAll:
                return "UNION ALL"
            case .Intersect:
                return "INTERSECT"
            case .Except:
                return "EXCEPT"
            }
        }
    }

    enum Conflict {
        case Rollback
        case Abort
        case Fail
        case Ignore
        case Replace

        var description: String {
            switch self {
            case .Rollback:
                return "ON CONFLICT ROLLBACK"
            case .Abort:
                return "ON CONFLICT ABORT"
            case .Fail:
                return "ON CONFLICT FAIL"
            case .Ignore:
                return "ON CONFLICT IGNORE"
            case .Replace:
                return "ON CONFLICT REPLACE"
            }
        }
    }

    enum JoinOperator {
        case With
        case Join
        case LeftOuterJoin
        case LeftJoin
        case InnerJoin
        case CrossJoin
        case NaturalJoin
        case NaturalLeftOuterJoin
        case NaturalLeftJoin
        case NaturalInnerJoin
        case NaturalCrossJoin

        var description: String {
            switch self {
            case .With:
                return ","
            case .Join:
                return "JOIN"
            case .LeftOuterJoin:
                return "LEFT OUTER JOIN"
            case .LeftJoin:
                return "LEFT JOIN"
            case .InnerJoin:
                return "INNER JOIN"
            case .CrossJoin:
                return "CROSS JOIN"
            case .NaturalJoin:
                return "NATURAL JOIN"
            case .NaturalLeftOuterJoin:
                return "NATURAL LEFT OUTER JOIN"
            case .NaturalLeftJoin:
                return "NATURAL LEFT JOIN"
            case .NaturalInnerJoin:
                return "NATURAL INNER JOIN"
            case .NaturalCrossJoin:
                return "NATURAL CROSS JOIN"
            }
        }
    }

    enum Order {
        case ASC
        case DESC

        var description: String {
            switch self {
            case .ASC:
                return "ASC"
            case .DESC:
                return "DESC"
            }
        }
    }

    enum LimitParameterType {
        case NotSet
        case Offset
        case End
    }

    enum ConflictAction {
        case Replace
        case Rollback
        case Abort
        case Fail
        case Ignore

        var description: String {
            switch self {
            case .Replace:
                return "OR REPLACE"
            case .Rollback:
                return "OR ROLLBACK"
            case .Abort:
                return "OR ABORT"
            case .Fail:
                return "OR FAIL"
            case .Ignore:
                return "OR IGNORE"
            }
        }
    }

    static let masterTable: String = "sqlite_master"
    static let mainSchema: String = "main"
    static let tempSchema: String = "temp"
    static let builtinTablePrefix: String = "sqlite_"
}
