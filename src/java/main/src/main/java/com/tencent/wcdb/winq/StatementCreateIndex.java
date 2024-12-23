// Created by chenqiuwen on 2023/4/10.
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

public class StatementCreateIndex extends Statement {
    @Override
    protected int getType() {
        return CPPType.CreateIndexSTMT;
    }

    public StatementCreateIndex() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public StatementCreateIndex createIndex(@NotNull String indexName) {
        createIndex(cppObj, indexName);
        return this;
    }

    private static native void createIndex(long self, String indexName);

    @NotNull
    public StatementCreateIndex unique() {
        configUnique(cppObj);
        return this;
    }

    private static native void configUnique(long self);

    @NotNull
    public StatementCreateIndex ifNotExist() {
        configIfNotExist(cppObj);
        return this;
    }

    private static native void configIfNotExist(long self);

    @NotNull
    public StatementCreateIndex of(@Nullable String schemaName) {
        configSchema(cppObj, CPPType.String, 0, schemaName);
        return this;
    }

    @NotNull
    public StatementCreateIndex of(@Nullable Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void configSchema(long self, int type, long schema, String schemaName);

    @NotNull
    public StatementCreateIndex on(@NotNull String tableName) {
        configTableName(cppObj, tableName);
        return this;
    }

    private static native void configTableName(long self, String tableName);

    @NotNull
    public StatementCreateIndex indexedBy(@NotNull IndexedColumnConvertible... indexedColumnConvertible) {
        if (indexedColumnConvertible.length == 0) {
            return this;
        }
        long[] columns = new long[indexedColumnConvertible.length];
        for (int i = 0; i < indexedColumnConvertible.length; i++) {
            columns[i] = CppObject.get(indexedColumnConvertible[i]);
        }
        configIndexedColumns(
                cppObj,
                Identifier.getCppType(indexedColumnConvertible[0]),
                columns, null);
        return this;
    }

    @NotNull
    public StatementCreateIndex indexedBy(@NotNull String... indexedColumnNames) {
        configIndexedColumns(cppObj, CPPType.String, null, indexedColumnNames);
        return this;
    }

    private static native void configIndexedColumns(long self, int type, long[] columns, String[] columnNames);

    @NotNull
    public StatementCreateIndex where(@Nullable Expression condition) {
        configCondition(cppObj, CppObject.get(condition));
        return this;
    }

    private static native void configCondition(long self, long condition);
}
