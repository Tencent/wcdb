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

#include <WCDB/Convertible.hpp>
#include <WCDB/Operable.hpp>
#include <WCDB/Redirectable.hpp>

#include <WCDB/BindParameter.hpp>
#include <WCDB/CTETableName.hpp>
#include <WCDB/Column.hpp>
#include <WCDB/ColumnConstraint.hpp>
#include <WCDB/ColumnDef.hpp>
#include <WCDB/CommonTableExpression.hpp>
#include <WCDB/Expression.hpp>
#include <WCDB/ForeignKeyClause.hpp>
#include <WCDB/IndexedColumn.hpp>
#include <WCDB/JoinClause.hpp>
#include <WCDB/JoinConstraint.hpp>
#include <WCDB/LiteralValue.hpp>
#include <WCDB/ModuleArgument.hpp>
#include <WCDB/OrderingTerm.hpp>
#include <WCDB/Pragma.hpp>
#include <WCDB/QualifiedTableName.hpp>
#include <WCDB/RaiseFunction.hpp>
#include <WCDB/ResultColumn.hpp>
#include <WCDB/Schema.hpp>
#include <WCDB/SelectCore.hpp>
#include <WCDB/TableConstraint.hpp>
#include <WCDB/TableOrSubquery.hpp>
#include <WCDB/WithClause.hpp>

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

#include <WCDB/ColumnType.hpp>

#include <WCDB/ConvertibleImpl.hpp>

#endif /* WINQ_h */
