// Created by chenqiuwen on 2023/4/3.
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

public class ColumnConstraint extends Identifier {
    @Override
    protected int getType() {
        return CPPType.ColumnConstraint;
    }

    public ColumnConstraint() {
        cppObj = createCppObject(null);
    }

    public ColumnConstraint(@NotNull String columnName) {
        cppObj = createCppObject(columnName);
    }

    private static native long createCppObject(String columnName);

    @NotNull
    public ColumnConstraint primaryKey() {
        configPrimaryKey(cppObj);
        return this;
    }

    private static native void configPrimaryKey(long self);

    @NotNull
    public ColumnConstraint order(Order order) {
        configOrder(cppObj, order.ordinal() + 1);
        return this;
    }

    private static native void configOrder(long self, int order);

    @NotNull
    public ColumnConstraint conflict(ConflictAction action) {
        configConflictAction(cppObj, action.ordinal());
        return this;
    }

    private static native void configConflictAction(long self, int action);

    @NotNull
    public ColumnConstraint autoIncrement() {
        configAutoIncrement(cppObj);
        return this;
    }

    private static native void configAutoIncrement(long self);

    @NotNull
    public ColumnConstraint notNull() {
        configNotNull(cppObj);
        return this;
    }

    private static native void configNotNull(long self);

    @NotNull
    public ColumnConstraint unique() {
        configUnique(cppObj);
        return this;
    }

    private static native void configUnique(long self);

    @NotNull
    public ColumnConstraint check(@Nullable Expression expression) {
        configCheck(cppObj, CppObject.get(expression));
        return this;
    }

    private static native void configCheck(long self, long expression);

    @NotNull
    public ColumnConstraint defaultTo(boolean value) {
        defaultTo(cppObj, CPPType.Bool, value ? 1 : 0, 0, null);
        return this;
    }

    @NotNull
    public ColumnConstraint defaultTo(byte value) {
        defaultTo(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public ColumnConstraint defaultTo(short value) {
        defaultTo(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public ColumnConstraint defaultTo(int value) {
        defaultTo(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public ColumnConstraint defaultTo(long value) {
        defaultTo(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public ColumnConstraint defaultTo(float value) {
        defaultTo(cppObj, CPPType.Double, 0, value, null);
        return this;
    }

    @NotNull
    public ColumnConstraint defaultTo(double value) {
        defaultTo(cppObj, CPPType.Double, 0, value, null);
        return this;
    }

    @NotNull
    public ColumnConstraint defaultTo(@Nullable String value) {
        if (value != null) {
            defaultTo(cppObj, CPPType.String, 0, 0, value);
        } else {
            defaultTo(cppObj, CPPType.Null, 0, 0, null);
        }

        return this;
    }

    @NotNull
    public ColumnConstraint defaultTo(@Nullable ExpressionConvertible value) {
        defaultTo(cppObj, Identifier.getCppType(value), CppObject.get(value), 0, null);
        return this;
    }

    private static native void defaultTo(long self, int type, long intValue, double doubleValue, String stringValue);

    @NotNull
    public ColumnConstraint collate(@NotNull String collation) {
        configCollate(cppObj, collation);
        return this;
    }

    private static native void configCollate(long self, String collation);

    @NotNull
    public ColumnConstraint foreignKey(@Nullable ForeignKey foreignKey) {
        configForeignKey(cppObj, CppObject.get(foreignKey));
        return this;
    }

    private static native void configForeignKey(long self, long foreignKey);

    @NotNull
    public ColumnConstraint unIndex() {
        configUnIndex(cppObj);
        return this;
    }

    private static native void configUnIndex(long self);
}
