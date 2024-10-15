// Created by chenqiuwen on 2023/6/11.
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

public class StatementCreateTrigger extends Statement {
    @Override
    protected int getType() {
        return CPPType.CreateTriggerSTMT;
    }

    public StatementCreateTrigger() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public StatementCreateTrigger createTrigger(@NotNull String name) {
        configTrigger(cppObj, name);
        return this;
    }

    @NotNull
    public StatementCreateTrigger createTempTrigger(@NotNull String name) {
        configTrigger(cppObj, name);
        configTemp(cppObj);
        return this;
    }

    private static native void configTrigger(long self, String name);

    private static native void configTemp(long self);

    @NotNull
    public StatementCreateTrigger ofSchema(@Nullable String schemaName) {
        configSchema(cppObj, CPPType.String, 0, schemaName);
        return this;
    }

    @NotNull
    public StatementCreateTrigger ofSchema(@Nullable Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void configSchema(long self, int type, long object, String path);

    @NotNull
    public StatementCreateTrigger ifNotExist() {
        configIfNotExist(cppObj);
        return this;
    }

    private static native void configIfNotExist(long self);

    @NotNull
    public StatementCreateTrigger before() {
        configBefore(cppObj);
        return this;
    }

    private static native void configBefore(long self);

    @NotNull
    public StatementCreateTrigger after() {
        configAfter(cppObj);
        return this;
    }

    private static native void configAfter(long self);

    @NotNull
    public StatementCreateTrigger insteadOf() {
        configInsteadOf(cppObj);
        return this;
    }

    private static native void configInsteadOf(long self);

    @NotNull
    public StatementCreateTrigger delete() {
        configDelete(cppObj);
        return this;
    }

    private static native void configDelete(long self);

    @NotNull
    public StatementCreateTrigger insert() {
        configInsert(cppObj);
        return this;
    }

    private static native void configInsert(long self);

    @NotNull
    public StatementCreateTrigger update() {
        configUpdate(cppObj);
        return this;
    }

    private static native void configUpdate(long self);

    @NotNull
    public StatementCreateTrigger ofColumns(@NotNull Column... columns) {
        if (columns.length == 0) {
            return this;
        }
        long[] cppObjs = new long[columns.length];
        for (int i = 0; i < columns.length; i++) {
            cppObjs[i] = CppObject.get(columns[i]);
        }
        configColumns(cppObj, CPPType.Column, cppObjs, null);
        return this;
    }

    @NotNull
    public StatementCreateTrigger ofColumns(@NotNull String... columnNames) {
        if (columnNames.length == 0) {
            return this;
        }
        configColumns(cppObj, CPPType.String, null, columnNames);
        return this;
    }

    private static native void configColumns(long self, int type, long[] objects, String[] columnNames);

    @NotNull
    public StatementCreateTrigger onTable(@NotNull String tableName) {
        configTable(cppObj, tableName);
        return this;
    }

    private static native void configTable(long self, String table);

    @NotNull
    public StatementCreateTrigger forEachRow() {
        configForEachRow(cppObj);
        return this;
    }

    private static native void configForEachRow(long self);

    @NotNull
    public StatementCreateTrigger when(@Nullable Expression condition) {
        configWhen(cppObj, CppObject.get(condition));
        return this;
    }

    private static native void configWhen(long self, long condition);

    @NotNull
    public StatementCreateTrigger execute(@Nullable StatementInsert insert) {
        executeInsert(cppObj, CppObject.get(insert));
        return this;
    }

    private static native void executeInsert(long self, long insert);

    @NotNull
    public StatementCreateTrigger execute(@Nullable StatementUpdate update) {
        executeUpdate(cppObj, CppObject.get(update));
        return this;
    }

    private static native void executeUpdate(long self, long insert);

    @NotNull
    public StatementCreateTrigger execute(@Nullable StatementDelete delete) {
        executeDelete(cppObj, CppObject.get(delete));
        return this;
    }

    private static native void executeDelete(long self, long insert);

    @NotNull
    public StatementCreateTrigger execute(@Nullable StatementSelect select) {
        executeSelect(cppObj, CppObject.get(select));
        return this;
    }

    private static native void executeSelect(long self, long insert);
}
