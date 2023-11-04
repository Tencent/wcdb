// Created by chenqiuwen on 2023/11/4.
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

public class StatementCreateVirtualTable extends Statement {
    @Override
    protected int getType() {
        return CPPType.CreateVirtualTableSTMT;
    }

    public StatementCreateVirtualTable() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    public StatementCreateVirtualTable createVirtualTable(String name) {
        configTable(cppObj, name);
        return this;
    }

    private static native void configTable(long self, String name);

    public StatementCreateVirtualTable of(String schemaName) {
        configSchema(cppObj, CPPType.String, 0, schemaName);
        return this;
    }

    public StatementCreateVirtualTable of(Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void configSchema(long self, int type, long object, String path);

    public StatementCreateVirtualTable ifNotExist() {
        configIfNotExist(cppObj);
        return this;
    }

    private static native void configIfNotExist(long self);

    public StatementCreateVirtualTable usingModule(String module) {
        configModule(cppObj, module);
        return this;
    }

    private static native void configModule(long self, String module);

    public StatementCreateVirtualTable arguments(String... arguments) {
        configArguments(cppObj, arguments);
        return this;
    }

    private static native void configArguments(long self, String[] argument);

}
