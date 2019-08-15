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

#pragma once

#include <WCDB/ColumnType.hpp>
#include <WCDB/Convertible.hpp>
#include <WCDB/StringView.hpp>
#include <WCDB/Syntax.h>
#include <WCDB/SyntaxForwardDeclaration.h>
#include <WCDB/SyntaxList.hpp>
#include <memory>
#include <optional>

namespace WCDB {

class SQL {
public:
    SQL();
    virtual ~SQL();

    typedef Syntax::Identifier::Type Type;
    Type getType() const;

    typedef Syntax::Identifier::Iterator Iterator;
    void iterate(const Iterator& iterator);

    StringView getDescription() const;

    Syntax::Identifier& syntax();
    const Syntax::Identifier& syntax() const;

protected:
    SQL(const SQL& sql);
    SQL(SQL&& sql);
    SQL(const Shadow<Syntax::Identifier>& syntax);
    SQL(Shadow<Syntax::Identifier>&& syntax);
    SQL(std::unique_ptr<Syntax::Identifier>&& underlying);

    SQL& operator=(const SQL& other);
    SQL& operator=(SQL&& other);

private:
    mutable std::shared_ptr<StringView> m_description;
    Shadow<Syntax::Identifier> m_syntax;
};

template<typename __SyntaxType, typename __SQLType>
class SpecifiedSyntax : public __SQLType {
public:
    using SyntaxType = __SyntaxType;
    static constexpr const SQL::Type type = SyntaxType::type;

private:
    using SQLType = __SQLType;
    using Self = SpecifiedSyntax<SyntaxType, SQLType>;
    using Super = SQLType;
    static_assert(std::is_base_of<Syntax::Identifier, SyntaxType>::value, "");
    static_assert(std::is_base_of<SQL, SQLType>::value, "");

public:
    SpecifiedSyntax() : Super(std::make_unique<SyntaxType>()) {}

    explicit SpecifiedSyntax(const Self& other) : Super(other) {}

    explicit SpecifiedSyntax(const SyntaxType& syntax) : Super(syntax) {}

    SpecifiedSyntax(SyntaxType&& syntax) : Super(std::move(syntax)) {}

    SpecifiedSyntax(Self&& other) : Super(std::move(other)) {}

    ~SpecifiedSyntax() override = default;

    Self& operator=(const Self& other)
    {
        Super::operator=(other);
        return *this;
    }

    SQL& operator=(SQL&& other)
    {
        Super::operator=(std::move(other));
        return *this;
    }

    SyntaxType& syntax() { return static_cast<SyntaxType&>(Super::syntax()); }

    const SyntaxType& syntax() const
    {
        return static_cast<const SyntaxType&>(Super::syntax());
    }

    // Convert SQL to Syntax implicitly
    operator const SyntaxType&() const
    {
        return static_cast<const SyntaxType&>(Super::syntax());
    }
};

} // namespace WCDB
