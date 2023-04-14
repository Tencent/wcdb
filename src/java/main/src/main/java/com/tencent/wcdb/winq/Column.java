// Created by qiuwenchen on 2023/3/31.
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

public class Column extends ExpressionOperable implements IndexedColumnConvertible, ResultColumnConvertible{

    @Override
    protected CPPType getCppType() {
        return CPPType.Column;
    }

    public Column(String name) {
        cppObj = createCppObj(name, 0);
    }

    protected Column(String name, long tableBinding) {
        cppObj = createCppObj(name, tableBinding);
    }

    private native long createCppObj(String name, long tableBinding);

    public Column in(String table) {
        inTable(cppObj, table);
        return this;
    }

    private native void inTable(long column, String table);

    public Column of(Schema schema) {
        inSchema(cppObj, schema.cppObj);
        return this;
    }

    private native void inSchema(long column, long schema);

    public Column of(String schema) {
        return of(new Schema(schema));
    }

    private Column() {
    }


    public static Column all() {
        Column ret = new Column();
        ret.cppObj = allColumn();
        return ret;
    }

    private static native long allColumn();

    public static Column rowId() {
        Column ret = new Column();
        ret.cppObj = rowidColumn();
        return ret;
    }

    private static native long rowidColumn();

    public OrderingTerm order(Order order) {
        return new OrderingTerm(this).order(order);
    }

    public ColumnDef asDef(ColumnType columnType) {
        return new ColumnDef(this, columnType);
    }
}
