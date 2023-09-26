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

public class StatementCreateTrigger extends Statement {
    @Override
    protected int getType() {
        return CPPType.CreateTriggerSTMT;
    }

    public StatementCreateTrigger() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    public StatementCreateTrigger createTrigger(String name) {
        configTrigger(cppObj, name);
        return this;
    }

    public StatementCreateTrigger createTempTrigger(String name) {
        configTrigger(cppObj, name);
        configTemp(cppObj);
        return this;
    }

    private static native void configTrigger(long self, String name);

    private static native void configTemp(long self);

    public StatementCreateTrigger ofSchema(String schemaName) {
        configSchema(cppObj, CPPType.String, 0, schemaName);
        return this;
    }

    public StatementCreateTrigger ofSchema(Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void configSchema(long self, int type, long object, String path);

    public StatementCreateTrigger ifNotExist() {
        configIfNotExist(cppObj);
        return this;
    }

    private static native void configIfNotExist(long self);

    public StatementCreateTrigger before() {
        configBefore(cppObj);
        return this;
    }

    private static native void configBefore(long self);

    public StatementCreateTrigger after() {
        configAfter(cppObj);
        return this;
    }

    private static native void configAfter(long self);

    public StatementCreateTrigger insteadOf() {
        configInsteadOf(cppObj);
        return this;
    }

    private static native void configInsteadOf(long self);

    public StatementCreateTrigger delete() {
        configDelete(cppObj);
        return this;
    }

    private static native void configDelete(long self);

    public StatementCreateTrigger insert() {
        configInsert(cppObj);
        return this;
    }

    private static native void configInsert(long self);

    public StatementCreateTrigger update() {
        configUpdate(cppObj);
        return this;
    }

    private static native void configUpdate(long self);

    public StatementCreateTrigger ofColumn(Column column) {
        configColumns(cppObj, Identifier.getCppType(column), new long[]{CppObject.get(column)}, null);
        return this;
    }

    public StatementCreateTrigger ofColumn(String columnName) {
        configColumns(cppObj, CPPType.String, null, new String[]{columnName});
        return this;
    }

    public StatementCreateTrigger ofColumns(Column[] columns) {
        if(columns == null || columns.length == 0) {
            return this;
        }
        long[] cppObjs = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppObjs[i] = CppObject.get(columns[i]);
        }
        configColumns(cppObj, CPPType.Column, cppObjs, null);
        return this;
    }

    public StatementCreateTrigger ofColumns(String[] columnNames) {
        if(columnNames == null || columnNames.length == 0) {
            return this;
        }
        configColumns(cppObj, CPPType.String, null, columnNames);
        return this;
    }

    private static native void configColumns(long self, int type, long[] objects, String[] columnNames);

    public StatementCreateTrigger onTable(String tableName) {
        configTable(cppObj, tableName);
        return this;
    }

    private static native void configTable(long self, String table);

    public StatementCreateTrigger forEachRow() {
        configForEachRow(cppObj);
        return this;
    }

    private static native void configForEachRow(long self);

    public StatementCreateTrigger when(Expression condition) {
        configWhen(cppObj, CppObject.get(condition));
        return this;
    }

    private static native void configWhen(long self, long condition);

    public StatementCreateTrigger execute(StatementInsert insert) {
        executeInsert(cppObj, CppObject.get(insert));
        return this;
    }

    private static native void executeInsert(long self, long insert);

    public StatementCreateTrigger execute(StatementUpdate update) {
        executeUpdate(cppObj, CppObject.get(update));
        return this;
    }

    private static native void executeUpdate(long self, long insert);

    public StatementCreateTrigger execute(StatementDelete delete) {
        executeDelete(cppObj, CppObject.get(delete));
        return this;
    }

    private static native void executeDelete(long self, long insert);

    public StatementCreateTrigger execute(StatementSelect select) {
        executeSelect(cppObj, CppObject.get(select));
        return this;
    }

    private static native void executeSelect(long self, long insert);
}
