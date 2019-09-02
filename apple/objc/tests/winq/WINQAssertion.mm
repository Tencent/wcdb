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

#import "WINQAssertion.h"

namespace WCDB {

NSString* getDescriptionOfType(WCDB::Syntax::Identifier::Type type)
{
    switch (type) {
    case Syntax::Identifier::Type::Column:
        return @"Column";
    case Syntax::Identifier::Type::Schema:
        return @"Schema";
    case Syntax::Identifier::Type::ColumnDef:
        return @"ColumnDef";
    case Syntax::Identifier::Type::ColumnConstraint:
        return @"ColumnConstraint";
    case Syntax::Identifier::Type::Expression:
        return @"Expression";
    case Syntax::Identifier::Type::LiteralValue:
        return @"LiteralValue";
    case Syntax::Identifier::Type::ForeignKeyClause:
        return @"ForeignKeyClause";
    case Syntax::Identifier::Type::BindParameter:
        return @"BindParameter";
    case Syntax::Identifier::Type::RaiseFunction:
        return @"RaiseFunction";
    case Syntax::Identifier::Type::WindowDef:
        return @"WindowDef";
    case Syntax::Identifier::Type::Filter:
        return @"Filter";
    case Syntax::Identifier::Type::IndexedColumn:
        return @"IndexedColumn";
    case Syntax::Identifier::Type::TableConstraint:
        return @"TableConstraint";
    case Syntax::Identifier::Type::CTETableName:
        return @"CTETableName";
    case Syntax::Identifier::Type::WithClause:
        return @"WithClause";
    case Syntax::Identifier::Type::RecursiveCTE:
        return @"RecursiveCTE";
    case Syntax::Identifier::Type::CommonTableExpression:
        return @"CommonTableExpression";
    case Syntax::Identifier::Type::QualifiedTableName:
        return @"QualifiedTableName";
    case Syntax::Identifier::Type::OrderingTerm:
        return @"OrderingTerm";
    case Syntax::Identifier::Type::UpsertClause:
        return @"UpsertClause";
    case Syntax::Identifier::Type::Pragma:
        return @"Pragma";
    case Syntax::Identifier::Type::JoinClause:
        return @"JoinClause";
    case Syntax::Identifier::Type::TableOrSubquery:
        return @"TableOrSubquery";
    case Syntax::Identifier::Type::JoinConstraint:
        return @"JoinConstraint";
    case Syntax::Identifier::Type::SelectCore:
        return @"SelectCore";
    case Syntax::Identifier::Type::ResultColumn:
        return @"ResultColumn";
    case Syntax::Identifier::Type::FrameSpec:
        return @"FrameSpec";
    case Syntax::Identifier::Type::FunctionInvocation:
        return @"FunctionInvocation";
    case Syntax::Identifier::Type::WindowFunctionInvocation:
        return @"WindowFunctionInvocation";
    case Syntax::Identifier::Type::AlterTableSTMT:
        return @"AlterTableSTMT";
    case Syntax::Identifier::Type::AnalyzeSTMT:
        return @"AnalyzeSTMT";
    case Syntax::Identifier::Type::AttachSTMT:
        return @"AttachSTMT";
    case Syntax::Identifier::Type::BeginSTMT:
        return @"BeginSTMT";
    case Syntax::Identifier::Type::CommitSTMT:
        return @"CommitSTMT";
    case Syntax::Identifier::Type::RollbackSTMT:
        return @"RollbackSTMT";
    case Syntax::Identifier::Type::SavepointSTMT:
        return @"SavepointSTMT";
    case Syntax::Identifier::Type::ReleaseSTMT:
        return @"ReleaseSTMT";
    case Syntax::Identifier::Type::CreateIndexSTMT:
        return @"CreateIndexSTMT";
    case Syntax::Identifier::Type::CreateTableSTMT:
        return @"CreateTableSTMT";
    case Syntax::Identifier::Type::CreateTriggerSTMT:
        return @"CreateTriggerSTMT";
    case Syntax::Identifier::Type::SelectSTMT:
        return @"SelectSTMT";
    case Syntax::Identifier::Type::InsertSTMT:
        return @"InsertSTMT";
    case Syntax::Identifier::Type::DeleteSTMT:
        return @"DeleteSTMT";
    case Syntax::Identifier::Type::UpdateSTMT:
        return @"UpdateSTMT";
    case Syntax::Identifier::Type::CreateViewSTMT:
        return @"CreateViewSTMT";
    case Syntax::Identifier::Type::CreateVirtualTableSTMT:
        return @"CreateVirtualTableSTMT";
    case Syntax::Identifier::Type::DetachSTMT:
        return @"DetachSTMT";
    case Syntax::Identifier::Type::DropIndexSTMT:
        return @"DropIndexSTMT";
    case Syntax::Identifier::Type::DropTableSTMT:
        return @"DropTableSTMT";
    case Syntax::Identifier::Type::DropTriggerSTMT:
        return @"DropTriggerSTMT";
    case Syntax::Identifier::Type::DropViewSTMT:
        return @"DropViewSTMT";
    case Syntax::Identifier::Type::PragmaSTMT:
        return @"PragmaSTMT";
    case Syntax::Identifier::Type::ReindexSTMT:
        return @"ReindexSTMT";
    case Syntax::Identifier::Type::VacuumSTMT:
        return @"VacuumSTMT";
    case Syntax::Identifier::Type::ExplainSTMT:
        return @"ExplainSTMT";
    }
}

std::list<WCDB::Syntax::Identifier::Type> getTypesOfSQL(WCDB::SQL& sql)
{
    std::list<WCDB::Syntax::Identifier::Type> types;
    sql.iterate([&types](WCDB::Syntax::Identifier& identifier, bool& stop) {
        WCDB_UNUSED(stop)
        types.push_back(identifier.getType());
    });
    return types;
}

NSString* getDescriptionOfTypes(const std::list<WCDB::Syntax::Identifier::Type>& types)
{
    NSMutableString* description = [NSMutableString string];
    BOOL comma = NO;
    for (const auto& type : types) {
        if (comma) {
            [description appendString:@", "];
        } else {
            comma = true;
        }
        [description appendString:getDescriptionOfType(type)];
    }
    return description;
}

}
