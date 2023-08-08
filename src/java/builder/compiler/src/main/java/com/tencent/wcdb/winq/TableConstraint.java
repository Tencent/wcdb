// Created by chenqiuwen on 2023/4/9.
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

public class TableConstraint extends Identifier {
    public TableConstraint() { throw new UnsatisfiedLinkError(); }
    public TableConstraint(String name){ throw new UnsatisfiedLinkError(); }
    public TableConstraint primaryKey() { throw new UnsatisfiedLinkError(); }
    public TableConstraint unique() { throw new UnsatisfiedLinkError(); }
    public TableConstraint indexedBy(IndexedColumnConvertible indexedColumn) { throw new UnsatisfiedLinkError(); }
    public TableConstraint indexedBy(IndexedColumnConvertible[] indexedColumns) { throw new UnsatisfiedLinkError(); }
    public TableConstraint indexedBy(String indexedColumn) { throw new UnsatisfiedLinkError(); }
    public TableConstraint indexedBy(String[] indexedColumns) { throw new UnsatisfiedLinkError(); }
    public TableConstraint onConflict(ConflictAction action) { throw new UnsatisfiedLinkError(); }
//    public TableConstraint check(Expression expression) { throw new UnsatisfiedLinkError(); }
    public TableConstraint foreignKey(String[] columns, ForeignKey foreignKey) { throw new UnsatisfiedLinkError(); }
    public TableConstraint foreignKey(Column[] columns, ForeignKey foreignKey) { throw new UnsatisfiedLinkError(); }
}
