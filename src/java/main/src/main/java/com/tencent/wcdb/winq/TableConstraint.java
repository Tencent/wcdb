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

public class TableConstraint extends Identifier {
    @Override
    protected CPPType getCppType() {
        return CPPType.TableConstraint;
    }

    public TableConstraint() {
        cppObj = createCppObj(null);
    }

    public TableConstraint(String name) {
        cppObj = createCppObj(name);
    }

    private native long createCppObj(String name);

    public TableConstraint primaryKey() {
        configPrimaryKey(cppObj);
        return this;
    }

    private native void configPrimaryKey(long self);

    public TableConstraint unique() {
        configUnique(cppObj);
        return this;
    }

    private native void configUnique(long self);

    public TableConstraint indexedBy(IndexedColumnConvertible indexedColumn) {
        configIndexedColumn(
                cppObj,
                indexedColumn.asIdentifier().getCppType().ordinal(),
                new long[] {indexedColumn.asIdentifier().getCppObj()}, null);
        return this;
    }

    public TableConstraint indexedBy(IndexedColumnConvertible[] indexedColumns) {
        if(indexedColumns == null || indexedColumns.length == 0) {
            return this;
        }
        long[] columns = new long[indexedColumns.length];
        for(int i = 0; i < indexedColumns.length; i++) {
            columns[i] = indexedColumns[i].asIdentifier().getCppObj();
        }
        configIndexedColumn(
                cppObj,
                indexedColumns[0].asIdentifier().getCppType().ordinal(),
                columns, null);
        return this;
    }

    public TableConstraint indexedBy(String indexedColumn) {
        configIndexedColumn(cppObj, CPPType.String.ordinal(), null, new String[]{indexedColumn});
        return this;
    }

    public TableConstraint indexedBy(String[] indexedColumns) {
        configIndexedColumn(cppObj, CPPType.String.ordinal(), null, indexedColumns);
        return this;
    }

    private native void configIndexedColumn(long self,
                                            int type,
                                            long[] columns,
                                            String[] columnNames);

    public TableConstraint onConflict(ConflictAction action) {
        configConfliction(cppObj, action.ordinal() + 1);
        return this;
    }

    private native void configConfliction(long self, int action);

    public TableConstraint check(Expression expression) {
        configCheckExpression(cppObj, expression.getCppObj());
        return this;
    }

    private native void configCheckExpression(long self, long expression);

    public TableConstraint foreignKey(String[] columns, ForeignKey foreignKey) {
        configForeignKey(
                cppObj,
                CPPType.String.ordinal(),
                null,
                columns,
                foreignKey.getCppObj());
        return this;
    }

    public TableConstraint foreignKey(Column[] columns, ForeignKey foreignKey) {
        long[] cppObjs = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppObjs[i] = columns[i].getCppObj();
        }
        configForeignKey(
                cppObj,
                CPPType.Column.ordinal(),
                cppObjs,
                null,
                foreignKey.getCppObj());
        return this;
    }

    private native void configForeignKey(long self,
                                         int type,
                                         long[] columns,
                                         String[] columnNames,
                                         long foreignKey);
}
