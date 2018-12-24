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

#ifndef _WCDB_SYNTAXCOMMONCONST_HPP
#define _WCDB_SYNTAXCOMMONCONST_HPP

#include <WCDB/Enum.hpp>

namespace WCDB {

namespace Syntax {

enum class LimitParameterType {
    NotSet,
    Offset,
    End,
};

enum class ConflictAction {
    Replace,
    Rollback,
    Abort,
    Fail,
    Ignore,
};

static const char* masterTable = "sqlite_master";
static const char* mainSchema = "main";
static const char* tempSchema = "temp";

} // namespace Syntax

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

} // namespace WCDB

#endif /* _WCDB_SYNTAXCOMMONCONST_HPP */
