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

#ifndef WINQ_h
#define WINQ_h

/*  How WINQ implement?
 *  
 *  It's implemented by C++ class, function and template feature.
 *
 *  As the [SQL syntax](http://www.sqlite.org/lang.html) documented,
 *
 *  1. For those fixed big-letter **keywords**, they will be implemented as function, while the keyword will be the function name.
 *
 *      e.g. `ALTER TABLE` keyword in [`alter-table-stmt`](http://www.sqlite.org/syntax/alter-table-stmt.html) is one of the functions in `StatementAlterTable`.
 *
 *  2. For those small-letter **tokens**, they will be implemented as a class , enum type or pure string.
 *
 *      e.g. `column-def` token in [`alter-table-stmt`](http://www.sqlite.org/syntax/alter-table-stmt.html) is a separated class.
 *
 *     WCDB divides tokens into two different types - statements and other tokens.
 *     A statement can be prepared or executed and all its functions will return itself.
 *     Instead, a non-statement token is the parameter of statement and it will return a new copy or have no return.
 *
 *  Why we need WINQ?
 *
 *  1. With a pure string SQL, IDE can't recognize it so that developers can't find the syntax error until it runs.
 *     Instead, it will result in a compiler error for a syntax error while writing WINQ, which can let developers notice the error much ealier.
 *     Further more, IDE can give developers a code hint so that developers have less need to check the syntax documentation.
 *  2. More importantly, WINQ gives WCDB an ability to recognize, check and hook the SQL to do some smart feature, such as data migration. This kind of feature may not be implemented in current version, but it's already in our roadmap.
 *
 */

#include <WINQ/BindParameter.hpp>
#include <WINQ/CTETableName.hpp>
#include <WINQ/Column.hpp>
#include <WINQ/ColumnConstraint.hpp>
#include <WINQ/ColumnDef.hpp>
#include <WINQ/CommonTableExpression.hpp>
#include <WINQ/Expression.hpp>
#include <WINQ/ForeignKeyClause.hpp>
#include <WINQ/IndexedColumn.hpp>
#include <WINQ/JoinClause.hpp>
#include <WINQ/JoinConstraint.hpp>
#include <WINQ/LiteralValue.hpp>
#include <WINQ/ModuleArgument.hpp>
#include <WINQ/OrderingTerm.hpp>
#include <WINQ/Pragma.hpp>
#include <WINQ/QualifiedTableName.hpp>
#include <WINQ/RaiseFunction.hpp>
#include <WINQ/ResultColumn.hpp>
#include <WINQ/SelectCore.hpp>
#include <WINQ/TableConstraint.hpp>
#include <WINQ/TableOrSubquery.hpp>
#include <WINQ/WithClause.hpp>

#include <WINQ/StatementAlterTable.hpp>
#include <WINQ/StatementAnalyze.hpp>
#include <WINQ/StatementAttach.hpp>
#include <WINQ/StatementBegin.hpp>
#include <WINQ/StatementCommit.hpp>
#include <WINQ/StatementCreateIndex.hpp>
#include <WINQ/StatementCreateTable.hpp>
#include <WINQ/StatementCreateTrigger.hpp>
#include <WINQ/StatementCreateView.hpp>
#include <WINQ/StatementCreateVirtualTable.hpp>
#include <WINQ/StatementDelete.hpp>
#include <WINQ/StatementDetach.hpp>
#include <WINQ/StatementDropIndex.hpp>
#include <WINQ/StatementDropTable.hpp>
#include <WINQ/StatementDropTrigger.hpp>
#include <WINQ/StatementDropView.hpp>
#include <WINQ/StatementInsert.hpp>
#include <WINQ/StatementPragma.hpp>
#include <WINQ/StatementReindex.hpp>
#include <WINQ/StatementRelease.hpp>
#include <WINQ/StatementRollback.hpp>
#include <WINQ/StatementSavepoint.hpp>
#include <WINQ/StatementSelect.hpp>
#include <WINQ/StatementUpdate.hpp>
#include <WINQ/StatementVacuum.hpp>

#endif /* WINQ_h */
