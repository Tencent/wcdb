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

import com.tencent.wcdb.base.CppObject;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class ForeignKey extends Identifier {
    @Override
    protected int getType() {
        return CPPType.ForeignKeyClause;
    }

    public ForeignKey() {
        cppObj = createCPPObj();
    }

    private static native long createCPPObj();

    @NotNull
    public ForeignKey references(@Nullable String table) {
        configReference(cppObj, table);
        return this;
    }

    private static native void configReference(long self, String table);

    @NotNull
    public ForeignKey column(@Nullable Column column) {
        configColumns(cppObj, Identifier.getCppType(column), new long[]{CppObject.get(column)}, null);
        return this;
    }

    @NotNull
    public ForeignKey column(@Nullable String column) {
        configColumns(cppObj, CPPType.String, null, new String[]{column});
        return this;
    }

    @NotNull
    public ForeignKey columns(@Nullable Column... columns) {
        if (columns == null || columns.length == 0) {
            return this;
        }
        long[] objects = new long[columns.length];
        for (int i = 0; i < columns.length; i++) {
            objects[i] = CppObject.get(columns[i]);
        }
        configColumns(cppObj, CPPType.Column, objects, null);
        return this;
    }

    @NotNull
    public ForeignKey columns(@Nullable String... columns) {
        if (columns == null || columns.length == 0) {
            return this;
        }
        configColumns(cppObj, CPPType.String, null, columns);
        return this;
    }

    private static native void configColumns(long self, int type, long[] objects, String[] columnNames);

    public enum Action {
        SetNull, SetDefault, Cascade, Restrict, NoAction
    }

    @NotNull
    public ForeignKey onDelete(Action action) {
        configOnDeleteAction(cppObj, action.ordinal());
        return this;
    }

    private static native void configOnDeleteAction(long self, int action);

    @NotNull
    public ForeignKey onUpdate(Action action) {
        configOnUpdateAction(cppObj, action.ordinal());
        return this;
    }

    private static native void configOnUpdateAction(long self, int action);

    public enum Match {
        Simple, Full, Partial
    }

    @NotNull
    public ForeignKey match(Match match) {
        configMatch(cppObj, match.ordinal() + 1);
        return this;
    }

    private static native void configMatch(long self, int match);

    public enum Initially {
        Default, Deferred, Immediate
    }

    @NotNull
    public ForeignKey deferrable(Initially initially) {
        configDeferrable(cppObj, initially.ordinal());
        return this;
    }

    private static native void configDeferrable(long self, int type);

    @NotNull
    public ForeignKey notDeferrable(Initially initially) {
        configNotDeferrable(cppObj, initially.ordinal());
        return this;
    }

    private static native void configNotDeferrable(long self, int type);
}
