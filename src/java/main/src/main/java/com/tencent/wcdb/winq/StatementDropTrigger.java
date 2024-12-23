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

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class StatementDropTrigger extends Statement {
    @Override
    protected int getType() {
        return CPPType.DropTriggerSTMT;
    }

    public StatementDropTrigger() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public StatementDropTrigger dropTrigger(@NotNull String triggerName) {
        configTrigger(cppObj, triggerName);
        return this;
    }

    private static native void configTrigger(long self, String triggerName);

    @NotNull
    public StatementDropTrigger of(@Nullable String schemaName) {
        configSchema(cppObj, CPPType.String, 0, schemaName);
        return this;
    }

    @NotNull
    public StatementDropTrigger of(@Nullable Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void configSchema(long self, int type, long object, String path);

    @NotNull
    public StatementDropTrigger ifExist() {
        configIfExist(cppObj);
        return this;
    }

    private static native void configIfExist(long self);
}
