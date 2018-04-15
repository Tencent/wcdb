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

#include <WCDB/HandleError.hpp>
#include <sqlcipher/sqlite3.h>

namespace WCDB {

HandleError::HandleError()
    : Error(), extendedCode(0), tag(invalidTag), operation(Operation::NotSet)
{
}

size_t HandleError::getHashedTypeid() const
{
    return typeid(HandleError).hash_code();
}

void HandleError::reset()
{
    code = SQLITE_OK;
    extendedCode = SQLITE_OK;
    statement.getCOWLang().clear();
    operation = Operation::NotSet;
}

std::string HandleError::getDescription() const
{
    std::string description = Error::getDescription();
    addToDescription(description, "Tag", tag);
    addToDescription(description, "Path", path);
    if (extendedCode != 0) {
        addToDescription(description, "ExtCode", extendedCode);
    }
    if (!statement.getCOWLang().empty()) {
        addToDescription(description, "SQL",
                         statement.getDescription().c_str());
    }
    if (operation != Operation::NotSet) {
        addToDescription(description, "Op", operation);
    }
    return description;
}

} //namespace WCDB
