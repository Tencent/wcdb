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

#pragma once

#include "Enum.hpp"
#include "SyntaxCommonConst.hpp"

namespace WCDB {

template<>
constexpr const char* Enum::description(const Syntax::ColumnType& columnType)
{
    switch (columnType) {
    case Syntax::ColumnType::Null:
        return "NULL";
    case Syntax::ColumnType::Integer:
        return "INTEGER";
    case Syntax::ColumnType::Float:
        return "REAL";
    case Syntax::ColumnType::Text:
        return "TEXT";
    case Syntax::ColumnType::BLOB:
        return "BLOB";
    }
}

template<>
constexpr const char* Enum::description(const Syntax::CompoundOperator& compoundOperator)
{
    switch (compoundOperator) {
    case Syntax::CompoundOperator::Union:
        return "UNION";
    case Syntax::CompoundOperator::UnionAll:
        return "UNION ALL";
    case Syntax::CompoundOperator::Intersect:
        return "INTERSECT";
    case Syntax::CompoundOperator::Except:
        return "EXCEPT";
    }
}

template<>
constexpr const char* Enum::description(const Syntax::Conflict& conflict)
{
    switch (conflict) {
    case Syntax::Conflict::Rollback:
        return "ON CONFLICT ROLLBACK";
    case Syntax::Conflict::Abort:
        return "ON CONFLICT ABORT";
    case Syntax::Conflict::Fail:
        return "ON CONFLICT FAIL";
    case Syntax::Conflict::Ignore:
        return "ON CONFLICT IGNORE";
    case Syntax::Conflict::Replace:
        return "ON CONFLICT REPLACE";
    }
}

template<>
constexpr const char* Enum::description(const Syntax::JoinOperator& joinOperator)
{
    switch (joinOperator) {
    case Syntax::JoinOperator::With:
        return ",";
    case Syntax::JoinOperator::Join:
        return "JOIN";
    case Syntax::JoinOperator::LeftOuterJoin:
        return "LEFT OUTER JOIN";
    case Syntax::JoinOperator::LeftJoin:
        return "LEFT JOIN";
    case Syntax::JoinOperator::InnerJoin:
        return "INNER JOIN";
    case Syntax::JoinOperator::CrossJoin:
        return "CROSS JOIN";
    case Syntax::JoinOperator::NaturalJoin:
        return "NATURAL JOIN";
    case Syntax::JoinOperator::NaturalLeftOuterJoin:
        return "NATURAL LEFT OUTER JOIN";
    case Syntax::JoinOperator::NaturalLeftJoin:
        return "NATURAL LEFT JOIN";
    case Syntax::JoinOperator::NaturalInnerJoin:
        return "NATURAL INNER JOIN";
    case Syntax::JoinOperator::NaturalCrossJoin:
        return "NATURAL CROSS JOIN";
    }
}

template<>
constexpr const char* Enum::description(const Syntax::Order& order)
{
    switch (order) {
    case Syntax::Order::ASC:
        return "ASC";
    case Syntax::Order::DESC:
        return "DESC";
    }
}

template<>
constexpr const char* Enum::description(const Syntax::ConflictAction& action)
{
    switch (action) {
    case Syntax::ConflictAction::Replace:
        return "OR REPLACE";
    case Syntax::ConflictAction::Rollback:
        return "OR ROLLBACK";
    case Syntax::ConflictAction::Abort:
        return "OR ABORT";
    case Syntax::ConflictAction::Fail:
        return "OR FAIL";
    case Syntax::ConflictAction::Ignore:
        return "OR IGNORE";
    }
}

template<>
constexpr const char* Enum::description(const Syntax::MatchType& match)
{
    switch (match) {
    case Syntax::MatchType::Simple:
        return "SIMPLE";
    case Syntax::MatchType::Full:
        return "FULL";
    case Syntax::MatchType::Partial:
        return "PARTIAL";
    }
}

} // namespace WCDB
