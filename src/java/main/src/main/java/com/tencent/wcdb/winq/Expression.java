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

import com.tencent.wcdb.base.CppObject;

public class Expression extends ExpressionOperable implements IndexedColumnConvertible, ResultColumnConvertible{
    @Override
    protected int getType() {
        return CPPType.Expression;
    }

    Expression(){
    }

    public Expression(LiteralValue value) {
        cppObj = createCppObj(Identifier.getCppType(value), CppObject.get(value));
    }

    public Expression(BindParameter bindParameter) {
        cppObj = createCppObj(Identifier.getCppType(bindParameter), CppObject.get(bindParameter));
    }

    public Expression(Column column) {
        cppObj = createCppObj(Identifier.getCppType(column), CppObject.get(column));
    }

    public Expression(StatementSelect select) {
        cppObj = createCppObj(Identifier.getCppType(select), CppObject.get(select));
    }

    private static native long createCppObj(int type, long intValue);

    public static Expression function(String funcName) {
        return createExpression(createWithFunction(funcName));
    }

    private static native long createWithFunction(String func);

    public Expression schema(String schema) {
        schema(cppObj, CPPType.String, 0, schema);
        return this;
    }

    public Expression schema(Schema schema) {
        schema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void schema(long self, int type, long schema, String schemaName);

    public Expression distinct() {
        distinct(cppObj);
        return this;
    }

    private static native void distinct(long self);

    public Expression invoke() {
        invoke(cppObj);
        return this;
    }

    private static native void invoke(long self);

    public Expression invokeAll() {
        invokeAll(cppObj);
        return this;
    }

    private static native void invokeAll(long self);

    public Expression argument(boolean arg) {
        argument(cppObj, CPPType.Bool, arg ? 1 : 0, 0, null);
        return this;
    }

    public Expression argument(byte arg) {
        argument(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression argument(short arg) {
        argument(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression argument(int arg) {
        argument(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression argument(long arg) {
        argument(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression argument(float arg) {
        argument(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    public Expression argument(double arg) {
        argument(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    public Expression argument(String arg) {
        if(arg != null) {
            argument(cppObj, CPPType.String, 0, 0, arg);
        }else{
            argument(cppObj, CPPType.Null, 0, 0, null);
        }
        return this;
    }

    public Expression argument(ExpressionConvertible arg) {
        argument(cppObj, Identifier.getCppType(arg), CppObject.get(arg), 0, null);
        return this;
    }

    private static native void argument(long self, int type, long intValue, double doubleValue, String stringValue);

    public Expression escape(String content) {
        escape(cppObj, content);
        return this;
    }

    private static native void escape(long self, String stringValue);

    public static Expression exists(StatementSelect select) {
        assert select != null;
        Expression exp = new Expression();
        exp.cppObj = createWithExistStatement(CppObject.get(select));
        return exp;
    }

    private static native long createWithExistStatement(long select);

    public static Expression notExists(StatementSelect select) {
        assert select != null;
        Expression exp = new Expression();
        exp.cppObj = createWithNotExistStatement(CppObject.get(select));
        return exp;
    }

    private static native long createWithNotExistStatement(long select);

    public static Expression cast(String columnName) {
        Expression ret = new Expression();
        ret.cppObj = cast(CPPType.String, 0, columnName);
        return ret;
    }

    public static Expression cast(ExpressionConvertible expression) {
        assert expression != null;
        Expression ret = new Expression();
        ret.cppObj = cast(Identifier.getCppType(expression), CppObject.get(expression), null);
        return ret;
    }

    private static native long cast(int type, long object, String columnName);

    public Expression as(ColumnType type) {
        as(cppObj, type.ordinal());
        return this;
    }

    private static native void as(long self, int type);

    public static Expression case_() {
        Expression ret = new Expression();
        ret.cppObj = caseWithExp(0, 0, null);
        return ret;
    }

    public static Expression case_(String columnName) {
        if(columnName == null) {
            return case_();
        }
        Expression ret = new Expression();
        ret.cppObj = caseWithExp(CPPType.String, 0, columnName);
        return ret;
    }

    public static Expression case_(ExpressionConvertible expression) {
        Expression ret = new Expression();
        ret.cppObj = caseWithExp(Identifier.getCppType(expression),
                CppObject.get(expression), null);
        return ret;
    }

    private static native long caseWithExp(int type, long object, String columnName);

    public Expression when(boolean arg) {
        setWithWhenExp(cppObj, CPPType.Bool, arg ? 1 : 0, 0, null);
        return this;
    }

    public Expression when(byte arg) {
        setWithWhenExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression when(short arg) {
        setWithWhenExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression when(int arg) {
        setWithWhenExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression when(long arg) {
        setWithWhenExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression when(float arg) {
        setWithWhenExp(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    public Expression when(double arg) {
        setWithWhenExp(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    public Expression when(String arg) {
        if(arg != null) {
            setWithWhenExp(cppObj, CPPType.String, 0, 0, arg);
        }else{
            setWithWhenExp(cppObj, CPPType.Null, 0, 0, null);
        }
        return this;
    }

    public Expression when(ExpressionConvertible arg) {
        setWithWhenExp(cppObj, Identifier.getCppType(arg), CppObject.get(arg), 0, null);
        return this;
    }

    private static native void setWithWhenExp(long self, int type, long intValue, double doubleValue, String stringValue);

    public Expression then(boolean arg) {
        setWithThenExp(cppObj, CPPType.Bool, arg ? 1 : 0, 0, null);
        return this;
    }

    public Expression then(byte arg) {
        setWithThenExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression then(short arg) {
        setWithThenExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression then(int arg) {
        setWithThenExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression then(long arg) {
        setWithThenExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression then(float arg) {
        setWithThenExp(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    public Expression then(double arg) {
        setWithThenExp(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    public Expression then(String arg) {
        if(arg != null) {
            setWithThenExp(cppObj, CPPType.String, 0, 0, arg);
        }else{
            setWithThenExp(cppObj, CPPType.Null, 0, 0, null);
        }
        return this;
    }

    public Expression then(ExpressionConvertible arg) {
        setWithThenExp(cppObj, Identifier.getCppType(arg), CppObject.get(arg), 0, null);

        return this;
    }

    private static native void setWithThenExp(long self, int type, long intValue, double doubleValue, String stringValue);

    public Expression else_(boolean arg) {
        setWithElseExp(cppObj, CPPType.Bool, arg ? 1 : 0, 0, null);
        return this;
    }

    public Expression else_(byte arg) {
        setWithElseExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression else_(short arg) {
        setWithElseExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression else_(int arg) {
        setWithElseExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression else_(long arg) {
        setWithElseExp(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    public Expression else_(float arg) {
        setWithElseExp(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    public Expression else_(double arg) {
        setWithElseExp(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    public Expression else_(String arg) {
        if(arg != null) {
            setWithElseExp(cppObj, CPPType.String, 0, 0, arg);
        }else{
            setWithElseExp(cppObj, CPPType.Null, 0, 0, null);
        }
        return this;
    }

    public Expression else_(ExpressionConvertible arg) {
        setWithElseExp(cppObj, Identifier.getCppType(arg), CppObject.get(arg), 0, null);
        return this;
    }

    private static native void setWithElseExp(long self, int type, long intValue, double doubleValue, String stringValue);

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

    private static native void filter(long self, long condition);

    public Expression over(WindowDef windowDef) {
        overWindowDef(cppObj, CppObject.get(windowDef));
        return this;
    }

    private static native void overWindowDef(long self, long windowDef);

    public Expression over(String window) {
        overWindow(cppObj, window);
        return this;
    }

    private static native void overWindow(long self, String window);
}
