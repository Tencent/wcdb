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

import com.tencent.wcdb.base.CppObject;

public class StatementDropTrigger extends Statement {
    @Override
    protected CPPType getType() {
        return CPPType.DropTriggerSTMT;
    }

    public StatementDropTrigger() {
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public StatementDropTrigger dropTrigger(String triggerName) {
        configTrigger(cppObj, triggerName);
        return this;
    }

    private native void configTrigger(long self, String triggerName);

    public StatementDropTrigger of(String schemaName) {
        configSchema(cppObj, CPPType.String.ordinal(), 0, schemaName);
        return this;
    }

    public StatementDropTrigger of(Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private native void configSchema(long self, int type, long object, String path);

    public StatementDropTrigger ifExist() {
        configIfExist(cppObj);
        return this;
    }

    private native void configIfExist(long self);
}
