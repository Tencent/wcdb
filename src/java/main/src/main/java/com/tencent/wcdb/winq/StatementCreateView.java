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

public class StatementCreateView extends Statement {
    @Override
    protected int getType() {
        return CPPType.CreateViewSTMT;
    }

    public StatementCreateView() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public StatementCreateView createView(@NotNull String name) {
        configView(cppObj, name);
        return this;
    }

    @NotNull
    public StatementCreateView createTempView(@NotNull String name) {
        configView(cppObj, name);
        configTemp(cppObj);
        return this;
    }

    private static native void configView(long self, String name);

    private static native void configTemp(long self);

    @NotNull
    public StatementCreateView of(@Nullable String schemaName) {
        configSchema(cppObj, CPPType.String, 0, schemaName);
        return this;
    }

    @NotNull
    public StatementCreateView of(@Nullable Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void configSchema(long self, int type, long object, String path);

    @NotNull
    public StatementCreateView ifNotExist() {
        configIfNotExist(cppObj);
        return this;
    }

    private static native void configIfNotExist(long self);

    @NotNull
    public StatementCreateView withColumns(@NotNull Column... columns) {
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
    public StatementCreateView withColumns(@NotNull String... columnNames) {
        if (columnNames.length == 0) {
            return this;
        }
        configColumns(cppObj, CPPType.String, null, columnNames);
        return this;
    }

    private static native void configColumns(long self, int type, long[] objects, String[] columnNames);

    @NotNull
    public StatementCreateView as(@NotNull StatementSelect select) {
        configAs(cppObj, CppObject.get(select));
        return this;
    }

    private static native void configAs(long self, long select);
}
