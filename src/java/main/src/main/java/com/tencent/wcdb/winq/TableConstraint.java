// Created by chenqiuwen on 2023/4/9.
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

public class TableConstraint extends Identifier {
    @Override
    protected int getType() {
        return CPPType.TableConstraint;
    }

    public TableConstraint() {
        cppObj = createCppObj(null);
    }

    public TableConstraint(@Nullable String name) {
        cppObj = createCppObj(name);
    }

    private static native long createCppObj(String name);

    @NotNull
    public TableConstraint primaryKey() {
        configPrimaryKey(cppObj);
        return this;
    }

    private static native void configPrimaryKey(long self);

    @NotNull
    public TableConstraint unique() {
        configUnique(cppObj);
        return this;
    }

    private static native void configUnique(long self);

    @NotNull
    public TableConstraint indexedBy(@NotNull IndexedColumnConvertible... indexedColumns) {
        if (indexedColumns.length == 0) {
            return this;
        }
        long[] columns = new long[indexedColumns.length];
        for (int i = 0; i < indexedColumns.length; i++) {
            columns[i] = CppObject.get(indexedColumns[i]);
        }
        configIndexedColumn(
                cppObj,
                Identifier.getCppType(indexedColumns[0]),
                columns, null);
        return this;
    }

    @NotNull
    public TableConstraint indexedBy(@NotNull String... indexedColumns) {
        configIndexedColumn(cppObj, CPPType.String, null, indexedColumns);
        return this;
    }

    private static native void configIndexedColumn(long self,
                                                   int type,
                                                   long[] columns,
                                                   String[] columnNames);

    @NotNull
    public TableConstraint onConflict(ConflictAction action) {
        configConfliction(cppObj, action.ordinal());
        return this;
    }

    private static native void configConfliction(long self, int action);

    @NotNull
    public TableConstraint check(@NotNull Expression expression) {
        configCheckExpression(cppObj, CppObject.get(expression));
        return this;
    }

    private static native void configCheckExpression(long self, long expression);

    @NotNull
    public TableConstraint foreignKey(@NotNull ForeignKey foreignKey, @NotNull String... columns) {
        configForeignKey(
                cppObj,
                CPPType.String,
                null,
                columns,
                CppObject.get(foreignKey));
        return this;
    }

    @NotNull
    public TableConstraint foreignKey(@NotNull ForeignKey foreignKey, @NotNull Column... columns) {
        long[] cppObjs = new long[columns.length];
        for (int i = 0; i < columns.length; i++) {
            cppObjs[i] = CppObject.get(columns[i]);
        }
        configForeignKey(
                cppObj,
                CPPType.Column,
                cppObjs,
                null,
                CppObject.get(foreignKey));
        return this;
    }

    private static native void configForeignKey(long self,
                                                int type,
                                                long[] columns,
                                                String[] columnNames,
                                                long foreignKey);
}
