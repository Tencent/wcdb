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

public class IndexedColumn extends Identifier implements IndexedColumnConvertible {
    @Override
    protected CPPType getCppType() {
        return CPPType.IndexedColumn;
    }

    public IndexedColumn(IndexedColumnConvertible indexedColumnConvertible) {
        cppObj = createCppObj(indexedColumnConvertible.asIdentifier().getCppType().ordinal(), indexedColumnConvertible.asIdentifier().getCppObj(), null);
    }

    public IndexedColumn(String columnName) {
        cppObj = createCppObj(CPPType.String.ordinal(), 0, columnName);
    }

    private native long createCppObj(int type, long object, String columnName);

    public IndexedColumn collate(String collation) {
        collate(cppObj, collation);
        return this;
    }

    private native void collate(long indexedColumn, String collation);

    public IndexedColumn order(Order order) {
        order(cppObj, order.ordinal() + 1);
        return this;
    }

    private native void order(long indexedColumn, int order);
}
