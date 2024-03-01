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

public class IndexedColumn extends Identifier implements IndexedColumnConvertible {
    @Override
    protected int getType() {
        return CPPType.IndexedColumn;
    }

    public IndexedColumn(@NotNull IndexedColumnConvertible indexedColumnConvertible) {
        cppObj = createCppObj(Identifier.getCppType(indexedColumnConvertible), CppObject.get(indexedColumnConvertible), null);
    }

    public IndexedColumn(@NotNull String columnName) {
        cppObj = createCppObj(CPPType.String, 0, columnName);
    }

    private static native long createCppObj(int type, long object, String columnName);

    @NotNull
    public IndexedColumn collate(String collation) {
        collate(cppObj, collation);
        return this;
    }

    private static native void collate(long indexedColumn, String collation);

    @NotNull
    public IndexedColumn order(Order order) {
        order(cppObj, order.ordinal() + 1);
        return this;
    }

    private static native void order(long indexedColumn, int order);
}
