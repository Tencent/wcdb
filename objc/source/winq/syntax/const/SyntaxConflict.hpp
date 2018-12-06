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

#ifndef _WCDB_SYNTAXCONFLICT_HPP
#define _WCDB_SYNTAXCONFLICT_HPP

#include <WCDB/Enum.hpp>

namespace WCDB {

namespace Syntax {

enum class Conflict {
    Rollback,
    Abort,
    Fail,
    Ignore,
    Replace,
};

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

} // namespace WCDB

#endif /* _WCDB_SYNTAXCONFLICT_HPP */
