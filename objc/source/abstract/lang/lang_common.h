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

#ifndef lang_common_h
#define lang_common_h

#include <assert.h>

#include <WCDB/copy_on_write_data.hpp>
#include <WCDB/copy_on_write_lazy_string_list.hpp>
#include <WCDB/copy_on_write_string.hpp>

#include <WCDB/copy_on_write_lazy_lang.hpp>

#include <WCDB/LangColumnType.hpp>
#include <WCDB/LangConflictClause.hpp>
#include <WCDB/LangOrder.hpp>

namespace WCDB {

namespace lang {

class Lang {
public:
    virtual copy_on_write_string SQL() const = 0;
};

class CRUDLang : public Lang {
};

class BindParameter;
class ColumnDef;
class ColumnConstraint;
class Expr;
class IndexedColumn;
class TableConstraint;
class ModuleArgument;
class WithClause;
class QualifiedTableName;
class OrderingTerm;
class LiteralValue;
class ResultColumn;
class SelectCore;
class TableOrSubquery;
class JoinClause;
class CommonTableExpression;
class ForeignKeyClause;
class CTETableName;
class JoinOperator;
class JoinConstraint;
class RaiseFunction;

class AnalyzeSTMT;
class AttachSTMT;
class BeginSTMT;
class CommitSTMT;
class CreateIndexSTMT;
class CreateTableSTMT;
class CreateTriggerSTMT;
class CreateViewSTMT;
class CreateVirtualTableSTMT;
class DeleteSTMT;
class DetachSTMT;
class DropIndexSTMT;
class DropTableSTMT;
class DropTriggerSTMT;
class DropViewSTMT;
class InsertSTMT;
class PragmaSTMT;
class ReindexSTMT;
class ReleaseSTMT;
class RollbackSTMT;
class SavepointSTMT;
class SelectSTMT;
class UpdateSTMT;
class VacuumSTMT;

} // namespace lang

} // namespace WCDB

    
#endif /* lang_common_h */
