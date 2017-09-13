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

#include <WCDB/describable.hpp>

namespace WCDB {

Describable::Describable(const std::string &description)
    : m_description(description)
{
}

Describable::Describable(const char *description) : m_description(description)
{
}

const std::string &Describable::getDescription() const
{
    return m_description;
}

bool Describable::isEmpty() const
{
    return m_description.empty();
}

Describable::operator const std::string &() const
{
    return m_description;
}

} //namespace WCDB
