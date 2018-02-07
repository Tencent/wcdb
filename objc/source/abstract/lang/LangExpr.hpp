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

#include <WCDB/lang_common.h>

class LangExprBase : public Lang {
};
class LangExprColumn;
class LangExprUnaryOperation;
class LangExprBinaryOperation;
class LangExprFunction;
class LangExprList;
class LangExprCast;
class LangExprCollate;
class LangExprPattern;
class LangExprNull;
class LangExprBetween;
class LangExprIn;
class LangExprExists;
class LangExprCase;
class LangExpr;

class LangExprColumn : public LangExprBase {
public:
    copy_on_write_string schemaName;
    copy_on_write_string tableName;
    copy_on_write_string columnName;

    virtual copy_on_write_string SQL() const override;
};

class LangExprUnaryOperation : public LangExprBase {
public:
    enum class Operator : int {
        Negative,
        Positive,
        Tilde,
        Not,
    };
    Operator unaryOperator;
    copy_on_write_lazy_lang<LangExprBase> expr;

    virtual copy_on_write_string SQL() const override;

private:
    static constexpr const char *
    OperatorName(const Operator &unaryOpeartor);
};

class LangExprBinaryOperation : public LangExprBase {
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
    copy_on_write_lazy_lang<LangExprBase> left;
    copy_on_write_lazy_lang<LangExprBase> right;
    Operator binaryOperator;

    virtual copy_on_write_string SQL() const override;

private:
    static constexpr const char *
    OperatorName(const Operator &binaryOpeartor);
};

class LangExprFunction : public LangExprBase {
public:
    copy_on_write_string functionName;
    enum class Type : int {
        NotSet,
        Expr,
        DistinctExpr,
        Star,
    };
    Type type;
    copy_on_write_lazy_lang_list<LangExprBase> exprs;

    virtual copy_on_write_string SQL() const override;
};

class LangExprList : public LangExprBase {
public:
    copy_on_write_lazy_lang_list<LangExprBase> exprs;

    virtual copy_on_write_string SQL() const override;
};

class LangExprCast : public LangExprBase {
public:
    copy_on_write_lazy_lang<LangExprBase> expr;
    LangColumnType type;

    virtual copy_on_write_string SQL() const override;
};

class LangExprCollate : public LangExprBase {
public:
    copy_on_write_lazy_lang<LangExprBase> expr;
    copy_on_write_string collationName;

    virtual copy_on_write_string SQL() const override;
};

class LangExprPattern : public LangExprBase {
public:
    bool isNot;
    copy_on_write_lazy_lang<LangExprBase> left;
    enum class Type : int {
        Like,
        Glob,
        Regexp,
        Match,
    };
    Type type;
    copy_on_write_lazy_lang<LangExprBase> right;
    copy_on_write_lazy_lang<LangExprBase> escape;

    virtual copy_on_write_string SQL() const override;

protected:
    static constexpr const char *TypeName(const Type &type);
};

class LangExprNull : public LangExprBase {
public:
    copy_on_write_lazy_lang<LangExprBase> expr;
    bool isNull;

    virtual copy_on_write_string SQL() const override;
};

class LangExprBetween : public LangExprBase {
public:
    bool isNot;
    copy_on_write_lazy_lang<LangExprBase> expr;
    copy_on_write_lazy_lang<LangExprBase> left;
    copy_on_write_lazy_lang<LangExprBase> right;

    virtual copy_on_write_string SQL() const override;
};

class LangExprIn : public LangExprBase {
public:
    bool isNot;
    copy_on_write_lazy_lang<LangExprBase> expr;

    enum class Switch : int {
        NotSet,
        Select,
        Expr,
        TableOrFunction,
    };
    Switch switcher;

    copy_on_write_lazy_lang<LangSelectSTMT> selectSTMT;
    copy_on_write_lazy_lang_list<LangExprBase> exprs;
    copy_on_write_string schemaName;
    copy_on_write_string tableNameOrFunction;

    virtual copy_on_write_string SQL() const override;
};

class LangExprExists : public LangExprBase {
public:
    bool exists;
    bool isNot;
    copy_on_write_lazy_lang<LangSelectSTMT> selectSTMT;

    virtual copy_on_write_string SQL() const override;
};

class LangExprCase : public LangExprBase {
public:
    copy_on_write_lazy_lang<LangExprBase> exprCase;

    class Pair : public Lang {
    public:
        copy_on_write_lazy_lang<LangExprBase> when;
        copy_on_write_lazy_lang<LangExprBase> then;

        virtual copy_on_write_string SQL() const override;
    };

    copy_on_write_lazy_lang_list<Pair> pairs;

    copy_on_write_lazy_lang<LangExprBase> exprElse;

    virtual copy_on_write_string SQL() const override;
};

template <>
copy_on_write_string
copy_on_write_lazy_lang_list<LangExprCase::Pair>::calculatedDescription() const;

class LangExpr : public LangExprBase {
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

    copy_on_write_lazy_lang<LangLiteralValue> literalValue;
    copy_on_write_lazy_lang<LangBindParameter> bindParamter;
    copy_on_write_lazy_lang<LangExprColumn> exprColumn;
    copy_on_write_lazy_lang<LangExprUnaryOperation> exprUnaryOperator;
    copy_on_write_lazy_lang<LangExprBinaryOperation> exprBinaryOperator;
    copy_on_write_lazy_lang<LangExprFunction> exprFunction;
    copy_on_write_lazy_lang<LangExprList> exprList;
    copy_on_write_lazy_lang<LangExprCast> exprCast;
    copy_on_write_lazy_lang<LangExprCollate> exprCollate;
    copy_on_write_lazy_lang<LangExprPattern> exprPattern;
    copy_on_write_lazy_lang<LangExprNull> exprNull;
    copy_on_write_lazy_lang<LangExprBetween> exprBetween;
    copy_on_write_lazy_lang<LangExprIn> exprIn;
    copy_on_write_lazy_lang<LangExprExists> exprExists;
    copy_on_write_lazy_lang<LangExprCase> exprCase;
    copy_on_write_lazy_lang<LangRaiseFunction> raiseFunction;

    virtual copy_on_write_string SQL() const override;
};

#endif /* LangExpr_hpp */
