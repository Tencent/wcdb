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

public class StatementAlterTable extends Statement {
    @Override
    protected CPPType getType() {
        return CPPType.AlterTableSTMT;
    }

    public StatementAlterTable() {
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public StatementAlterTable alterTable(String tableName) {
        configTable(cppObj, tableName);
        return this;
    }

    private native void configTable(long self, String tableName);

    public StatementAlterTable of(String schemaName) {
        configSchema(cppObj, CPPType.String.ordinal(), 0, schemaName);
        return this;
    }

    public StatementAlterTable of(Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private native void configSchema(long self, int type, long object, String schemaName);

    public StatementAlterTable renameTo(String tableName) {
        configRenameToTable(cppObj, tableName);
        return this;
    }

    private native void configRenameToTable(long self, String tableName);

    public StatementAlterTable renameColumn(String columnName) {
        configRenameColumn(cppObj, CPPType.String.ordinal(), 0, columnName);
        return this;
    }

    public StatementAlterTable renameColumn(Column column) {
        configRenameColumn(cppObj, Identifier.getCppType(column), CppObject.get(column), null);
        return this;
    }

    private native void configRenameColumn(long self, int type, long object, String schemaName);

    public StatementAlterTable toColumn(String columnName) {
        configRenameToColumn(cppObj, CPPType.String.ordinal(), 0, columnName);
        return this;
    }

    public StatementAlterTable toColumn(Column column) {
        configRenameToColumn(cppObj, Identifier.getCppType(column), CppObject.get(column), null);
        return this;
    }

    private native void configRenameToColumn(long self, int type, long object, String schemaName);

    public StatementAlterTable addColumn(ColumnDef columnDef) {
        configAddColumn(cppObj, CppObject.get(columnDef));
        return this;
    }

    private native void configAddColumn(long self, long columnDef);

}
