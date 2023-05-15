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

public class QualifiedTable extends Identifier {
    @Override
    protected CPPType getCppType() {
        return CPPType.QualifiedTableName;
    }

    public QualifiedTable(String tableName) {
        cppObj = createCppObj(tableName);
    }

    private native long createCppObj(String tableName);

    public QualifiedTable of(String schema) {
        schema(cppObj, CPPType.String.ordinal(), 0, schema);
        return this;
    }

    public QualifiedTable of(Schema schema) {
        schema(cppObj, schema.getCppType().ordinal(), schema.getCppObj(), null);
        return this;
    }

    private native void schema(long self, int type, long object, String schemaName);

    public QualifiedTable as(String alias) {
        alias(cppObj, alias);
        return this;
    }

    private native void alias(long self, String alias);

    public QualifiedTable indexed(String indexName) {
        indexed(cppObj, indexName);
        return this;
    }

    private native void indexed(long self, String indexName);

    public QualifiedTable notIndexed() {
        notIndexed(cppObj);
        return this;
    }

    private native void notIndexed(long self);

}
