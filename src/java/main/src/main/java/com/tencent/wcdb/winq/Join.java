// Created by qiuwenchen on 2023/4/7.
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

public class Join extends Identifier implements TableOrSubqueryConvertible{
    @Override
    protected int getType() {
        return CPPType.JoinClause;
    }

    public Join(String tableName) {
        cppObj = createCppObj(CPPType.String, 0, tableName);
    }

    public Join(TableOrSubqueryConvertible tableOrSubquery) {
        cppObj = createCppObj(Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
    }

    private static native long createCppObj(int type, long object, String tableName);

    public Join with(String tableName) {
        configWith(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    public Join with(TableOrSubqueryConvertible tableOrSubquery) {
        configWith(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWith(long self, int type, long object, String tableName);

    public Join join(String tableName) {
        configJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    public Join join(TableOrSubqueryConvertible tableOrSubquery) {
        configJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configJoin(long self, int type, long object, String tableName);

    public Join leftOuterJoin(String tableName) {
        configWithLeftOuterJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    public Join leftOuterJoin(TableOrSubqueryConvertible tableOrSubquery) {
        configWithLeftOuterJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithLeftOuterJoin(long self, int type, long object, String tableName);

    public Join leftJoin(String tableName) {
        configWithLeftJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    public Join leftJoin(TableOrSubqueryConvertible tableOrSubquery) {
        configWithLeftJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithLeftJoin(long self, int type, long object, String tableName);

    public Join innerJoin(String tableName) {
        configWithInnerJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    public Join innerJoin(TableOrSubqueryConvertible tableOrSubquery) {
        configWithInnerJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithInnerJoin(long self, int type, long object, String tableName);

    public Join crossJoin(String tableName) {
        configWithCrossJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    public Join crossJoin(TableOrSubqueryConvertible tableOrSubquery) {
        configWithCrossJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithCrossJoin(long self, int type, long object, String tableName);

    public Join naturalJoin(String tableName) {
        configWithNaturalJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    public Join naturalJoin(TableOrSubqueryConvertible tableOrSubquery) {
        configWithNaturalJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithNaturalJoin(long self, int type, long object, String tableName);

    public Join naturalLeftOuterJoin(String tableName) {
        configWithNaturalLeftOuterJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    public Join naturalLeftOuterJoin(TableOrSubqueryConvertible tableOrSubquery) {
        configWithNaturalLeftOuterJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithNaturalLeftOuterJoin(long self, int type, long object, String tableName);

    public Join naturalLeftJoin(String tableName) {
        configWithNaturalLeftJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    public Join naturalLeftJoin(TableOrSubqueryConvertible tableOrSubquery) {
        configWithNaturalLeftJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithNaturalLeftJoin(long self, int type, long object, String tableName);

    public Join naturalInnerJoin(String tableName) {
        configWithNaturalInnerJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    public Join naturalInnerJoin(TableOrSubqueryConvertible tableOrSubquery) {
        configWithNaturalInnerJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithNaturalInnerJoin(long self, int type, long object, String tableName);

    public Join naturalCrossJoin(String tableName) {
        configWithNaturalCrossJoin(cppObj, CPPType.String, 0, tableName);
        return this;
    }

    public Join naturalCrossJoin(TableOrSubqueryConvertible tableOrSubquery) {
        configWithNaturalCrossJoin(cppObj, Identifier.getCppType(tableOrSubquery), CppObject.get(tableOrSubquery), null);
        return this;
    }

    private static native void configWithNaturalCrossJoin(long self, int type, long object, String tableName);

    public Join on(Expression expression) {
        configOn(cppObj, CppObject.get(expression));
        return this;
    }

    private static native void configOn(long self, long expression);

    public Join using(String column) {
        configUsingColumn(cppObj, CPPType.String, null, new String[]{column});
        return this;
    }

    public Join using(Column column) {
        configUsingColumn(cppObj, Identifier.getCppType(column), new long[]{CppObject.get(column)}, null);
        return this;
    }

    public Join using(String... columns) {
        configUsingColumn(cppObj, CPPType.String, null, columns);
        return this;
    }

    public Join using(Column... columns) {
        if(columns == null || columns.length == 0) {
            return this;
        }
        long[] cppColumns = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppColumns[i] = CppObject.get(columns[i]);
        }
        configUsingColumn(cppObj, CPPType.Column, cppColumns, null);
        return this;
    }

    private static native void configUsingColumn(long self, int type, long[] columns, String[] columnNames);
}
