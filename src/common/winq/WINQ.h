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

#include "SQL.hpp"

#include "AggregateFunction.hpp"
#include "ColumnType.hpp"
#include "Convertible.hpp"
#include "CoreFunction.hpp"
#include "ExpressionOperable.hpp"
#include "FTSFunction.hpp"

#include "BindParameter.hpp"
#include "Column.hpp"
#include "ColumnConstraint.hpp"
#include "ColumnDef.hpp"
#include "CommonTableExpression.hpp"
#include "Expression.hpp"
#include "Filter.hpp"
#include "ForeignKey.hpp"
#include "FrameSpec.hpp"
#include "IndexedColumn.hpp"
#include "Join.hpp"
#include "JoinConstraint.hpp"
#include "LiteralValue.hpp"
#include "OrderingTerm.hpp"
#include "Pragma.hpp"
#include "QualifiedTable.hpp"
#include "RaiseFunction.hpp"
#include "ResultColumn.hpp"
#include "Schema.hpp"
#include "TableConstraint.hpp"
#include "TableOrSubquery.hpp"
#include "Upsert.hpp"
#include "WindowDef.hpp"

#include "Statement.hpp"
#include "StatementAlterTable.hpp"
#include "StatementAnalyze.hpp"
#include "StatementAttach.hpp"
#include "StatementBegin.hpp"
#include "StatementCommit.hpp"
#include "StatementCreateIndex.hpp"
#include "StatementCreateTable.hpp"
#include "StatementCreateTrigger.hpp"
#include "StatementCreateView.hpp"
#include "StatementCreateVirtualTable.hpp"
#include "StatementDelete.hpp"
#include "StatementDetach.hpp"
#include "StatementDropIndex.hpp"
#include "StatementDropTable.hpp"
#include "StatementDropTrigger.hpp"
#include "StatementDropView.hpp"
#include "StatementInsert.hpp"
#include "StatementPragma.hpp"
#include "StatementReindex.hpp"
#include "StatementRelease.hpp"
#include "StatementRollback.hpp"
#include "StatementSavepoint.hpp"
#include "StatementSelect.hpp"
#include "StatementUpdate.hpp"
#include "StatementVacuum.hpp"

#include "StatementExplain.hpp"

#include "ConvertibleImplementation.hpp"
