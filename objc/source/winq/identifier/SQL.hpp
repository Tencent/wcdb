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

#ifndef __WCDB_SQL_HPP
#define __WCDB_SQL_HPP

#include <WCDB/ColumnType.hpp>
#include <WCDB/Convertible.hpp>
#include <WCDB/Syntax.h>
#include <WCDB/SyntaxForwardDeclaration.h>
#include <WCDB/SyntaxList.hpp>
#include <WCDB/UnsafeString.hpp>
#include <memory>

namespace WCDB {

class SQL {
public:
    SQL();
    virtual ~SQL();

    typedef Syntax::Identifier::Type Type;
    Type getType() const;

    typedef Syntax::Identifier::Iterator Iterator;
    void iterate(const Iterator& iterator);

    String getDescription() const;

    Syntax::Identifier* getSyntaxIdentifier() const;

protected:
    template<typename T, typename Enable = typename std::enable_if<std::is_base_of<Syntax::Identifier, T>::value>::type>
    SQL(T* dummy) : m_syntax(new T)
    {
        WCDB_UNUSED(dummy)
    }

    template<typename T, typename Enable = typename std::enable_if<std::is_base_of<Syntax::Identifier, T>::value>::type>
    SQL(T* dummy, const SQL& sql)
    : SQL(T::type, sql){ WCDB_UNUSED(dummy) }

    Shadow<Syntax::Identifier> m_syntax;

private:
    SQL(Type type, const SQL& sql);
};

template<typename T, typename U>
class TypedSyntax : public U {
    static_assert(std::is_base_of<Syntax::Identifier, T>::value, "");
    static_assert(std::is_base_of<SQL, U>::value, "");

public:
    typedef T SyntaxType;
    static constexpr const SQL::Type type = SyntaxType::type;

    TypedSyntax() : U((T*) nullptr) {}

    explicit TypedSyntax(const U& other) : U((T*) nullptr, other) {}

    virtual ~TypedSyntax() {}

    inline T& syntax() const { return *static_cast<T*>(U::m_syntax.get()); }

    // Convert SQL to Syntax implicitly
    operator const T&() const { return *static_cast<T*>(U::m_syntax.get()); }
};

} // namespace WCDB

#endif /* __WCDB_SQL_HPP */
