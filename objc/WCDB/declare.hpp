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

#ifndef declare_hpp
#define declare_hpp

#include <WCDB/conflict.hpp>
#include <atomic>
#include <cassert>
#include <functional>
#include <list>
#include <string>
#include <utility>

namespace WCDB {

class Column;
class ColumnDef;
class ColumnIndex;
class ColumnResult;
class Describable;
class Expr;
class JoinClause;
class Order;
class Pragma;
class StatementHandle;
class Handle;
class ModuleArgument;
class Statement;
class StatementAlterTable;
class StatementCreateIndex;
class StatementCreateTable;
class StatementDelete;
class StatementDropIndex;
class StatementDropTable;
class StatementInsert;
class StatementPragma;
class StatementSelect;
class StatementTransaction;
class StatementUpdate;
class StatementCreateVirtualTable;
class Subquery;
class TableConstraint;

typedef std::list<const Column> ColumnList;
typedef std::list<const ColumnDef> ColumnDefList;
typedef std::list<const ColumnIndex> ColumnIndexList;
typedef std::list<const ColumnResult> ColumnResultList;
typedef std::list<const Expr> ExprList;
typedef std::list<const Order> OrderList;
typedef std::list<const StatementSelect> StatementSelectList;
typedef std::list<const Subquery> SubqueryList;
typedef std::list<const TableConstraint> TableConstraintList;
typedef std::pair<const Column, const Expr> UpdateValue;
typedef std::list<const UpdateValue> UpdateValueList;
typedef std::list<const ModuleArgument> ModuleArgumentList;

}; //namespace WCDB

#endif /* declare_hpp */
