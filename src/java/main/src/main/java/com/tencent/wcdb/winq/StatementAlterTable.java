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

public class StatementAlterTable extends Statement {
    @Override
    protected int getType() {
        return CPPType.AlterTableSTMT;
    }

    public StatementAlterTable() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public StatementAlterTable alterTable(@NotNull String tableName) {
        configTable(cppObj, tableName);
        return this;
    }

    private static native void configTable(long self, String tableName);

    @NotNull
    public StatementAlterTable of(@Nullable String schemaName) {
        configSchema(cppObj, CPPType.String, 0, schemaName);
        return this;
    }

    @NotNull
    public StatementAlterTable of(@Nullable Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void configSchema(long self, int type, long object, String schemaName);

    @NotNull
    public StatementAlterTable renameTo(@NotNull String tableName) {
        configRenameToTable(cppObj, tableName);
        return this;
    }

    private static native void configRenameToTable(long self, String tableName);

    @NotNull
    public StatementAlterTable renameColumn(@NotNull String columnName) {
        configRenameColumn(cppObj, CPPType.String, 0, columnName);
        return this;
    }

    @NotNull
    public StatementAlterTable renameColumn(@NotNull Column column) {
        configRenameColumn(cppObj, Identifier.getCppType(column), CppObject.get(column), null);
        return this;
    }

    private static native void configRenameColumn(long self, int type, long object, String schemaName);

    @NotNull
    public StatementAlterTable toColumn(@NotNull String columnName) {
        configRenameToColumn(cppObj, CPPType.String, 0, columnName);
        return this;
    }

    @NotNull
    public StatementAlterTable toColumn(@NotNull Column column) {
        configRenameToColumn(cppObj, Identifier.getCppType(column), CppObject.get(column), null);
        return this;
    }

    private static native void configRenameToColumn(long self, int type, long object, String schemaName);

    @NotNull
    public StatementAlterTable addColumn(@NotNull ColumnDef columnDef) {
        configAddColumn(cppObj, CppObject.get(columnDef));
        return this;
    }

    private static native void configAddColumn(long self, long columnDef);

}
