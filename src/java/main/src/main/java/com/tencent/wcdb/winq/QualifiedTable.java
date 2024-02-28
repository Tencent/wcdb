// Created by chenqiuwen on 2023/4/8.
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

public class QualifiedTable extends Identifier {
    @Override
    protected int getType() {
        return CPPType.QualifiedTableName;
    }

    public QualifiedTable(@NotNull String tableName) {
        cppObj = createCppObj(tableName);
    }

    private static native long createCppObj(String tableName);

    @NotNull
    public QualifiedTable of(@Nullable String schema) {
        schema(cppObj, CPPType.String, 0, schema);
        return this;
    }

    @NotNull
    public QualifiedTable of(@Nullable Schema schema) {
        schema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void schema(long self, int type, long object, String schemaName);

    @NotNull
    public QualifiedTable as(@Nullable String alias) {
        alias(cppObj, alias);
        return this;
    }

    private static native void alias(long self, String alias);

    @NotNull
    public QualifiedTable indexed(@Nullable String indexName) {
        indexed(cppObj, indexName);
        return this;
    }

    private static native void indexed(long self, String indexName);

    @NotNull
    public QualifiedTable notIndexed() {
        notIndexed(cppObj);
        return this;
    }

    private static native void notIndexed(long self);

}
