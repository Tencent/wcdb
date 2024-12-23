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

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class ColumnDef extends Identifier {
    @Override
    protected int getType() {
        return CPPType.ColumnDef;
    }

    public ColumnDef(@NotNull String columnName) {
        cppObj = createCppObj(CPPType.String, 0, columnName, 0);
    }

    public ColumnDef(@NotNull Column column) {
        cppObj = createCppObj(Identifier.getCppType(column), CppObject.get(column), null, 0);
    }

    public ColumnDef(@NotNull String columnName, ColumnType columnType) {
        cppObj = createCppObj(CPPType.String, 0, columnName, columnType.ordinal());
    }

    public ColumnDef(@NotNull Column column, ColumnType columnType) {
        cppObj = createCppObj(Identifier.getCppType(column), CppObject.get(column), null, columnType.ordinal());
    }

    private static native long createCppObj(int type, long column, String columnName, int columnType);

    @NotNull
    public ColumnDef constraint(ColumnConstraint constraint) {
        constraint(cppObj, CppObject.get(constraint));
        return this;
    }

    private static native void constraint(long columnDef, long columnConstraint);

    @NotNull
    public ColumnDef makePrimary() {
        return constraint(new ColumnConstraint().primaryKey());
    }

    @NotNull
    public ColumnDef makePrimary(boolean isAutoIncrement) {
        return constraint(new ColumnConstraint().primaryKey().autoIncrement());
    }

    @NotNull
    public ColumnDef makeDefaultTo(boolean value) {
        return constraint(new ColumnConstraint().defaultTo(value));
    }

    @NotNull
    public ColumnDef makeDefaultTo(byte value) {
        return constraint(new ColumnConstraint().defaultTo(value));
    }

    @NotNull
    public ColumnDef makeDefaultTo(short value) {
        return constraint(new ColumnConstraint().defaultTo(value));
    }

    @NotNull
    public ColumnDef makeDefaultTo(int value) {
        return constraint(new ColumnConstraint().defaultTo(value));
    }

    @NotNull
    public ColumnDef makeDefaultTo(long value) {
        return constraint(new ColumnConstraint().defaultTo(value));
    }

    @NotNull
    public ColumnDef makeDefaultTo(float value) {
        return constraint(new ColumnConstraint().defaultTo(value));
    }

    @NotNull
    public ColumnDef makeDefaultTo(double value) {
        return constraint(new ColumnConstraint().defaultTo(value));
    }

    @NotNull
    public ColumnDef makeDefaultTo(@Nullable String value) {
        return constraint(new ColumnConstraint().defaultTo(value));
    }

    @NotNull
    public ColumnDef makeDefaultTo(@Nullable ExpressionConvertible value) {
        return constraint(new ColumnConstraint().defaultTo(value));
    }

    @NotNull
    public ColumnDef makeUnique() {
        return constraint(new ColumnConstraint().unique());
    }

    @NotNull
    public ColumnDef makeNotNull() {
        return constraint(new ColumnConstraint().notNull());
    }

    @NotNull
    public ColumnDef makeForeignKey(@Nullable ForeignKey foreignKey) {
        return constraint(new ColumnConstraint().foreignKey(foreignKey));
    }

    @NotNull
    public ColumnDef makeNotIndexed() {
        return constraint(new ColumnConstraint().unIndex());
    }
}
