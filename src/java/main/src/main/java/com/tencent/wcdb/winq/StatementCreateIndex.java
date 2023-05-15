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

public class StatementCreateIndex extends Statement {
    @Override
    protected CPPType getCppType() {
        return CPPType.CreateIndexSTMT;
    }

    public StatementCreateIndex() {
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public StatementCreateIndex createIndex(String indexName) {
        createIndex(cppObj, indexName);
        return this;
    }

    private native void createIndex(long self, String indexName);

    public StatementCreateIndex unique() {
        configUnique(cppObj);
        return this;
    }

    private native void configUnique(long self);

    public StatementCreateIndex ifNotExist() {
        configIfNotExist(cppObj);
        return this;
    }

    private native void configIfNotExist(long self);

    public StatementCreateIndex of(String schemaName) {
        configSchema(cppObj, CPPType.String.ordinal(), 0, schemaName);
        return this;
    }

    public StatementCreateIndex of(Schema schema) {
        configSchema(cppObj, CPPType.Schema.ordinal(), schema.getCppObj(), null);
        return this;
    }

    private native void configSchema(long self, int type, long schema, String schemaName);

    public StatementCreateIndex on(String tableName) {
        configTableName(cppObj, tableName);
        return this;
    }

    private native void configTableName(long self, String tableName);

    public StatementCreateIndex indexedBy(IndexedColumnConvertible[] indexedColumnConvertible) {
        if(indexedColumnConvertible == null || indexedColumnConvertible.length == 0) {
            return this;
        }
        long[] columns = new long[indexedColumnConvertible.length];
        for(int i = 0; i < indexedColumnConvertible.length; i++) {
            columns[i] = indexedColumnConvertible[i].asIdentifier().getCppObj();
        }
        configIndexedColumns(
                cppObj,
                indexedColumnConvertible[0].asIdentifier().getCppType().ordinal(),
                columns, null);
        return this;
    }

    public StatementCreateIndex indexedBy(IndexedColumnConvertible indexedColumnConvertible) {
        configIndexedColumns(
                cppObj,
                indexedColumnConvertible.asIdentifier().getCppType().ordinal(),
                new long[]{indexedColumnConvertible.asIdentifier().getCppObj()},
                null);
        return this;
    }

    public StatementCreateIndex indexedBy(String[] indexedColumnNames) {
        configIndexedColumns(cppObj, CPPType.String.ordinal(), null, indexedColumnNames);
        return this;
    }

    public StatementCreateIndex indexedBy(String indexedColumnName) {
        configIndexedColumns(cppObj, CPPType.String.ordinal(), null, new String[]{indexedColumnName});
        return this;
    }

    private native void configIndexedColumns(long self, int type, long[] columns, String[] columnNames);

    public StatementCreateIndex where(Expression condition) {
        configCondition(cppObj, condition.getCppObj());
        return this;
    }

    private native void configCondition(long self, long condition);
}
