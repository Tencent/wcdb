// Created by qiuwenchen on 2023/4/6.
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

public class ForeignKey extends Identifier {
    @Override
    protected CPPType getCppType() {
        return CPPType.ForeignKeyClause;
    }

    public ForeignKey() {
        cppObj = createCPPObj();
    }

    private native long createCPPObj();

    public ForeignKey references(String table) {
        configReference(cppObj, table);
        return this;
    }

    private native void configReference(long self, String table);

    public ForeignKey column(Column column) {
        assert column != null;
        if(column == null) {
            return this;
        }
        configColumns(cppObj, CPPType.Column.ordinal(), new long[]{column.getCppObj()}, null);
        return this;
    }

    public ForeignKey column(String column) {
        assert column != null;
        if(column == null) {
            return this;
        }
        configColumns(cppObj, CPPType.String.ordinal(), null, new String[]{column});
        return this;
    }

    public ForeignKey columns(Column[] columns) {
        assert columns != null && columns.length > 0;
        if(columns == null || columns.length == 0) {
            return this;
        }
        long[] objects = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            objects[i] = columns[i].getCppObj();
        }
        configColumns(cppObj, CPPType.Column.ordinal(), objects, null);
        return this;
    }

    public ForeignKey columns(String[] columns) {
        assert columns != null && columns.length > 0;
        if(columns == null || columns.length == 0) {
            return this;
        }
        configColumns(cppObj, CPPType.String.ordinal(), null, columns);
        return this;
    }

    private native void configColumns(long self, int type, long[] objects, String[] columnNames);

    public enum Action {
        SetNull, SetDefault, Cascade, Restrict, NoAction
    }

    public ForeignKey onDelete(Action action) {
        configOnDeleteAction(cppObj, action.ordinal());
        return this;
    }

    private native void configOnDeleteAction(long self, int action);

    public ForeignKey onUpdate(Action action) {
        configOnUpdateAction(cppObj, action.ordinal());
        return this;
    }

    private native void configOnUpdateAction(long self, int action);

    public enum Match {
        Simple, Full, Partial
    }

    public ForeignKey match(Match match) {
        configMatch(cppObj,match.ordinal() + 1);
        return this;
    }

    private native void configMatch(long self, int match);

    public enum Initially {
        Default, Deferred, Immediate
    }

    public ForeignKey deferrable(Initially initially) {
        configDeferrable(cppObj, initially.ordinal());
        return this;
    }

    private native void configDeferrable(long self, int type);

    public ForeignKey notDeferrable(Initially initially) {
        configNotDeferrable(cppObj, initially.ordinal());
        return this;
    }

    private native void configNotDeferrable(long self, int type);
}
