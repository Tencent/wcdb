// Created by qiuwenchen on 2023/3/30.
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
package com.tencent.wcdb.winq;

public class Expression extends ExpressionOperable implements IndexedColumnConvertible, ResultColumnConvertible{
    @Override
    protected CPPType getCppType() {
        return CPPType.Expression;
    }

    Expression(){
    }

    public Expression(LiteralValue value) {
        cppObj = createCppObj(value.getCppType().ordinal(), value.getCppObj());
    }

    public Expression(BindParameter bindParameter) {
        cppObj = createCppObj(bindParameter.getCppType().ordinal(), bindParameter.getCppObj());
    }

    public Expression(Column column) {
        cppObj = createCppObj(column.getCppType().ordinal(), column.getCppObj());
    }

    public Expression(StatementSelect select) {
        cppObj = createCppObj(select.getCppType().ordinal(), select.getCppObj());
    }

    private native long createCppObj(int type, long intValue);

    public static Expression function(String funcName) {
        return createExpression(createWithFunction(funcName));
    }

    private static native long createWithFunction(String func);

    public Expression schema(String schema) {
        return schema(new Schema(schema));
    }

    public Expression schema(Schema schema) {
        schema(cppObj, schema.getCppObj());
        return this;
    }

    private native void schema(long self, long schema);

    public Expression distinct() {
        distinct(cppObj);
        return this;
    }

    private native void distinct(long self);

    public Expression invoke() {
        invoke(cppObj);
        return this;
    }

    private native void invoke(long self);

    public Expression invokeAll() {
        invokeAll(cppObj);
        return this;
    }

    private native void invokeAll(long self);

    public Expression argument(boolean arg) {
        argument(cppObj, CPPType.Bool.ordinal(), arg ? 1 : 0, 0, null);
        return this;
    }

    public Expression argument(long arg) {
        argument(cppObj, CPPType.Int.ordinal(), arg, 0, null);
        return this;
    }

    public Expression argument(double arg) {
        argument(cppObj, CPPType.Double.ordinal(), 0, arg, null);
        return this;
    }

    public Expression argument(String arg) {
        if(arg != null) {
            argument(cppObj, CPPType.String.ordinal(), 0, 0, arg);
        }else{
            argument(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }
        return this;
    }

    public Expression argument(ExpressionConvertible arg) {
        if(arg != null){
            argument(cppObj, arg.asIdentifier().getCppType().ordinal(), arg.asIdentifier().getCppObj(), 0, null);
        } else {
            argument(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }

        return this;
    }

    private native void argument(long self, int type, long intValue, double doubleValue, String stringValue);

    public Expression escape(String content) {
        escape(cppObj, content);
        return this;
    }

    private native void escape(long self, String stringValue);

    public static Expression exists(StatementSelect select) {
        assert select != null;
        Expression exp = new Expression();
        if(select == null) {
            return exp;
        }
        exp.cppObj = createWithExistStatement(select.getCppObj());
        return exp;
    }

    private static native long createWithExistStatement(long select);

    public static Expression notExists(StatementSelect select) {
        assert select != null;
        Expression exp = new Expression();
        if(select == null) {
            return exp;
        }
        exp.cppObj = createWithNotExistStatement(select.getCppObj());
        return exp;
    }

    private static native long createWithNotExistStatement(long select);

    public static Expression cast(String columnName) {
        Expression ret = new Expression();
        ret.cppObj = cast(CPPType.String.ordinal(), 0, columnName);
        return ret;
    }

    public static Expression cast(ExpressionConvertible expression) {
        assert expression != null;
        Expression ret = new Expression();
        if(expression == null) {
            return ret;
        }
        ret.cppObj = cast(expression.asIdentifier().getCppType().ordinal(),
                expression.asIdentifier().getCppObj(), null);
        return ret;
    }

    private static native long cast(int type, long object, String columnName);

    public Expression as(ColumnType type) {
        as(cppObj, type.ordinal());
        return this;
    }

    private native void as(long self, int type);

    public static Expression case_() {
        Expression ret = new Expression();
        ret.cppObj = caseWithExp(0, 0, null);
        return ret;
    }

    public static Expression case_(String columnName) {
        assert columnName != null;
        if(columnName == null) {
            return case_();
        }
        Expression ret = new Expression();
        ret.cppObj = caseWithExp(CPPType.String.ordinal(), 0, columnName);
        return ret;
    }

    public static Expression case_(ExpressionConvertible expression) {
        assert expression != null;
        if(expression == null) {
            return case_();
        }
        Expression ret = new Expression();
        ret.cppObj = caseWithExp(expression.asIdentifier().getCppType().ordinal(),
                expression.asIdentifier().getCppObj(), null);
        return ret;
    }

    private static native long caseWithExp(int type, long object, String columnName);

    public Expression when(boolean arg) {
        setWithWhenExp(cppObj, CPPType.Bool.ordinal(), arg ? 1 : 0, 0, null);
        return this;
    }

    public Expression when(long arg) {
        setWithWhenExp(cppObj, CPPType.Int.ordinal(), arg, 0, null);
        return this;
    }

    public Expression when(double arg) {
        setWithWhenExp(cppObj, CPPType.Double.ordinal(), 0, arg, null);
        return this;
    }

    public Expression when(String arg) {
        if(arg != null) {
            setWithWhenExp(cppObj, CPPType.String.ordinal(), 0, 0, arg);
        }else{
            setWithWhenExp(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }
        return this;
    }

    public Expression when(ExpressionConvertible arg) {
        if(arg != null){
            setWithWhenExp(cppObj, arg.asIdentifier().getCppType().ordinal(), arg.asIdentifier().getCppObj(), 0, null);
        } else {
            setWithWhenExp(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }

        return this;
    }

    private native void setWithWhenExp(long self, int type, long intValue, double doubleValue, String stringValue);

    public Expression then(boolean arg) {
        setWithThenExp(cppObj, CPPType.Bool.ordinal(), arg ? 1 : 0, 0, null);
        return this;
    }

    public Expression then(long arg) {
        setWithThenExp(cppObj, CPPType.Int.ordinal(), arg, 0, null);
        return this;
    }

    public Expression then(double arg) {
        setWithThenExp(cppObj, CPPType.Double.ordinal(), 0, arg, null);
        return this;
    }

    public Expression then(String arg) {
        if(arg != null) {
            setWithThenExp(cppObj, CPPType.String.ordinal(), 0, 0, arg);
        }else{
            setWithThenExp(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }
        return this;
    }

    public Expression then(ExpressionConvertible arg) {
        if(arg != null){
            setWithThenExp(cppObj, arg.asIdentifier().getCppType().ordinal(), arg.asIdentifier().getCppObj(), 0, null);
        } else {
            setWithThenExp(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }

        return this;
    }

    private native void setWithThenExp(long self, int type, long intValue, double doubleValue, String stringValue);

    public Expression else_(boolean arg) {
        setWithElseExp(cppObj, CPPType.Bool.ordinal(), arg ? 1 : 0, 0, null);
        return this;
    }

    public Expression else_(long arg) {
        setWithElseExp(cppObj, CPPType.Int.ordinal(), arg, 0, null);
        return this;
    }

    public Expression else_(double arg) {
        setWithElseExp(cppObj, CPPType.Double.ordinal(), 0, arg, null);
        return this;
    }

    public Expression else_(String arg) {
        if(arg != null) {
            setWithElseExp(cppObj, CPPType.String.ordinal(), 0, 0, arg);
        }else{
            setWithElseExp(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }
        return this;
    }

    public Expression else_(ExpressionConvertible arg) {
        if(arg != null){
            setWithElseExp(cppObj, arg.asIdentifier().getCppType().ordinal(), arg.asIdentifier().getCppObj(), 0, null);
        } else {
            setWithElseExp(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }

        return this;
    }

    private native void setWithElseExp(long self, int type, long intValue, double doubleValue, String stringValue);

    public static Expression windowFunction(String funcName) {
        Expression ret = new Expression();
        ret.cppObj = createWithWindowFunction(funcName);
        return ret;
    }

    private static native long createWithWindowFunction(String funcName);

    public Expression filter(Expression condition) {
        filter(cppObj, condition.cppObj);
        return this;
    }

    private native void filter(long self, long condition);

    public Expression over(WindowDef windowDef) {
        overWindowDef(cppObj, windowDef.getCppObj());
        return this;
    }

    private native void overWindowDef(long self, long windowDef);

    public Expression over(String window) {
        overWindow(cppObj, window);
        return this;
    }

    private native void overWindow(long self, String window);
}
