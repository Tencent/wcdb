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

#ifndef LangExpr_hpp
#define LangExpr_hpp

#include <WINQ/lang_common.h>

namespace WCDB {

namespace lang {

class ExprBase : public Lang {
};
class ExprColumn;
class ExprUnaryOperation;
class ExprBinaryOperation;
class ExprFunction;
class ExprList;
class ExprCast;
class ExprCollate;
class ExprPattern;
class ExprNull;
class ExprBetween;
class ExprIn;
class ExprExists;
class ExprCase;
class Expr;

class ExprColumn : public ExprBase {
public:
    copy_on_write_string schemaName;
    copy_on_write_string tableName;
    copy_on_write_string columnName;

    virtual copy_on_write_string SQL() const override;
};

class ExprUnaryOperation : public ExprBase {
public:
    enum class Operator : int {
        Negative,
        Positive,
        Tilde,
        Not,
    };
    Operator unaryOperator;
    copy_on_write_lazy_lang<ExprBase> expr;

    virtual copy_on_write_string SQL() const override;

private:
    static constexpr const char *OperatorName(const Operator &unaryOpeartor);
};

class ExprBinaryOperation : public ExprBase {
public:
    enum class Operator : int {
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
        IsNot,
        And,
        Or,
    };
    copy_on_write_lazy_lang<ExprBase> left;
    copy_on_write_lazy_lang<ExprBase> right;
    Operator binaryOperator;

    virtual copy_on_write_string SQL() const override;

private:
    static constexpr const char *OperatorName(const Operator &binaryOpeartor);
};

class ExprFunction : public ExprBase {
public:
    copy_on_write_string functionName;
    enum class Type : int {
        NotSet,
        Expr,
        DistinctExpr,
        Star,
    };
    Type type;
    copy_on_write_lazy_lang_list<ExprBase> exprs;

    virtual copy_on_write_string SQL() const override;
};

class ExprList : public ExprBase {
public:
    copy_on_write_lazy_lang_list<ExprBase> exprs;

    virtual copy_on_write_string SQL() const override;
};

class ExprCast : public ExprBase {
public:
    copy_on_write_lazy_lang<ExprBase> expr;
    ColumnType type;

    virtual copy_on_write_string SQL() const override;
};

class ExprCollate : public ExprBase {
public:
    copy_on_write_lazy_lang<ExprBase> expr;
    copy_on_write_string collationName;

    virtual copy_on_write_string SQL() const override;
};

class ExprPattern : public ExprBase {
public:
    bool isNot;
    copy_on_write_lazy_lang<ExprBase> left;
    enum class Type : int {
        Like,
        Glob,
        Regexp,
        Match,
    };
    Type type;
    copy_on_write_lazy_lang<ExprBase> right;
    copy_on_write_lazy_lang<ExprBase> escape;

    virtual copy_on_write_string SQL() const override;

protected:
    static constexpr const char *TypeName(const Type &type);
};

class ExprNull : public ExprBase {
public:
    copy_on_write_lazy_lang<ExprBase> expr;
    bool isNull;

    virtual copy_on_write_string SQL() const override;
};

class ExprBetween : public ExprBase {
public:
    bool isNot;
    copy_on_write_lazy_lang<ExprBase> expr;
    copy_on_write_lazy_lang<ExprBase> left;
    copy_on_write_lazy_lang<ExprBase> right;

    virtual copy_on_write_string SQL() const override;
};

class ExprIn : public ExprBase {
public:
    bool isNot;
    copy_on_write_lazy_lang<ExprBase> expr;

    enum class Switch : int {
        NotSet,
        Select,
        Expr,
        Table,
        Function,
    };
    Switch switcher;

    copy_on_write_lazy_lang<SelectSTMT> selectSTMT;
    copy_on_write_lazy_lang_list<ExprBase> exprs;
    copy_on_write_string schemaName;
    copy_on_write_string tableNameOrFunction;

    virtual copy_on_write_string SQL() const override;
};

class ExprExists : public ExprBase {
public:
    bool exists;
    bool isNot;
    copy_on_write_lazy_lang<SelectSTMT> selectSTMT;

    virtual copy_on_write_string SQL() const override;
};

class ExprCase : public ExprBase {
public:
    copy_on_write_lazy_lang<ExprBase> exprCase;

    class Pair : public Lang {
    public:
        copy_on_write_lazy_lang<ExprBase> when;
        copy_on_write_lazy_lang<ExprBase> then;

        virtual copy_on_write_string SQL() const override;
    };

    copy_on_write_lazy_lang_list<Pair> pairs;

    copy_on_write_lazy_lang<ExprBase> exprElse;

    virtual copy_on_write_string SQL() const override;
};

template <>
copy_on_write_string
copy_on_write_lazy_lang_list<ExprCase::Pair>::calculatedDescription() const;

class Expr : public ExprBase {
public:
    enum class Type : int {
        LiteralValue,
        BindParameter,
        Column,
        UnaryOperator,
        BinaryOperator,
        Function,
        List,
        Cast,
        Collate,
        Pattern,
        Null,
        Between,
        In,
        Exists,
        Case,
        RaiseFunction,
    };
    Type type;

    copy_on_write_lazy_lang<LiteralValue> literalValue;
    copy_on_write_lazy_lang<BindParameter> bindParamter;
    copy_on_write_lazy_lang<ExprColumn> exprColumn;
    copy_on_write_lazy_lang<ExprUnaryOperation> exprUnaryOperator;
    copy_on_write_lazy_lang<ExprBinaryOperation> exprBinaryOperator;
    copy_on_write_lazy_lang<ExprFunction> exprFunction;
    copy_on_write_lazy_lang<ExprList> exprList;
    copy_on_write_lazy_lang<ExprCast> exprCast;
    copy_on_write_lazy_lang<ExprCollate> exprCollate;
    copy_on_write_lazy_lang<ExprPattern> exprPattern;
    copy_on_write_lazy_lang<ExprNull> exprNull;
    copy_on_write_lazy_lang<ExprBetween> exprBetween;
    copy_on_write_lazy_lang<ExprIn> exprIn;
    copy_on_write_lazy_lang<ExprExists> exprExists;
    copy_on_write_lazy_lang<ExprCase> exprCase;
    copy_on_write_lazy_lang<RaiseFunction> raiseFunction;

    virtual copy_on_write_string SQL() const override;
};

} // namespace lang

} // namespace WCDB

#endif /* LangExpr_hpp */
