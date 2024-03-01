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

#include "SyntaxBindParameter.hpp"
#include "SyntaxFilter.hpp"
#include "SyntaxIdentifier.hpp"
#include "SyntaxLiteralValue.hpp"
#include "SyntaxRaiseFunction.hpp"
#include "SyntaxSchema.hpp"
#include "SyntaxWindowDef.hpp"

namespace WCDB {

namespace Syntax {

class SelectSTMT;

class WCDB_API ExpressionUnionMember {
public:
    ExpressionUnionMember();
    ~ExpressionUnionMember();
    ExpressionUnionMember(const ExpressionUnionMember& other);
    ExpressionUnionMember(ExpressionUnionMember&& other);
    ExpressionUnionMember& operator=(const ExpressionUnionMember& other);
    ExpressionUnionMember& operator=(ExpressionUnionMember&& other);

    LiteralValue& literalValue();
    BindParameter& bindParameter();
    Column& column();
    Shadow<SelectSTMT>& select();
    StringView& table();
    RaiseFunction& raiseFunction();
    WindowDef& windowDef();
    StringView& collation();
    Schema& schema();
    Filter& filter();
    StringView& windowName();
    StringView& function();

    const LiteralValue& literalValue() const;
    const BindParameter& bindParameter() const;
    const Column& column() const;
    const Shadow<SelectSTMT>& select() const;
    const StringView& table() const;
    const RaiseFunction& raiseFunction() const;
    const WindowDef& windowDef() const;
    const StringView& collation() const;
    const Schema& schema() const;
    const Filter& filter() const;
    const StringView& windowName() const;
    const StringView& function() const;

protected:
    enum class Member : signed char {
        Invalid = 0,
        literalValue,
        bindParameter,
        column,
        raiseFunction,
        windowDef,
        windowName,
        select,
        collation,
        schema,
        filter,
        table,
        function,
    };

    mutable Member m_firstMember = Member::Invalid;
    mutable Member m_secondMember = Member::Invalid;
    mutable Member m_thirdMember = Member::Invalid;

    union {
        mutable LiteralValue m_literalValue;
        mutable BindParameter m_bindParameter;
        mutable Column m_column;
        mutable RaiseFunction m_raiseFunction;
        mutable WindowDef m_windowDef;
        mutable StringView m_windowName;
    };
    void firstMemberReset() const;

    union {
        mutable Shadow<SelectSTMT> m_select;
        mutable StringView m_collation;
        mutable Schema m_schema;
        mutable Filter m_filter;
    };
    void secondMemberReset() const;

    union {
        mutable StringView m_table;
        mutable StringView m_function;
    };
    void thirdMemberReset() const;

    template<typename T>
    void assignFromOther(T&& member);
};

class WCDB_API Expression final : public Identifier, public ExpressionUnionMember {
public:
    ~Expression() override;

    std::list<Expression> expressions;

    WCDB_SYNTAX_MAIN_UNION_ENUM(LiteralValue,
                                BindParameter,
                                Column,
                                UnaryOperation,
                                BinaryOperation,
                                Function,
                                Expressions,
                                Cast,
                                Collate,
                                Between,
                                In,
                                Exists,
                                Case,
                                RaiseFunction,
                                Window,
                                Select, );

    enum class UnaryOperator : signed char {
        Negative = 1,
        Positive,
        Tilde,
        Not,
        Null,
    } unaryOperator;

    enum class BinaryOperator : signed char {
        Concatenate = 1,
        Multiply,
        Divide,
        Modulo,
        Plus,
        Minus,
        LeftShift,
        RightShift,
        BitwiseAnd,
        BitwiseOr,
        Less,
        LessOrEqual,
        Greater,
        GreaterOrEqual,
        Equal,
        NotEqual,
        Is,
        And,
        Or,
        Like,
        GLOB,
        RegExp,
        Match,
    } binaryOperator;

    bool distinct = false;
    bool useWildcard = false;

    ColumnType castType;

    bool isNot = false;
    bool escape = false;

    bool hasCase = false;
    bool hasElse = false;

    enum class SwitchIn : signed char {
        Empty = 1,
        Select,
        Expressions,
        Table,
        Function,
    } inSwitcher;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::Expression;
    Type getType() const override final;
    bool describle(std::ostream& stream) const override final;
    void iterate(const Iterator& iterator, bool& stop) override final;
};

static_assert(sizeof(Expression) < 500, "Expression is too large.");

} // namespace Syntax

} // namespace WCDB
