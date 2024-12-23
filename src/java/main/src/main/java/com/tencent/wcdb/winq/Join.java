// Created by qiuwenchen on 2023/4/7.
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

public class Join extends Identifier implements TableOrSubqueryConvertible {
    @Override
    protected int getType() {
        return CPPType.JoinClause;
    }

    public Join(@NotNull String tableName) {
        cppObj = createCppObj(CPPType.String, 0, tableName);
    }

    public Join(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        cppObj = createCppObj(Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
    }

    private static native long createCppObj(int type, long object, String tableName);


    @NotNull
    public Join with(@NotNull String tableName) {
        configWith(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public Join with(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        configWith(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWith(long self, int type, long object, String tableName);

    @NotNull
    public Join join(@NotNull String tableName) {
        configJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public Join join(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        configJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configJoin(long self, int type, long object, String tableName);

    @NotNull
    public Join leftOuterJoin(@NotNull String tableName) {
        configWithLeftOuterJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public Join leftOuterJoin(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        configWithLeftOuterJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithLeftOuterJoin(long self, int type, long object, String tableName);

    @NotNull
    public Join leftJoin(@NotNull String tableName) {
        configWithLeftJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public Join leftJoin(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        configWithLeftJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithLeftJoin(long self, int type, long object, String tableName);

    @NotNull
    public Join innerJoin(@NotNull String tableName) {
        configWithInnerJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public Join innerJoin(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        configWithInnerJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithInnerJoin(long self, int type, long object, String tableName);

    @NotNull
    public Join crossJoin(@NotNull String tableName) {
        configWithCrossJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public Join crossJoin(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        configWithCrossJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithCrossJoin(long self, int type, long object, String tableName);

    @NotNull
    public Join naturalJoin(@NotNull String tableName) {
        configWithNaturalJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public Join naturalJoin(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        configWithNaturalJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithNaturalJoin(long self, int type, long object, String tableName);

    @NotNull
    public Join naturalLeftOuterJoin(@NotNull String tableName) {
        configWithNaturalLeftOuterJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public Join naturalLeftOuterJoin(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        configWithNaturalLeftOuterJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithNaturalLeftOuterJoin(long self, int type, long object, String tableName);

    @NotNull
    public Join naturalLeftJoin(@NotNull String tableName) {
        configWithNaturalLeftJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public Join naturalLeftJoin(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        configWithNaturalLeftJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithNaturalLeftJoin(long self, int type, long object, String tableName);

    @NotNull
    public Join naturalInnerJoin(@NotNull String tableName) {
        configWithNaturalInnerJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public Join naturalInnerJoin(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        configWithNaturalInnerJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithNaturalInnerJoin(long self, int type, long object, String tableName);

    @NotNull
    public Join naturalCrossJoin(@NotNull String tableName) {
        configWithNaturalCrossJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    @NotNull
    public Join naturalCrossJoin(@NotNull TableOrSubqueryConvertible tableOrSubquery) {
        configWithNaturalCrossJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithNaturalCrossJoin(long self, int type, long object, String tableName);

    @NotNull
    public Join on(@Nullable Expression expression) {
        configOn(cppObj, CppObject.get(expression));
        return this;
    }

    private static native void configOn(long self, long expression);

    @NotNull
    public Join using(@Nullable String column) {
        configUsingColumn(cppObj, CPPType.String, null, new String[]{column});
        return this;
    }

    @NotNull
    public Join using(@Nullable Column column) {
        configUsingColumn(cppObj, Identifier.getCppType(column), new long[]{CppObject.get(column)}, null);
        return this;
    }

    @NotNull
    public Join using(@Nullable String... columns) {
        configUsingColumn(cppObj, CPPType.String, null, columns);
        return this;
    }

    @NotNull
    public Join using(@Nullable Column... columns) {
        if (columns == null || columns.length == 0) {
            return this;
        }
        long[] cppColumns = new long[columns.length];
        for (int i = 0; i < columns.length; i++) {
            cppColumns[i] = CppObject.get(columns[i]);
        }
        configUsingColumn(cppObj, CPPType.Column, cppColumns, null);
        return this;
    }

    private static native void configUsingColumn(long self, int type, long[] columns, String[] columnNames);
}
