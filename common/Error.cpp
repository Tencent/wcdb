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

#include <WCDB/Error.hpp>
#include <WCDB/Macro.hpp>

namespace WCDB {

Error::Error() : level(Level::Error), code(0)
{
}

int Error::getType() const
{
    return Error::type;
}

bool Error::isOK() const
{
    return code == 0;
}

std::string Error::getDescription() const
{
    std::string description;
    addToDescription(description, "Code", code);
    if (!message.empty()) {
        addToDescription(description, "Msg", message);
    }
    return "[" + std::string(Error::LevelName(level)) + "]" + description;
}

void Error::addToDescription(std::string &description,
                             const char *key,
                             int64_t value) const
{
    addToDescription(description, key, std::to_string(value));
}

void Error::addToDescription(std::string &description,
                             const char *key,
                             const std::string &value) const
{
    if (!description.empty()) {
        description.append(", ");
    }
    description.append(key);
    description.append(": ");
    description.append(value);
}

} //namespace WCDB
