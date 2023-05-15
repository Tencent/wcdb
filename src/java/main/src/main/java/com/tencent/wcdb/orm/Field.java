// Created by qiuwenchen on 2023/3/30.
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
package com.tencent.wcdb.orm;

import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.Schema;

import java.io.File;

public class Field<T> extends Column {
    protected TableBinding<T> binding = null;

    public TableBinding<T> getTableBinding() {
        return binding;
    }
    protected int fieldId = 0;

    protected String name;

    public int getFieldId() {
        return fieldId;
    }

    private boolean isAutoIncrement = false;

    public boolean isAutoIncrement() {
        return isAutoIncrement;
    }

    public Field(String name, TableBinding<T> binding, int fieldId, boolean isAutoIncrement) {
        super(name, binding.baseBinding().getBaseBinding());
        this.name = name;
        this.binding = binding;
        this.fieldId = fieldId;
        this.isAutoIncrement = isAutoIncrement;
    }

    protected Field() {
        super();
    }

    protected Field<T> copySelf() {
        Field<T> field = new Field<>();
        field.cppObj = copy(cppObj);
        field.name = name;
        field.fieldId = fieldId;
        field.isAutoIncrement = isAutoIncrement;
        field.binding = binding;
        return field;
    }

    public String getName() {
        return name;
    }

    public Field<T> table(String tableName) {
        Field<T> field = copySelf();
        field.inTable(field.cppObj, tableName);
        return field;
    }

    @Override
    public Field<T> of(String schema) {
        Field<T> field = copySelf();
        Schema newSchema = new Schema(schema);
        field.inSchema(field.cppObj, newSchema.getCppObj());
        return field;
    }

    @Override
    public Field<T> of(Schema schema) {
        Field<T> field = copySelf();
        field.inSchema(field.cppObj, schema.getCppObj());
        return field;
    }

    public static String getTableName(Field field) {
        assert field != null;
        if(field != null) {
            assert field.getTableBinding().bindingTableName() != null;
            return field.getTableBinding().bindingTableName();
        }
        return null;
    }

    public static String getTableName(Field[] fields) {
        assert fields != null && fields.length > 0;
        Field field = fields[0];
        return getTableName(field);
    }

    public static <T> TableBinding<T> getBinding(Field<T> field) {
        assert field != null;
        if(field != null) {
            assert field.getTableBinding() != null;
            return field.getTableBinding();
        }
        return null;
    }

    public static <T> TableBinding<T> getBinding(Field<T>[] fields) {
        assert fields != null && fields.length > 0;
        Field field = fields[0];
        return getBinding(field);
    }
}
