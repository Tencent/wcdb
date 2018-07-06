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
#include <WCDB/String.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

Describable::Describable()
{
}

Describable::Describable(const Lang::CopyOnWriteLazyLang<Lang::Lang> &cowLang)
: m_cowLang(cowLang)
{
}

const std::string &Describable::getDescription() const
{
    if (!m_cowLang.empty()) {
        return m_cowLang.description().get();
    }
    return String::empty();
}

Lang::CopyOnWriteLazyLang<Lang::Lang> &Describable::getCOWLang()
{
    return m_cowLang;
}

const Lang::CopyOnWriteLazyLang<Lang::Lang> &Describable::getCOWLang() const
{
    return m_cowLang;
}

bool Describable::empty() const
{
    return m_cowLang.empty();
}

} // namespace WCDB
