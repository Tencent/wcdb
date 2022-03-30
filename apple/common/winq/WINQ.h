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

// See also: https://sqlite.org/syntaxdiagrams.html

#include <WCDB/SQL.hpp>

#include <WCDB/AggregateFunction.hpp>
#include <WCDB/ColumnType.hpp>
#include <WCDB/Convertible.hpp>
#include <WCDB/CoreFunction.hpp>
#include <WCDB/ExpressionOperable.hpp>
#include <WCDB/FTS3Function.hpp>

#include <WCDB/BindParameter.hpp>
#include <WCDB/CTETable.hpp>
#include <WCDB/Column.hpp>
#include <WCDB/ColumnConstraint.hpp>
#include <WCDB/ColumnDef.hpp>
#include <WCDB/CommonTableExpression.hpp>
#include <WCDB/Expression.hpp>
#include <WCDB/Filter.hpp>
#include <WCDB/ForeignKey.hpp>
#include <WCDB/FrameSpec.hpp>
#include <WCDB/FunctionInvocation.hpp>
#include <WCDB/IndexedColumn.hpp>
#include <WCDB/Join.hpp>
#include <WCDB/JoinConstraint.hpp>
#include <WCDB/LiteralValue.hpp>
#include <WCDB/OrderingTerm.hpp>
#include <WCDB/Pragma.hpp>
#include <WCDB/QualifiedTable.hpp>
#include <WCDB/RaiseFunction.hpp>
#include <WCDB/RecursiveCTE.hpp>
#include <WCDB/ResultColumn.hpp>
#include <WCDB/Schema.hpp>
#include <WCDB/TableConstraint.hpp>
#include <WCDB/TableOrSubquery.hpp>
#include <WCDB/Upsert.hpp>
#include <WCDB/WindowDef.hpp>
#include <WCDB/WindowFunctionInvocation.hpp>
#include <WCDB/With.hpp>

#include <WCDB/Statement.hpp>
#include <WCDB/StatementAlterTable.hpp>
#include <WCDB/StatementAnalyze.hpp>
#include <WCDB/StatementAttach.hpp>
#include <WCDB/StatementBegin.hpp>
#include <WCDB/StatementCommit.hpp>
#include <WCDB/StatementCreateIndex.hpp>
#include <WCDB/StatementCreateTable.hpp>
#include <WCDB/StatementCreateTrigger.hpp>
#include <WCDB/StatementCreateView.hpp>
#include <WCDB/StatementCreateVirtualTable.hpp>
#include <WCDB/StatementDelete.hpp>
#include <WCDB/StatementDetach.hpp>
#include <WCDB/StatementDropIndex.hpp>
#include <WCDB/StatementDropTable.hpp>
#include <WCDB/StatementDropTrigger.hpp>
#include <WCDB/StatementDropView.hpp>
#include <WCDB/StatementInsert.hpp>
#include <WCDB/StatementPragma.hpp>
#include <WCDB/StatementReindex.hpp>
#include <WCDB/StatementRelease.hpp>
#include <WCDB/StatementRollback.hpp>
#include <WCDB/StatementSavepoint.hpp>
#include <WCDB/StatementSelect.hpp>
#include <WCDB/StatementUpdate.hpp>
#include <WCDB/StatementVacuum.hpp>

#include <WCDB/StatementExplain.hpp>

#include <WCDB/ConvertibleImplementation.hpp>
