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

#include "SQL.hpp"

namespace WCDB {

class WCDB_API Statement : public SQL {
public:
    virtual ~Statement() override;
    Statement(const Shadow<Syntax::Identifier>& syntax);
    Statement(Shadow<Syntax::Identifier>&& syntax);
    Statement(std::shared_ptr<Syntax::Identifier>&& underlying);

    virtual Syntax::Identifier& syntax() override;
    virtual const Syntax::Identifier& syntax() const override;

    Statement(const Statement& sql);
    Statement(Statement&& sql);
    Statement& operator=(const Statement& other);
    Statement& operator=(Statement&& other);
    bool isWriteStatement() const;

private:
    Shadow<Syntax::Identifier> m_syntax;
};

template<typename __SyntaxType>
class SpecifiedSyntax<__SyntaxType, Statement> : public Statement {
public:
    using SyntaxType = __SyntaxType;
    static constexpr const SQL::Type type = SyntaxType::type;

private:
    using Self = SpecifiedSyntax<SyntaxType, Statement>;
    using Super = Statement;
    static_assert(std::is_base_of<Syntax::Identifier, SyntaxType>::value, "");
    static_assert(std::is_base_of<SQL, Statement>::value, "");

public:
    SpecifiedSyntax() : Super(std::make_shared<SyntaxType>()) {}

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

    Self& operator=(Self&& other)
    {
        Super::operator=(std::move(other));
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
