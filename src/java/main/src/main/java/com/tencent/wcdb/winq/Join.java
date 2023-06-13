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

public class Join extends Identifier implements TableOrSubqueryConvertible{
    @Override
    protected CPPType getCppType() {
        return CPPType.JoinClause;
    }

    public Join(String tableName) {
        cppObj = createCppObj(CPPType.String.ordinal(), 0, tableName);
    }

    public Join(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            cppObj = createCppObj(CPPType.String.ordinal(),
                    0, null);
            return;
        }
        cppObj = createCppObj(tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
    }

    private native long createCppObj(int type, long object, String tableName);

    public Join with(String tableName) {
        configWith(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public Join with(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            return this;
        }
        configWith(cppObj, tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
        return this;
    }

    private native void configWith(long self, int type, long object, String tableName);

    public Join join(String tableName) {
        configJoin(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public Join join(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            return this;
        }
        configJoin(cppObj, tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
        return this;
    }

    private native void configJoin(long self, int type, long object, String tableName);

    public Join leftOuterJoin(String tableName) {
        configWithLeftOuterJoin(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public Join leftOuterJoin(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            return this;
        }
        configWithLeftOuterJoin(cppObj, tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
        return this;
    }

    private native void configWithLeftOuterJoin(long self, int type, long object, String tableName);

    public Join leftJoin(String tableName) {
        configWithLeftJoin(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public Join leftJoin(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            return this;
        }
        configWithLeftJoin(cppObj, tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
        return this;
    }

    private native void configWithLeftJoin(long self, int type, long object, String tableName);

    public Join innerJoin(String tableName) {
        configWithInnerJoin(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public Join innerJoin(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            return this;
        }
        configWithInnerJoin(cppObj, tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
        return this;
    }

    private native void configWithInnerJoin(long self, int type, long object, String tableName);

    public Join crossJoin(String tableName) {
        configWithCrossJoin(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public Join crossJoin(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            return this;
        }
        configWithCrossJoin(cppObj, tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
        return this;
    }

    private native void configWithCrossJoin(long self, int type, long object, String tableName);

    public Join naturalJoin(String tableName) {
        configWithNaturalJoin(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public Join naturalJoin(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            return this;
        }
        configWithNaturalJoin(cppObj, tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
        return this;
    }

    private native void configWithNaturalJoin(long self, int type, long object, String tableName);

    public Join naturalLeftOuterJoin(String tableName) {
        configWithNaturalLeftOuterJoin(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public Join naturalLeftOuterJoin(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            return this;
        }
        configWithNaturalLeftOuterJoin(cppObj, tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
        return this;
    }

    private native void configWithNaturalLeftOuterJoin(long self, int type, long object, String tableName);

    public Join naturalLeftJoin(String tableName) {
        configWithNaturalLeftJoin(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public Join naturalLeftJoin(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            return this;
        }
        configWithNaturalLeftJoin(cppObj, tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
        return this;
    }

    private native void configWithNaturalLeftJoin(long self, int type, long object, String tableName);

    public Join naturalInnerJoin(String tableName) {
        configWithNaturalInnerJoin(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public Join naturalInnerJoin(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            return this;
        }
        configWithNaturalInnerJoin(cppObj, tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
        return this;
    }

    private native void configWithNaturalInnerJoin(long self, int type, long object, String tableName);

    public Join naturalCrossJoin(String tableName) {
        configWithNaturalCrossJoin(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public Join naturalCrossJoin(TableOrSubqueryConvertible tableOrSubquery) {
        assert tableOrSubquery != null;
        if(tableOrSubquery == null) {
            return this;
        }
        configWithNaturalCrossJoin(cppObj, tableOrSubquery.asIdentifier().getCppType().ordinal(),
                tableOrSubquery.asIdentifier().getCppObj(), null);
        return this;
    }

    private native void configWithNaturalCrossJoin(long self, int type, long object, String tableName);

    public Join on(Expression expression) {
        configOn(cppObj, expression.getCppObj());
        return this;
    }

    private native void configOn(long self, long expression);

    public Join using(String column) {
        configUsingColumn(cppObj, CPPType.String.ordinal(), null, new String[]{column});
        return this;
    }

    public Join using(Column column) {
        assert column != null;
        if(column == null) {
            return this;
        }
        configUsingColumn(cppObj, CPPType.Column.ordinal(), new long[]{column.getCppObj()}, null);
        return this;
    }

    public Join using(String[] columns) {
        configUsingColumn(cppObj, CPPType.String.ordinal(), null, columns);
        return this;
    }

    public Join using(Column[] columns) {
        assert columns != null && columns.length > 0;
        if(columns == null || columns.length == 0) {
            return this;
        }
        long[] cppColumns = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppColumns[i] = columns[i].getCppObj();
        }
        configUsingColumn(cppObj, CPPType.Column.ordinal(), cppColumns, null);
        return this;
    }

    private native void configUsingColumn(long self, int type, long[] columns, String[] columnNames);
}
