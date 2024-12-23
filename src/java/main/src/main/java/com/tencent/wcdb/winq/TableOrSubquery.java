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

public class TableOrSubquery extends Identifier implements TableOrSubqueryConvertible {
    @Override
    protected int getType() {
        return CPPType.TableOrSubquery;
    }

    private TableOrSubquery() {
    }

    public TableOrSubquery(@NotNull TableOrSubqueryConvertible tableOrSubqueryConvertible) {
        cppObj = createCppObj(Identifier.getCppType(tableOrSubqueryConvertible), CppObject.get(tableOrSubqueryConvertible),
                null);
    }

    public TableOrSubquery(@NotNull String tableName) {
        cppObj = createCppObj(CPPType.String, 0, tableName);
    }

    private static native long createCppObj(int type, long object, String tableName);

    public TableOrSubquery(@NotNull TableOrSubqueryConvertible... tableOrSubqueries) {
        if (tableOrSubqueries.length == 0) {
            cppObj = createCppObj(CPPType.String, new long[]{}, null);
            return;
        }
        long[] array = new long[tableOrSubqueries.length];
        for (int i = 0; i < tableOrSubqueries.length; i++) {
            array[i] = CppObject.get(tableOrSubqueries[i]);
        }
        cppObj = createCppObj(Identifier.getCppType(tableOrSubqueries[0]), array, null);
    }

    public TableOrSubquery(@NotNull String... tables) {
        cppObj = createCppObj(CPPType.String, null, tables);
    }

    private static native long createCppObj(int type, long[] tableOrSubqueries, String[] tables);

    @NotNull
    public TableOrSubquery schema(@Nullable String schemaName) {
        schema(cppObj, CPPType.String, 0, schemaName);
        return this;
    }

    @NotNull
    public TableOrSubquery schema(@Nullable Schema schema) {
        schema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void schema(long self, int type, long schema, String schemaName);

    @NotNull
    public TableOrSubquery as(@Nullable String alias) {
        as(cppObj, alias);
        return this;
    }

    private static native void as(long object, String alias);

    @NotNull
    public TableOrSubquery notIndexd() {
        notIndexed(cppObj);
        return this;
    }

    private static native void notIndexed(long object);

    @NotNull
    public TableOrSubquery indexedBy(@Nullable String indexName) {
        indexedBy(cppObj, indexName);
        return this;
    }

    private static native void indexedBy(long object, String indexName);

    public static TableOrSubquery function(@NotNull String funcName) {
        TableOrSubquery tableOrSubquery = new TableOrSubquery();
        tableOrSubquery.cppObj = createWithFunction(funcName);
        return tableOrSubquery;
    }

    public static native long createWithFunction(String funcName);

    @NotNull
    public TableOrSubquery argument(boolean arg) {
        argument(cppObj, CPPType.Bool, arg ? 1 : 0, 0, null);
        return this;
    }

    @NotNull
    public TableOrSubquery argument(byte arg) {
        argument(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    @NotNull
    public TableOrSubquery argument(short arg) {
        argument(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    @NotNull
    public TableOrSubquery argument(int arg) {
        argument(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    @NotNull
    public TableOrSubquery argument(long arg) {
        argument(cppObj, CPPType.Int, arg, 0, null);
        return this;
    }

    @NotNull
    public TableOrSubquery argument(float arg) {
        argument(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    @NotNull
    public TableOrSubquery argument(double arg) {
        argument(cppObj, CPPType.Double, 0, arg, null);
        return this;
    }

    @NotNull
    public TableOrSubquery argument(@Nullable String arg) {
        if (arg != null) {
            argument(cppObj, CPPType.String, 0, 0, arg);
        } else {
            argument(cppObj, CPPType.Null, 0, 0, null);
        }
        return this;
    }

    @NotNull
    public TableOrSubquery argument(@Nullable ExpressionConvertible arg) {
        argument(cppObj,
                Identifier.getCppType(arg),
                CppObject.get(arg),
                0,
                null);
        return this;
    }

    private static native void argument(long self,
                                        int type,
                                        long intValue,
                                        double doubleValue,
                                        String stringValue);
}
