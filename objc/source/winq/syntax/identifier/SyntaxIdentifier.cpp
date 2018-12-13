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

#include <WCDB/Assertion.hpp>
#include <WCDB/Syntax.h>

namespace WCDB {

namespace Syntax {

Identifier::~Identifier()
{
}

void Identifier::iterate(const Iterator &iterator)
{
    bool stop = false;
    iterate(iterator, stop);
}

void Identifier::recursiveIterate(Identifier &identifier, const Iterator &iterator, bool &stop)
{
    identifier.iterate(iterator, stop);
}

void Identifier::iterate(const Iterator &iterator, bool &stop)
{
    if (!stop) {
        iterator(*this, stop);
    }
}

Identifier *Identifier::clone() const
{
    switch (getType()) {
    case Type::Column:
        return new Column(*static_cast<const Column *>(this));
    case Type::Schema:
        return new Schema(*static_cast<const Schema *>(this));
    case Type::ColumnDef:
        return new ColumnDef(*static_cast<const ColumnDef *>(this));
    case Type::ColumnConstraint:
        return new ColumnConstraint(*static_cast<const ColumnConstraint *>(this));
    case Type::Expression:
        return new Expression(*static_cast<const Expression *>(this));
    case Type::LiteralValue:
        return new LiteralValue(*static_cast<const LiteralValue *>(this));
    case Type::ForeignKeyClause:
        return new ForeignKeyClause(*static_cast<const ForeignKeyClause *>(this));
    case Type::BindParameter:
        return new BindParameter(*static_cast<const BindParameter *>(this));
    case Type::RaiseFunction:
        return new RaiseFunction(*static_cast<const RaiseFunction *>(this));
    case Type::WindowDef:
        return new WindowDef(*static_cast<const WindowDef *>(this));
    case Type::Filter:
        return new Filter(*static_cast<const Filter *>(this));
    case Type::IndexedColumn:
        return new IndexedColumn(*static_cast<const IndexedColumn *>(this));
    case Type::TableConstraint:
        return new TableConstraint(*static_cast<const TableConstraint *>(this));
    case Type::CTETableName:
        return new CTETableName(*static_cast<const CTETableName *>(this));
    case Type::WithClause:
        return new WithClause(*static_cast<const WithClause *>(this));
    case Type::RecursiveCTE:
        return new RecursiveCTE(*static_cast<const RecursiveCTE *>(this));
    case Type::CommonTableExpression:
        return new CommonTableExpression(*static_cast<const CommonTableExpression *>(this));
    case Type::QualifiedTableName:
        return new QualifiedTableName(*static_cast<const QualifiedTableName *>(this));
    case Type::OrderingTerm:
        return new OrderingTerm(*static_cast<const OrderingTerm *>(this));
    case Type::UpsertClause:
        return new UpsertClause(*static_cast<const UpsertClause *>(this));
    case Type::Pragma:
        return new Pragma(*static_cast<const Pragma *>(this));
    case Type::JoinClause:
        return new JoinClause(*static_cast<const JoinClause *>(this));
    case Type::TableOrSubquery:
        return new TableOrSubquery(*static_cast<const TableOrSubquery *>(this));
    case Type::JoinConstraint:
        return new JoinConstraint(*static_cast<const JoinConstraint *>(this));
    case Type::SelectCore:
        return new SelectCore(*static_cast<const SelectCore *>(this));
    case Type::ResultColumn:
        return new ResultColumn(*static_cast<const ResultColumn *>(this));
    case Type::FrameSpec:
        return new FrameSpec(*static_cast<const FrameSpec *>(this));
    case Type::FunctionInvocation:
        return new FunctionInvocation(*static_cast<const FunctionInvocation *>(this));
    case Type::WindowFunctionInvocation:
        return new WindowFunctionInvocation(
        *static_cast<const WindowFunctionInvocation *>(this));
    case Type::AlterTableSTMT:
        return new AlterTableSTMT(*static_cast<const AlterTableSTMT *>(this));
    case Type::AnalyzeSTMT:
        return new AnalyzeSTMT(*static_cast<const AnalyzeSTMT *>(this));
    case Type::AttachSTMT:
        return new AttachSTMT(*static_cast<const AttachSTMT *>(this));
    case Type::BeginSTMT:
        return new BeginSTMT(*static_cast<const BeginSTMT *>(this));
    case Type::CommitSTMT:
        return new CommitSTMT(*static_cast<const CommitSTMT *>(this));
    case Type::RollbackSTMT:
        return new RollbackSTMT(*static_cast<const RollbackSTMT *>(this));
    case Type::SavepointSTMT:
        return new SavepointSTMT(*static_cast<const SavepointSTMT *>(this));
    case Type::ReleaseSTMT:
        return new ReleaseSTMT(*static_cast<const ReleaseSTMT *>(this));
    case Type::CreateIndexSTMT:
        return new CreateIndexSTMT(*static_cast<const CreateIndexSTMT *>(this));
    case Type::CreateTableSTMT:
        return new CreateTableSTMT(*static_cast<const CreateTableSTMT *>(this));
    case Type::CreateTriggerSTMT:
        return new CreateTriggerSTMT(*static_cast<const CreateTriggerSTMT *>(this));
    case Type::SelectSTMT:
        return new SelectSTMT(*static_cast<const SelectSTMT *>(this));
    case Type::InsertSTMT:
        return new InsertSTMT(*static_cast<const InsertSTMT *>(this));
    case Type::DeleteSTMT:
        return new DeleteSTMT(*static_cast<const DeleteSTMT *>(this));
    case Type::UpdateSTMT:
        return new UpdateSTMT(*static_cast<const UpdateSTMT *>(this));
    case Type::CreateViewSTMT:
        return new CreateViewSTMT(*static_cast<const CreateViewSTMT *>(this));
    case Type::CreateVirtualTableSTMT:
        return new CreateVirtualTableSTMT(*static_cast<const CreateVirtualTableSTMT *>(this));
    case Type::DetachSTMT:
        return new DetachSTMT(*static_cast<const DetachSTMT *>(this));
    case Type::DropIndexSTMT:
        return new DropIndexSTMT(*static_cast<const DropIndexSTMT *>(this));
    case Type::DropTableSTMT:
        return new DropTableSTMT(*static_cast<const DropTableSTMT *>(this));
    case Type::DropTriggerSTMT:
        return new DropTriggerSTMT(*static_cast<const DropTriggerSTMT *>(this));
    case Type::DropViewSTMT:
        return new DropViewSTMT(*static_cast<const DropViewSTMT *>(this));
    case Type::PragmaSTMT:
        return new PragmaSTMT(*static_cast<const PragmaSTMT *>(this));
    case Type::ReindexSTMT:
        return new ReindexSTMT(*static_cast<const ReindexSTMT *>(this));
    case Type::VacuumSTMT:
        return new VacuumSTMT(*static_cast<const VacuumSTMT *>(this));
    }
}

} // namespace Syntax

} // namespace WCDB

std::ostream &operator<<(std::ostream &stream, const WCDB::Syntax::Identifier &identifier)
{
    stream << identifier.getDescription();
    return stream;
}
