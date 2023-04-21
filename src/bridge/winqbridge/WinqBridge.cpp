//
// Created by qiuwenchen on 2022/5/26.
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

#include "WinqBridge.hpp"
#include "ObjectBridge.hpp"
#include "Recyclable.hpp"
#include "SQL.hpp"
#include "Statement.hpp"

const char* _Nonnull WCDBWinqGetDescription(CPPObject* statement)
{
    WCDB::SQL* innerValue = (WCDB::SQL*) statement->realValue;
    return innerValue->getDescription().data();
}

CPPStatement WCDBGetUnmangedStatement(CPPObject* _Nonnull statement)
{
    WCDB::Statement* cppStatement
    = (WCDB::Statement*) WCDB::ObjectBridge::extractOriginalCPPObject(statement);
    return WCDBCreateUnmanagedCPPObject(CPPStatement, cppStatement);
}

namespace WCDB {

Order WinqBridge::changeOrder(enum WCDBSyntaxOrder order)
{
    switch (order) {
    case WCDBSyntaxOrder_Asc:
        return Order::ASC;
    case WCDBSyntaxOrder_Desc:
        return Order::DESC;
    }
}

Conflict WinqBridge::changeConflict(enum WCDBSyntaxConflictAction action)
{
    switch (action) {
    case WCDBSyntaxConflictAction_Fail:
        return Conflict::Fail;
    case WCDBSyntaxConflictAction_Abort:
        return Conflict::Abort;
    case WCDBSyntaxConflictAction_Ignore:
        return Conflict::Ignore;
    case WCDBSyntaxConflictAction_Replace:
        return Conflict::Replace;
    case WCDBSyntaxConflictAction_Rollback:
        return Conflict::Rollback;
    }
}

Syntax::ConflictAction WinqBridge::changeConflictAction(enum WCDBSyntaxConflictAction action)
{
    switch (action) {
    case WCDBSyntaxConflictAction_Fail:
        return Syntax::ConflictAction::Fail;
    case WCDBSyntaxConflictAction_Abort:
        return Syntax::ConflictAction::Abort;
    case WCDBSyntaxConflictAction_Ignore:
        return Syntax::ConflictAction::Ignore;
    case WCDBSyntaxConflictAction_Replace:
        return Syntax::ConflictAction::Replace;
    case WCDBSyntaxConflictAction_Rollback:
        return Syntax::ConflictAction::Rollback;
    }
}

Syntax::CompoundOperator
WinqBridge::changeCompoundOperator(enum WCDBSyntaxCompoundOperator compound)
{
    switch (compound) {
    case WCDBSyntaxCompoundOperator_Union:
        return Syntax::CompoundOperator::Union;
    case WCDBSyntaxCompoundOperator_Except:
        return Syntax::CompoundOperator::Except;
    case WCDBSyntaxCompoundOperator_Intersect:
        return Syntax::CompoundOperator::Intersect;
    case WCDBSyntaxCompoundOperator_UnionAll:
        return Syntax::CompoundOperator::UnionAll;
    }
}

Syntax::JoinOperator WinqBridge::changeJoinOperator(enum WCDBSyntaxJoinOperator join)
{
    switch (join) {
    case WCDBSyntaxJoinOperator_Join:
        return Syntax::JoinOperator::Join;
    case WCDBSyntaxJoinOperator_With:
        return Syntax::JoinOperator::With;
    case WCDBSyntaxJoinOperator_LeftJoin:
        return Syntax::JoinOperator::LeftJoin;
    case WCDBSyntaxJoinOperator_LeftOuterJoin:
        return Syntax::JoinOperator::LeftOuterJoin;
    case WCDBSyntaxJoinOperator_CrossJoin:
        return Syntax::JoinOperator::CrossJoin;
    case WCDBSyntaxJoinOperator_InnerJoin:
        return Syntax::JoinOperator::InnerJoin;
    case WCDBSyntaxJoinOperator_NaturalJoin:
        return Syntax::JoinOperator::NaturalJoin;
    case WCDBSyntaxJoinOperator_NaturalLeftJoin:
        return Syntax::JoinOperator::NaturalLeftJoin;
    case WCDBSyntaxJoinOperator_NaturalLeftOuterJoin:
        return Syntax::JoinOperator::NaturalLeftOuterJoin;
    case WCDBSyntaxJoinOperator_NaturalInnerJoin:
        return Syntax::JoinOperator::NaturalInnerJoin;
    case WCDBSyntaxJoinOperator_NaturalCrossJoin:
        return Syntax::JoinOperator::NaturalCrossJoin;
    }
}

Syntax::LimitParameterType
WinqBridge::changeLimitParameterType(enum WCDBSyntaxLimitParameterType type)
{
    switch (type) {
    case WCDBSyntaxLimitParameterType_End:
        return Syntax::LimitParameterType::End;
    case WCDBSyntaxLimitParameterType_NotSet:
        return Syntax::LimitParameterType::NotSet;
    case WCDBSyntaxLimitParameterType_Offset:
        return Syntax::LimitParameterType::Offset;
    }
}

MatchType WinqBridge::changeMatchType(enum WCDBSyntaxMatchType type)
{
    switch (type) {
    case WCDBSyntaxMatchType_Full:
        return MatchType::Full;
    case WCDBSyntaxMatchType_Simple:
        return MatchType::Simple;
    case WCDBSyntaxMatchType_Patial:
        return MatchType::Partial;
    }
}

ColumnType WinqBridge::changeColumnType(enum WCDBSyntaxColumnType type)
{
    switch (type) {
    case WCDBSyntaxColumnType_Null:
        return ColumnType::Null;
    case WCDBSyntaxColumnType_Integer:
        return ColumnType::Integer;
    case WCDBSyntaxColumnType_Float:
        return ColumnType::Float;
    case WCDBSyntaxColumnType_Text:
        return ColumnType::Text;
    case WCDBSyntaxColumnType_BLOB:
        return ColumnType::BLOB;
    }
}

} // namespace WCDB
