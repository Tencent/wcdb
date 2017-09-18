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

#include <WCDB/describable.hpp>

#include <WCDB/clause_join.hpp>
#include <WCDB/column.hpp>
#include <WCDB/column_def.hpp>
#include <WCDB/column_index.hpp>
#include <WCDB/column_result.hpp>
#include <WCDB/column_type.hpp>
#include <WCDB/constraint_table.hpp>
#include <WCDB/expr.hpp>
#include <WCDB/literal_value.hpp>
#include <WCDB/module_argument.hpp>
#include <WCDB/order.hpp>
#include <WCDB/pragma.hpp>
#include <WCDB/subquery.hpp>

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
