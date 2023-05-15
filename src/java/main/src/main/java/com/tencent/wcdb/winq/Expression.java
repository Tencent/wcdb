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

    public Expression(){
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
}
