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

enum class LangConflictClause {
    NotSet,
    Rollback,
    Abort,
    Fail,
    Ignore,
    Replace,
};

constexpr const char *
LangConflictClauseName(const LangConflictClause &conflictClause)
{
    switch (conflictClause) {
        case LangConflictClause::Rollback:
            return "ROLLBACK";
        case LangConflictClause::Abort:
            return "ABORT";
        case LangConflictClause::Fail:
            return "FAIL";
        case LangConflictClause::Ignore:
            return "IGNORE";
        case LangConflictClause::Replace:
            return "REPLACE";
        default:
            return "";
    }
}

#endif /* LangConflictClause_hpp */
