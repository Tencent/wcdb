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

#ifndef SQL_hpp
#define SQL_hpp

#include <WCDB/ColumnType.hpp>
#include <WCDB/Convertible.hpp>
#include <WCDB/Syntax.h>
#include <WCDB/SyntaxForwardDeclaration.h>
#include <WCDB/SyntaxList.hpp>
#include <memory>

namespace WCDB {

class SQL {
public:
    virtual ~SQL();

    typedef Syntax::Identifier::Type Type;
    virtual Type getType() const = 0;

    typedef Syntax::Identifier::Iterator Iterator;
    virtual void iterate(const Iterator& iterator, void* parameter) = 0;

    virtual String getDescription() const = 0;
};

template<typename T>
class TypedSyntax {
    static_assert(std::is_base_of<Syntax::Identifier, T>::value, "");

public:
    typedef T SyntaxType;
    static constexpr const SQL::Type type = SyntaxType::type;

    virtual ~TypedSyntax() {}

    // Convert SQL to Syntax implicitly
    operator const T&() const { return syntax; }

    T syntax;
};

template<typename T>
class SQLSyntax : public SQL, public TypedSyntax<T> {
public:
    virtual ~SQLSyntax() {}

    Type getType() const override final
    {
        return TypedSyntax<T>::syntax.getType();
    }

    void iterate(const Iterator& iterator, void* parameter) override final
    {
        return TypedSyntax<T>::syntax.iterate(iterator, parameter);
    }

    String getDescription() const override final
    {
        return TypedSyntax<T>::syntax.getDescription();
    }
};

} // namespace WCDB

#endif /* SQL_hpp */
