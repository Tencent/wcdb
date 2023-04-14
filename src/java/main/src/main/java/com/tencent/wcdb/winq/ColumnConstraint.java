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

import com.tencent.wcdb.winq.Identifier;

public class ColumnConstraint extends Identifier {
    @Override
    protected CPPType getCppType() {
        return CPPType.ColumnConstraint;
    }

    public ColumnConstraint() {
        cppObj = createCppObject(null);
    }

    public ColumnConstraint(String columnName) {
        cppObj = createCppObject(columnName);
    }

    private native long createCppObject(String columnName);

    public ColumnConstraint primaryKey() {
        configPrimaryKey(cppObj);
        return this;
    }

    private native void configPrimaryKey(long self);

    public ColumnConstraint order(Order order) {
        configOrder(cppObj, order.ordinal() + 1);
        return this;
    }

    private native void configOrder(long self, int order);

    public ColumnConstraint conflict(ConflictAction action) {
        configConflictAction(cppObj, action.ordinal() + 1);
        return this;
    }

    private native void configConflictAction(long self, int action);

    public ColumnConstraint autoIncrement() {
        configAutoIncrement(cppObj);
        return this;
    }

    private native void configAutoIncrement(long self);

    public ColumnConstraint notNull() {
        configNotNull(cppObj);
        return this;
    }

    private native void configNotNull(long self);

    public ColumnConstraint unique() {
        configUnique(cppObj);
        return this;
    }

    private native void configUnique(long self);

    public ColumnConstraint check(Expression expression) {
        configCheck(cppObj, expression.cppObj);
        return this;
    }

    private native void configCheck(long self, long expression);

    public ColumnConstraint defaultTo(boolean value) {
        defaultTo(cppObj, CPPType.Bool.ordinal(), value ? 1 : 0, 0, null);
        return this;
    }

    public ColumnConstraint defaultTo(long value) {
        defaultTo(cppObj, CPPType.Int.ordinal(), value, 0, null);
        return this;
    }

    public ColumnConstraint defaultTo(double value) {
        defaultTo(cppObj, CPPType.Double.ordinal(), 0, value, null);
        return this;
    }

    public ColumnConstraint defaultTo(String value) {
        if(value != null) {
            defaultTo(cppObj, CPPType.String.ordinal(), 0, 0, value);
        } else {
            defaultTo(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }

        return this;
    }

    public ColumnConstraint defaultTo(ExpressionConvertible value) {
        if(value != null) {
            defaultTo(cppObj, value.asIdentifier().getCppType().ordinal(), value.asIdentifier().cppObj, 0, null);
        } else {
            defaultTo(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }
        return this;
    }

    private native void defaultTo(long self, int type, long intValue, double doubleValue, String stringValue);

    public ColumnConstraint collate(String collation) {
        configCollate(cppObj, collation);
        return this;
    }

    private native void configCollate(long self, String collation);

    public ColumnConstraint foreignKey(ForeignKey foreignKey) {
        configForeignKey(cppObj, foreignKey.cppObj);
        return this;
    }

    private native void configForeignKey(long self, long foreignKey);

    public ColumnConstraint unIndex() {
        configUnIndex(cppObj);
        return this;
    }

    private native void configUnIndex(long self);
}
