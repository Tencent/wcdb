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

#ifndef __WCDB_SYNTAX_EXPRESSION_HPP
#define __WCDB_SYNTAX_EXPRESSION_HPP

#include <WCDB/SyntaxBindParameter.hpp>
#include <WCDB/SyntaxFilter.hpp>
#include <WCDB/SyntaxIdentifier.hpp>
#include <WCDB/SyntaxLiteralValue.hpp>
#include <WCDB/SyntaxRaiseFunction.hpp>
#include <WCDB/SyntaxSchema.hpp>
#include <WCDB/SyntaxWindowDef.hpp>

namespace WCDB {

namespace Syntax {

class SelectSTMT;

class Expression final : public Identifier {
#pragma mark - Lang
public:
    WCDB_DEFAULT_SYNTAX_ENUM(
        LiteralValue,
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
        Select,
    );

    LiteralValue literalValue;
    BindParameter bindParameter;

    Schema schema;
    String table;
    Column column;

    enum class UnaryOperator {
        Negative,
        Positive,
        Tilde,
        Not,
        Null,
    } unaryOperator;

    enum class BinaryOperator {
        Concatenate,
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

    std::list<Expression> expressions;

    String function;
    bool distinct = false;
    bool useWildcard = false;

    ColumnType castType;

    String collation;

    bool isNot = false;
    bool escape = false;

    enum class SwitchIn {
        Empty,
        Select,
        Expressions,
        Table,
        Function,
    } inSwitcher;
    Shadow<SelectSTMT> select;

    bool hasCase = false;
    bool hasElse = false;

    RaiseFunction raiseFunction;

    WindowDef windowDef;
    String windowName;
    bool useFilter = false;
    Filter filter;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::Expression;
    Type getType() const override final;
    String getValidDescription() const override final;
    void iterate(const Iterator& iterator, bool& stop) override final;
};

} // namespace Syntax

} // namespace WCDB

#endif /* __WCDB_SYNTAX_EXPRESSION_HPP */
