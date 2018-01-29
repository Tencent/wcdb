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

#ifndef abstract_h
#define abstract_h

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
       Further more, IDE can give developers a code hint so that developers have less need to check the syntax documentation.
 *  2. More importantly, WINQ gives WCDB an ability to recognize, check and hook the SQL to do some smart feature, such as data migration. This kind of feature may not be implemented in current version, but it's already in our roadmap.
 *
 */

#include <WCDB/describable.hpp>

#include <WCDB/convertible.hpp>
#include <WCDB/convertible_impl.hpp>

#include <WCDB/operable.hpp>
#include <WCDB/operable_impl.hpp>

#include <WCDB/redirectable.hpp>

#include <WCDB/clause_join.hpp>
#include <WCDB/column.hpp>
#include <WCDB/column_def.hpp>
#include <WCDB/column_index.hpp>
#include <WCDB/column_result.hpp>
#include <WCDB/column_type.hpp>
#include <WCDB/expression.hpp>
#include <WCDB/foreign_key.hpp>
#include <WCDB/literal_value.hpp>
#include <WCDB/module_argument.hpp>
#include <WCDB/order.hpp>
#include <WCDB/pragma.hpp>
#include <WCDB/subquery.hpp>
#include <WCDB/table_constraint.hpp>

#include <WCDB/statement.hpp>
#include <WCDB/statement_alter_table.hpp>
#include <WCDB/statement_attach.hpp>
#include <WCDB/statement_create_index.hpp>
#include <WCDB/statement_create_table.hpp>
#include <WCDB/statement_create_virtual_table.hpp>
#include <WCDB/statement_delete.hpp>
#include <WCDB/statement_detach.hpp>
#include <WCDB/statement_drop_index.hpp>
#include <WCDB/statement_drop_table.hpp>
#include <WCDB/statement_explain.hpp>
#include <WCDB/statement_insert.hpp>
#include <WCDB/statement_pragma.hpp>
#include <WCDB/statement_reindex.hpp>
#include <WCDB/statement_release.hpp>
#include <WCDB/statement_rollback.hpp>
#include <WCDB/statement_savepoint.hpp>
#include <WCDB/statement_select.hpp>
#include <WCDB/statement_transaction.hpp>
#include <WCDB/statement_update.hpp>
#include <WCDB/statement_vacuum.hpp>

#endif /* abstract_h */
