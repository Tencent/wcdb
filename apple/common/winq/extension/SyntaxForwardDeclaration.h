//
// Created by sanhuazhang on 2019/05/02
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

#pragma once

#include "Syntax.h"

namespace WCDB {

typedef Syntax::ColumnType ColumnType;
typedef Syntax::Order Order;
typedef Syntax::Conflict Conflict;
typedef Syntax::MatchType MatchType;

class Schema;
class Column;
class BindParameter;
class ColumnDef;
class Expression;
class IndexedColumn;
class ColumnConstraint;
class TableConstraint;
class ForeignKey;
class CommonTableExpression;
class Expression;
class ModuleArgument;
class RaiseFunction;
class LiteralValue;
class Upsert;
class Pragma;
class Join;
class TableOrSubquery;
class ResultColumn;
class JoinConstraint;
class OrderingTerm;
class QualifiedTable;
class WindowDef;
class FrameSpec;
class Filter;

class Statement;
class StatementAlterTable;
class StatementAttach;
class StatementAnalyze;
class StatementBegin;
class StatementCommit;
class StatementRollback;
class StatementSavepoint;
class StatementRelease;
class StatementCreateIndex;
class StatementCreateTable;
class StatementCreateTrigger;
class StatementCreateView;
class StatementCreateVirtualTable;
class StatementDelete;
class StatementDetach;
class StatementDropIndex;
class StatementDropTable;
class StatementDropTrigger;
class StatementDropView;
class StatementInsert;
class StatementSelect;
class StatementUpdate;

template<typename T>
class SyntaxList;

typedef SyntaxList<IndexedColumn> IndexedColumns;
typedef SyntaxList<Expression> Expressions;
typedef SyntaxList<Column> Columns;
typedef SyntaxList<ColumnConstraint> ColumnConstraints;
typedef SyntaxList<TableOrSubquery> TablesOrSubqueries;
typedef SyntaxList<OrderingTerm> OrderingTerms;
typedef SyntaxList<TableConstraint> TableConstraints;
typedef SyntaxList<ModuleArgument> ModuleArguments;
typedef SyntaxList<CommonTableExpression> CommonTableExpressions;
typedef SyntaxList<ResultColumn> ResultColumns;
typedef SyntaxList<ColumnDef> ColumnDefs;
typedef SyntaxList<BindParameter> BindParameters;
typedef SyntaxList<LiteralValue> LiteralValues;

} // namespace WCDB
