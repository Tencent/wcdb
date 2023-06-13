// Created by chenqiuwen on 2023/6/11.
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

public class StatementAttach extends Identifier {
    @Override
    protected CPPType getCppType() {
        return CPPType.AttachSTMT;
    }

    public StatementAttach() {
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public StatementAttach attach(String path) {
        configPath(cppObj, CPPType.String.ordinal(), 0, path);
        return this;
    }

    public StatementAttach attach(BindParameter bindParameter) {
        configPath(cppObj, CPPType.BindParameter.ordinal(), bindParameter.getCppObj(), null);
        return this;
    }

    private native void configPath(long self, int type, long object, String path);

    public StatementAttach as(String schemaName) {
        configSchema(cppObj, CPPType.String.ordinal(), 0, schemaName);
        return this;
    }

    public StatementAttach as(Schema schema) {
        configSchema(cppObj, CPPType.Schema.ordinal(), schema.getCppObj(), null);
        return this;
    }

    private native void configSchema(long self, int type, long object, String path);

    public StatementAttach key(String key) {
        configKey(cppObj, CPPType.String.ordinal(), 0, key);
        return this;
    }

    public StatementAttach key(BindParameter bindParameter) {
        configKey(cppObj, CPPType.BindParameter.ordinal(), bindParameter.getCppObj(), null);
        return this;
    }

    private native void configKey(long self, int type, long object, String path);
}
