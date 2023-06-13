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

public class StatementCreateView extends Identifier {
    @Override
    protected CPPType getCppType() {
        return CPPType.CreateViewSTMT;
    }

    public StatementCreateView() {
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public StatementCreateView createView(String name) {
        configView(cppObj, name);
        return this;
    }

    public StatementCreateView createTempView(String name) {
        configView(cppObj, name);
        configTemp(cppObj);
        return this;
    }

    private native void configView(long self, String name);

    private native void configTemp(long self);

    public StatementCreateView of(String schemaName) {
        configSchema(cppObj, CPPType.String.ordinal(), 0, schemaName);
        return this;
    }

    public StatementCreateView of(Schema schema) {
        configSchema(cppObj, CPPType.Schema.ordinal(), schema.getCppObj(), null);
        return this;
    }

    private native void configSchema(long self, int type, long object, String path);

    public StatementCreateView ifNotExist() {
        configIfNotExist(cppObj);
        return this;
    }

    private native void configIfNotExist(long self);

    public StatementCreateView withColumn(Column column) {
        assert column != null;
        if(column == null) {
            return this;
        }
        configColumns(cppObj, CPPType.Column.ordinal(), new long[]{column.getCppObj()}, null);
        return this;
    }

    public StatementCreateView withColumn(String columnName) {
        assert columnName != null;
        if(columnName == null) {
            return this;
        }
        configColumns(cppObj, CPPType.String.ordinal(), null, new String[]{columnName});
        return this;
    }

    public StatementCreateView withColumns(Column[] columns) {
        assert columns != null && columns.length > 0;
        if(columns == null || columns.length == 0) {
            return this;
        }
        long[] cppObjs = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppObjs[i] = columns[i].getCppObj();
        }
        configColumns(cppObj, CPPType.Column.ordinal(), cppObjs, null);
        return this;
    }

    public StatementCreateView withColumns(String[] columnNames) {
        assert columnNames != null && columnNames.length > 0;
        if(columnNames == null || columnNames.length == 0) {
            return this;
        }
        configColumns(cppObj, CPPType.String.ordinal(), null, columnNames);
        return this;
    }

    private native void configColumns(long self, int type, long[] objects, String[] columnNames);

    public StatementCreateView as(StatementSelect select) {
        configAs(cppObj, select.getCppObj());
        return this;
    }

    private native void configAs(long self, long select);
}
