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

#include <WCDB/Lang.h>

namespace WCDB {

namespace Lang {

CreateViewSTMT::CreateViewSTMT() : temp(false), ifNotExists(false)
{
}

CopyOnWriteString CreateViewSTMT::SQL() const
{
    std::string description("CREATE ");
    if (temp) {
        description.append("TEMP ");
    }
    description.append("VIEW ");
    if (ifNotExists) {
        description.append("IF NOT EXISTS ");
    }
    if (schemaName.isNull()) {
        description.append(mainSchema() + ".");
    } else if (!schemaName.get().empty()) {
        description.append(schemaName.get() + ".");
    }
    LangRemedialAssert(!viewName.empty());
    description.append(viewName.get());
    if (!columns.empty()) {
        description.append("(" + columns.description().get() + ")");
    }
    LangRemedialAssert(!selectSTMT.empty());
    description.append(" AS " + selectSTMT.description().get());
    return description;
}

STMT::Type CreateViewSTMT::getSTMTType() const
{
    return STMT::Type::CreateView;
}

STMT::Type CreateViewSTMT::getType()
{
    return STMT::Type::CreateView;
}

} // namespace Lang

} // namespace WCDB
