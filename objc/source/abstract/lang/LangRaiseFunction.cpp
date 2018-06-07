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
#include <WCDB/Lang.h>

namespace WCDB {

namespace Lang {

RaiseFunction::RaiseFunction() : type(Type::Ignore)
{
}

CopyOnWriteString RaiseFunction::SQL() const
{
    std::string description("RAISE(");
    switch (type) {
        case Type::Ignore:
            description.append("IGNORE");
            break;
        case Type::Rollback:
            LangRemedialAssert(!errorMessage.empty());
            description.append("ROLLBACK, " + errorMessage.get());
            break;
        case Type::Abort:
            LangRemedialAssert(!errorMessage.empty());
            description.append("ABORT, " + errorMessage.get());
            break;
        case Type::Fail:
            LangRemedialAssert(!errorMessage.empty());
            description.append("FAIL, " + errorMessage.get());
            break;
    }
    description.append(")");
    return description;
}

} // namespace Lang

} // namespace WCDB
