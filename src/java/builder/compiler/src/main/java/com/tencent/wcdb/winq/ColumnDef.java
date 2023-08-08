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

public class ColumnDef {
    public ColumnDef(String columnName) { throw new UnsatisfiedLinkError(); }
    public ColumnDef(Column column) { throw new UnsatisfiedLinkError(); }
    public ColumnDef(String columnName, ColumnType columnType) { throw new UnsatisfiedLinkError(); }
    public ColumnDef(Column column, ColumnType columnType) { throw new UnsatisfiedLinkError(); }

    public ColumnDef constraint(ColumnConstraint constraint) { throw new UnsatisfiedLinkError(); }
//    public ColumnDef makePrimary() { throw new UnsatisfiedLinkError(); }
//    public ColumnDef makePrimary(boolean isAutoIncrement) { throw new UnsatisfiedLinkError(); }
//    public ColumnDef makeDefaultTo(boolean value) { throw new UnsatisfiedLinkError(); }
//    public ColumnDef makeDefaultTo(long value) { throw new UnsatisfiedLinkError(); }
//    public ColumnDef makeDefaultTo(double value) { throw new UnsatisfiedLinkError(); }
//    public ColumnDef makeDefaultTo(String value) { throw new UnsatisfiedLinkError(); }
//    public ColumnDef makeDefaultTo(ExpressionConvertible value) { throw new UnsatisfiedLinkError(); }
//    public ColumnDef makeUnique() { throw new UnsatisfiedLinkError(); }
//    public ColumnDef makeNotNull() { throw new UnsatisfiedLinkError(); }
//    public ColumnDef makeForeignKey(ForeignKey foreignKey) { throw new UnsatisfiedLinkError(); }
//    public ColumnDef makeNotIndexed() { throw new UnsatisfiedLinkError(); }
}
