// Created by qiuwenchen on 2023/4/11.
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

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class StatementDelete extends Statement {
    @Override
    protected int getType() {
        return CPPType.DeleteSTMT;
    }

    public StatementDelete() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public StatementDelete with(CommonTableExpression... expressions) {
        if (expressions == null || expressions.length == 0) {
            return this;
        }
        long[] cppExps = new long[expressions.length];
        for (int i = 0; i < expressions.length; i++) {
            cppExps[i] = CppObject.get(expressions[i]);
        }
        configWith(cppObj, cppExps);
        return this;
    }

    @NotNull
    public StatementDelete withRecursive(@Nullable CommonTableExpression... expressions) {
        if (expressions == null || expressions.length == 0) {
            return this;
        }
        long[] cppExps = new long[expressions.length];
        for (int i = 0; i < expressions.length; i++) {
            cppExps[i] = CppObject.get(expressions[i]);
        }
        configWith(cppObj, cppExps);
        configRecursive(cppObj);
        return this;
    }

    private static native void configWith(long self, long[] expressions);

    private static native void configRecursive(long self);

    @NotNull
    public StatementDelete deleteFrom(@NotNull String tableName) {
        configTable(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public StatementDelete deleteFrom(@NotNull QualifiedTable table) {
        configTable(cppObj, Identifier.getCppType(table), CppObject.get(table), null);
        return this;
    }

    private static native void configTable(long self, int type, long table, String tableName);

    @NotNull
    public StatementDelete where(@Nullable Expression condition) {
        configCondition(cppObj, CppObject.get(condition));
        return this;
    }

    private static native void configCondition(long self, long condition);

    @NotNull
    public StatementDelete orderBy(@Nullable OrderingTerm... orders) {
        if (orders == null || orders.length == 0) {
            return this;
        }
        long[] cppOrders = new long[orders.length];
        for (int i = 0; i < orders.length; i++) {
            cppOrders[i] = CppObject.get(orders[i]);
        }
        configOrders(cppObj, cppOrders);
        return this;
    }

    private static native void configOrders(long self, long[] orders);

    @NotNull
    public StatementDelete limit(long from, long to) {
        configLimitRange(cppObj, CPPType.Int, from, CPPType.Int, to);
        return this;
    }

    @NotNull
    public StatementDelete limit(long from, @Nullable ExpressionConvertible to) {
        configLimitRange(cppObj, CPPType.Int, from, Identifier.getCppType(to), CppObject.get(to));
        return this;
    }

    @NotNull
    public StatementDelete limit(@Nullable ExpressionConvertible from, @Nullable ExpressionConvertible to) {
        configLimitRange(cppObj, Identifier.getCppType(from), CppObject.get(from), Identifier.getCppType(to), CppObject.get(to));
        return this;
    }

    @NotNull
    public StatementDelete limit(@Nullable ExpressionConvertible from, long to) {
        configLimitRange(cppObj, Identifier.getCppType(from), CppObject.get(from), CPPType.Int, to);
        return this;
    }

    private static native void configLimitRange(long self, int fromType, long from, int toType, long to);

    @NotNull
    public StatementDelete limit(long count) {
        configLimitCount(cppObj, CPPType.Int, count);
        return this;
    }

    @NotNull
    public StatementDelete limit(@Nullable ExpressionConvertible count) {
        configLimitCount(cppObj, Identifier.getCppType(count), CppObject.get(count));
        return this;
    }

    private static native void configLimitCount(long self, int type, long count);

    @NotNull
    public StatementDelete offset(long offset) {
        configOffset(cppObj, CPPType.Int, offset);
        return this;
    }

    @NotNull
    public StatementDelete offset(@Nullable ExpressionConvertible offset) {
        configOffset(cppObj, Identifier.getCppType(offset), CppObject.get(offset));
        return this;
    }

    private static native void configOffset(long self, int type, long count);
}
