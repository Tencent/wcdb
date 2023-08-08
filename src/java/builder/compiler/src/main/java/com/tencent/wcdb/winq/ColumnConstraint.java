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

public class ColumnConstraint extends Identifier {
    public ColumnConstraint() { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint(String columnName) { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint primaryKey() { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint order(Order order) { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint conflict(ConflictAction action) { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint autoIncrement() { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint notNull() { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint unique() { throw new UnsatisfiedLinkError(); }
//    public ColumnConstraint check(Expression expression) { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint defaultTo(boolean value) { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint defaultTo(long value) { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint defaultTo(double value) { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint defaultTo(String value) { throw new UnsatisfiedLinkError(); }
//    public ColumnConstraint defaultTo(ExpressionConvertible value) { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint collate(String collation) { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint foreignKey(ForeignKey foreignKey) { throw new UnsatisfiedLinkError(); }
    public ColumnConstraint unIndex() { throw new UnsatisfiedLinkError(); }
}
