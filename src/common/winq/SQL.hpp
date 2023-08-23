//
// Created by sanhuazhang on 2019/05/02
//

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

#include "ColumnType.hpp"
#include "Convertible.hpp"
#include "StringView.hpp"
#include "Syntax.h"
#include "SyntaxForwardDeclaration.h"
#include "SyntaxList.hpp"
#include <memory>

namespace WCDB {

class WCDB_API SQL {
public:
    SQL();
    virtual ~SQL();

    typedef Syntax::Identifier::Type Type;
    virtual Type getType() const;

    typedef Syntax::Identifier::Iterator Iterator;
    void iterate(const Iterator& iterator);
    typedef Syntax::Identifier::ConstIterator ConstIterator;
    void iterate(const ConstIterator& iterator) const;

    StringView getDescription() const;

    virtual Syntax::Identifier& syntax();
    virtual const Syntax::Identifier& syntax() const;

protected:
    SQL(const SQL& sql);
    SQL(SQL&& sql);
    SQL& operator=(const SQL& other);
    SQL& operator=(SQL&& other);

    mutable Syntax::Identifier* m_syntaxPtr = nullptr;
    mutable std::shared_ptr<StringView> m_description;
    mutable bool m_hasDescription = false;
};

template<typename __SyntaxType, typename __SQLType>
class SpecifiedSyntax : public __SQLType {
    static_assert(sizeof(__SyntaxType) < 1000,
                  "Syntax is too large to be allocated on the stack.");

public:
    using SyntaxType = __SyntaxType;
    static constexpr const SQL::Type type = SyntaxType::type;

private:
    using SQLType = __SQLType;
    using Self = SpecifiedSyntax<SyntaxType, SQLType>;
    using Super = SQLType;
    static_assert(std::is_base_of<Syntax::Identifier, SyntaxType>::value, "");
    static_assert(std::is_base_of<SQL, SQLType>::value, "");
    __SyntaxType m_syntax;

public:
    SpecifiedSyntax() : Super(), m_syntax() { this->m_syntaxPtr = &m_syntax; }

    explicit SpecifiedSyntax(const Self& other)
    : Super(other), m_syntax(other.m_syntax)
    {
        this->m_syntaxPtr = &m_syntax;
    }

    explicit SpecifiedSyntax(const SyntaxType& syntax)
    : Super(), m_syntax(syntax)
    {
        this->m_syntaxPtr = &m_syntax;
    }

    SpecifiedSyntax(SyntaxType&& syntax) : Super(), m_syntax(std::move(syntax))
    {
        this->m_syntaxPtr = &m_syntax;
    }

    SpecifiedSyntax(Self&& other)
    : Super(std::move(other)), m_syntax(std::move(other.m_syntax))
    {
        this->m_syntaxPtr = &m_syntax;
    }

    ~SpecifiedSyntax() override = default;

    Self& operator=(const Self& other)
    {
        Super::operator=(other);
        m_syntax = other.m_syntax;
        this->m_syntaxPtr = &m_syntax;
        return *this;
    }

    Self& operator=(Self&& other)
    {
        Super::operator=(std::move(other));
        m_syntax = std::move(other.m_syntax);
        this->m_syntaxPtr = &m_syntax;
        return *this;
    }

    SyntaxType& syntax() override
    {
        return static_cast<SyntaxType&>(Super::syntax());
    }

    const SyntaxType& syntax() const override
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
