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

public class StatementCreateTable extends Statement {
    @Override
    protected CPPType getType() {
        return CPPType.CreateTableSTMT;
    }

    public StatementCreateTable() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    public StatementCreateTable createTable(String tableName) {
        configTableName(cppObj, tableName);
        return this;
    }

    public StatementCreateTable createTempTable(String tableName) {
        configTableName(cppObj, tableName);
        configTemp(cppObj);
        return this;
    }

    private static native void configTableName(long self, String tableName);

    private static native void configTemp(long self);

    public StatementCreateTable ifNotExist() {
        configIfNotExist(cppObj);
        return this;
    }

    private static native void configIfNotExist(long self);

    public StatementCreateTable of(String schemaName) {
        configSchema(cppObj, CPPType.String.ordinal(), 0, schemaName);
        return this;
    }

    public StatementCreateTable of(Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void configSchema(long self, int type, long schema, String schemaName);

    public StatementCreateTable as(StatementSelect select) {
        configAs(cppObj, CppObject.get(select));
        return this;
    }

    private static native void configAs(long self, long select);

    public StatementCreateTable define(ColumnDef column) {
        configColumn(cppObj, CppObject.get(column));
        return this;
    }

    private static native void configColumn(long self, long column);

    public StatementCreateTable define(ColumnDef[] columns) {
        if(columns == null || columns.length == 0) {
            return this;
        }
        long[] cppColumns = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppColumns[i] = CppObject.get(columns[i]);
        }
        configColumns(cppObj, cppColumns);
        return this;
    }

    private static native void configColumns(long self, long[] columns);

    public StatementCreateTable constraint(TableConstraint[] constraints) {
        if(constraints == null || constraints.length == 0) {
            return this;
        }
        long[] cppConstraints = new long[constraints.length];
        for(int i = 0; i < constraints.length; i++) {
            cppConstraints[i] = CppObject.get(constraints[i]);
        }
        configConstraints(cppObj, cppConstraints);
        return this;
    }

    public StatementCreateTable constraint(TableConstraint constraint) {
        configConstraints(cppObj, new long[]{CppObject.get(constraint)});
        return this;
    }

    private static native void configConstraints(long self, long[] constraints);

    public StatementCreateTable withoutRowid() {
        configWithoutRowid(cppObj);
        return this;
    }

    private static native void configWithoutRowid(long self);
}
