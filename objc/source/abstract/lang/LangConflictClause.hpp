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

#ifndef LangConflictClause_hpp
#define LangConflictClause_hpp

namespace WCDB {

namespace lang {

enum class ConflictClause {
    NotSet,
    Rollback,
    Abort,
    Fail,
    Ignore,
    Replace,
};

constexpr const char *
LangConflictClauseName(const ConflictClause &conflictClause)
{
    switch (conflictClause) {
        case ConflictClause::Rollback:
            return "ON CONFLICT ROLLBACK";
        case ConflictClause::Abort:
            return "ON CONFLICT ABORT";
        case ConflictClause::Fail:
            return "ON CONFLICT FAIL";
        case ConflictClause::Ignore:
            return "ON CONFLICT IGNORE";
        case ConflictClause::Replace:
            return "ON CONFLICT REPLACE";
        default:
            return "";
    }
}

} // namespace lang

} // namespace WCDB

#endif /* LangConflictClause_hpp */
