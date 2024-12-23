// Created by qiuwenchen on 2023/3/31.
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

public class Column extends ExpressionOperable implements IndexedColumnConvertible, ResultColumnConvertible {

    @Override
    protected int getType() {
        return CPPType.Column;
    }

    public Column(@NotNull String name) {
        cppObj = createCppObj(name, 0);
    }

    protected Column(String name, long tableBinding) {
        cppObj = createCppObj(name, tableBinding);
    }

    private static native long createCppObj(String name, long tableBinding);

    protected native long copy(long self);

    @NotNull
    public Column table(@Nullable String table) {
        inTable(cppObj, table);
        return this;
    }

    protected native void inTable(long column, String table);

    @NotNull
    public Column of(@Nullable Schema schema) {
        ofSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    @NotNull
    public Column of(@Nullable String schema) {
        ofSchema(cppObj, CPPType.String, 0, schema);
        return this;
    }

    protected void ofSchema(Schema schema) {
        ofSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
    }

    protected void ofSchema(String schema) {
        ofSchema(cppObj, CPPType.String, 0, schema);
    }

    protected native void ofSchema(long column, int type, long schema, String schemaName);

    @NotNull
    public ResultColumn as(@Nullable String alias) {
        return new ResultColumn(configAlias(cppObj, alias));
    }

    private native static long configAlias(long self, String alias);

    protected Column() {
    }

    public static Column all() {
        Column ret = new Column();
        ret.cppObj = allColumn();
        return ret;
    }

    private static native long allColumn();

    @NotNull
    public static Column rowId() {
        Column ret = new Column();
        ret.cppObj = rowidColumn();
        return ret;
    }

    private static native long rowidColumn();

    @NotNull
    public OrderingTerm order(Order order) {
        return new OrderingTerm(this).order(order);
    }

    @NotNull
    public ColumnDef asDef(ColumnType columnType) {
        return new ColumnDef(this, columnType);
    }
}
