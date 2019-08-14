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
#include <WCDB/SQL.hpp>
#include <atomic>

namespace WCDB {

SQL::SQL(const SQL& other)
: m_syntax(other.m_syntax), m_description(std::atomic_load(&other.m_description))
{
}

SQL::SQL(SQL&& other)
: m_syntax(std::move(other.m_syntax))
, m_description(std::atomic_load(&other.m_description))
{
    std::atomic_store(&other.m_description, std::shared_ptr<StringView>(nullptr));
}

SQL::SQL(const Shadow<Syntax::Identifier>& syntax)
: m_syntax(syntax), m_description(nullptr)
{
}

SQL::SQL(Shadow<Syntax::Identifier>&& syntax)
: m_syntax(std::move(syntax)), m_description(nullptr)
{
}

SQL::SQL(std::unique_ptr<Syntax::Identifier>&& underlying)
: m_syntax(std::move(underlying)), m_description(nullptr)
{
}

SQL& SQL::operator=(const SQL& other)
{
    m_syntax = other.m_syntax;
    m_description = std::atomic_load(&other.m_description);
    return *this;
}

SQL& SQL::operator=(SQL&& other)
{
    m_syntax = std::move(other.m_syntax);
    m_description = std::atomic_load(&other.m_description);
    std::atomic_store(&other.m_description, std::shared_ptr<StringView>(nullptr));
    return *this;
}

SQL::Type SQL::getType() const
{
    return m_syntax->getType();
}

void SQL::iterate(const Iterator& iterator)
{
    return m_syntax->iterate(iterator);
}

StringView SQL::getDescription() const
{
    // class SQL is not designed for thread-safe.
    // But here, the cache of `m_description` may be accessed/modified in different threads.
    // So we must make this const function thread-safe.
    std::shared_ptr<StringView> description = std::atomic_load(&m_description);
    while (description == nullptr) {
        if (m_syntax != nullptr) {
            std::atomic_store(
            &m_description, std::make_shared<StringView>(m_syntax->getDescription()));
            description = std::atomic_load(&m_description);
        } else {
            return StringView();
        }
    }
    return *description.get();
}

Syntax::Identifier& SQL::syntax()
{
    // Note that `syntax()` is not designed for thread-safe.
    std::atomic_store(&m_description, std::shared_ptr<StringView>(nullptr));
    return *m_syntax.get();
}

const Syntax::Identifier& SQL::syntax() const
{
    return *m_syntax.get();
}

} // namespace WCDB
