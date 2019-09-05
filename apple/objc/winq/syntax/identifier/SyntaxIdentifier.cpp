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

#include <WCDB/Syntax.h>
#include <WCDB/SyntaxAssertion.hpp>

namespace WCDB {

namespace Syntax {

Identifier::~Identifier() = default;

StringView Identifier::getDescription() const
{
    if (isValid()) {
        std::ostringstream stream;
        if (describle(stream)) {
            return stream.str();
        }
        WCTAssert(false);
    }
    return StringView();
}

void Identifier::iterate(const Iterator &iterator)
{
    if (isValid()) {
        bool stop = false;
        iterate(iterator, stop);
    }
}

void Identifier::recursiveIterate(Identifier &identifier, const Iterator &iterator, bool &stop)
{
    identifier.iterate(iterator, stop);
}

void Identifier::iterate(const Iterator &iterator, bool &stop)
{
    if (!stop && isValid()) {
        iterator(*this, stop);
    }
}

std::unique_ptr<Identifier> Identifier::clone() const
{
    switch (getType()) {
    case Type::Column:
        return std::make_unique<Column>(*static_cast<const Column *>(this));
    case Type::Schema:
        return std::make_unique<Schema>(*static_cast<const Schema *>(this));
    case Type::ColumnDef:
        return std::make_unique<ColumnDef>(*static_cast<const ColumnDef *>(this));
    case Type::ColumnConstraint:
        return std::make_unique<ColumnConstraint>(
        *static_cast<const ColumnConstraint *>(this));
    case Type::Expression:
        return std::make_unique<Expression>(*static_cast<const Expression *>(this));
    case Type::LiteralValue:
        return std::make_unique<LiteralValue>(*static_cast<const LiteralValue *>(this));
    case Type::ForeignKeyClause:
        return std::make_unique<ForeignKeyClause>(
        *static_cast<const ForeignKeyClause *>(this));
    case Type::BindParameter:
        return std::make_unique<BindParameter>(*static_cast<const BindParameter *>(this));
    case Type::RaiseFunction:
        return std::make_unique<RaiseFunction>(*static_cast<const RaiseFunction *>(this));
    case Type::WindowDef:
        return std::make_unique<WindowDef>(*static_cast<const WindowDef *>(this));
    case Type::Filter:
        return std::make_unique<Filter>(*static_cast<const Filter *>(this));
    case Type::IndexedColumn:
        return std::make_unique<IndexedColumn>(*static_cast<const IndexedColumn *>(this));
    case Type::TableConstraint:
        return std::make_unique<TableConstraint>(*static_cast<const TableConstraint *>(this));
    case Type::CTETableName:
        return std::make_unique<CTETableName>(*static_cast<const CTETableName *>(this));
    case Type::WithClause:
        return std::make_unique<WithClause>(*static_cast<const WithClause *>(this));
    case Type::RecursiveCTE:
        return std::make_unique<RecursiveCTE>(*static_cast<const RecursiveCTE *>(this));
    case Type::CommonTableExpression:
        return std::make_unique<CommonTableExpression>(
        *static_cast<const CommonTableExpression *>(this));
    case Type::QualifiedTableName:
        return std::make_unique<QualifiedTableName>(
        *static_cast<const QualifiedTableName *>(this));
    case Type::OrderingTerm:
        return std::make_unique<OrderingTerm>(*static_cast<const OrderingTerm *>(this));
    case Type::UpsertClause:
        return std::make_unique<UpsertClause>(*static_cast<const UpsertClause *>(this));
    case Type::Pragma:
        return std::make_unique<Pragma>(*static_cast<const Pragma *>(this));
    case Type::JoinClause:
        return std::make_unique<JoinClause>(*static_cast<const JoinClause *>(this));
    case Type::TableOrSubquery:
        return std::make_unique<TableOrSubquery>(*static_cast<const TableOrSubquery *>(this));
    case Type::JoinConstraint:
        return std::make_unique<JoinConstraint>(*static_cast<const JoinConstraint *>(this));
        //    case Type::SelectCore:
        //        return std::make_unique<SelectCore>(*static_cast<const SelectCore *>(this));
    case Type::ResultColumn:
        return std::make_unique<ResultColumn>(*static_cast<const ResultColumn *>(this));
    case Type::FrameSpec:
        return std::make_unique<FrameSpec>(*static_cast<const FrameSpec *>(this));
    case Type::FunctionInvocation:
        return std::make_unique<FunctionInvocation>(
        *static_cast<const FunctionInvocation *>(this));
    case Type::WindowFunctionInvocation:
        return std::make_unique<WindowFunctionInvocation>(
        *static_cast<const WindowFunctionInvocation *>(this));
    case Type::AlterTableSTMT:
        return std::make_unique<AlterTableSTMT>(*static_cast<const AlterTableSTMT *>(this));
    case Type::AnalyzeSTMT:
        return std::make_unique<AnalyzeSTMT>(*static_cast<const AnalyzeSTMT *>(this));
    case Type::AttachSTMT:
        return std::make_unique<AttachSTMT>(*static_cast<const AttachSTMT *>(this));
    case Type::BeginSTMT:
        return std::make_unique<BeginSTMT>(*static_cast<const BeginSTMT *>(this));
    case Type::CommitSTMT:
        return std::make_unique<CommitSTMT>(*static_cast<const CommitSTMT *>(this));
    case Type::RollbackSTMT:
        return std::make_unique<RollbackSTMT>(*static_cast<const RollbackSTMT *>(this));
    case Type::SavepointSTMT:
        return std::make_unique<SavepointSTMT>(*static_cast<const SavepointSTMT *>(this));
    case Type::ReleaseSTMT:
        return std::make_unique<ReleaseSTMT>(*static_cast<const ReleaseSTMT *>(this));
    case Type::CreateIndexSTMT:
        return std::make_unique<CreateIndexSTMT>(*static_cast<const CreateIndexSTMT *>(this));
    case Type::CreateTableSTMT:
        return std::make_unique<CreateTableSTMT>(*static_cast<const CreateTableSTMT *>(this));
    case Type::CreateTriggerSTMT:
        return std::make_unique<CreateTriggerSTMT>(
        *static_cast<const CreateTriggerSTMT *>(this));
    case Type::SelectSTMT:
        return std::make_unique<SelectSTMT>(*static_cast<const SelectSTMT *>(this));
    case Type::InsertSTMT:
        return std::make_unique<InsertSTMT>(*static_cast<const InsertSTMT *>(this));
    case Type::DeleteSTMT:
        return std::make_unique<DeleteSTMT>(*static_cast<const DeleteSTMT *>(this));
    case Type::UpdateSTMT:
        return std::make_unique<UpdateSTMT>(*static_cast<const UpdateSTMT *>(this));
    case Type::CreateViewSTMT:
        return std::make_unique<CreateViewSTMT>(*static_cast<const CreateViewSTMT *>(this));
    case Type::CreateVirtualTableSTMT:
        return std::make_unique<CreateVirtualTableSTMT>(
        *static_cast<const CreateVirtualTableSTMT *>(this));
    case Type::DetachSTMT:
        return std::make_unique<DetachSTMT>(*static_cast<const DetachSTMT *>(this));
    case Type::DropIndexSTMT:
        return std::make_unique<DropIndexSTMT>(*static_cast<const DropIndexSTMT *>(this));
    case Type::DropTableSTMT:
        return std::make_unique<DropTableSTMT>(*static_cast<const DropTableSTMT *>(this));
    case Type::DropTriggerSTMT:
        return std::make_unique<DropTriggerSTMT>(*static_cast<const DropTriggerSTMT *>(this));
    case Type::DropViewSTMT:
        return std::make_unique<DropViewSTMT>(*static_cast<const DropViewSTMT *>(this));
    case Type::PragmaSTMT:
        return std::make_unique<PragmaSTMT>(*static_cast<const PragmaSTMT *>(this));
    case Type::ReindexSTMT:
        return std::make_unique<ReindexSTMT>(*static_cast<const ReindexSTMT *>(this));
    case Type::VacuumSTMT:
        return std::make_unique<VacuumSTMT>(*static_cast<const VacuumSTMT *>(this));
    case Type::ExplainSTMT:
        return std::make_unique<ExplainSTMT>(*static_cast<const ExplainSTMT *>(this));
    default:
        WCTAssert(false);
        return nullptr;
    }
}

STMT::~STMT() = default;

} // namespace Syntax

} // namespace WCDB

std::ostream &operator<<(std::ostream &stream, const WCDB::Syntax::Identifier &identifier)
{
    stream << identifier.getDescription();
    return stream;
}
