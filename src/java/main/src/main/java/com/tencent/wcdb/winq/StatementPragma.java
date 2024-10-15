// Created by qiuwenchen on 2023/4/13.
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

public class StatementPragma extends Statement {
    @Override
    protected int getType() {
        return CPPType.PragmaSTMT;
    }

    public StatementPragma() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public StatementPragma pragma(@NotNull Pragma pragma) {
        configPragma(cppObj, CppObject.get(pragma));
        return this;
    }

    private static native void configPragma(long self, long pragma);

    @NotNull
    public StatementPragma of(@Nullable String schemaName) {
        configSchema(cppObj, CPPType.String, 0, schemaName);
        return this;
    }

    @NotNull
    public StatementPragma of(@Nullable Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void configSchema(long self, int type, long schema, String schemaName);

    @NotNull
    public StatementPragma withValue(boolean value) {
        configWithValue(cppObj, CPPType.Bool, value ? 1 : 0, 0, null);
        return this;
    }

    @NotNull
    public StatementPragma withValue(byte value) {
        configWithValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public StatementPragma withValue(short value) {
        configWithValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public StatementPragma withValue(int value) {
        configWithValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public StatementPragma withValue(long value) {
        configWithValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public StatementPragma withValue(float value) {
        configWithValue(cppObj, CPPType.Double, 0, value, null);
        return this;
    }

    @NotNull
    public StatementPragma withValue(double value) {
        configWithValue(cppObj, CPPType.Double, 0, value, null);
        return this;
    }

    @NotNull
    public StatementPragma withValue(@Nullable String value) {
        if (value != null) {
            configWithValue(cppObj, CPPType.String, 0, 0, value);
        } else {
            configWithValue(cppObj, CPPType.Null, 0, 0, null);
        }
        return this;
    }

    private static native void configWithValue(long self, int type, long longValue, double doubleValue, String stringValue);

    @NotNull
    public StatementPragma toValue(boolean value) {
        configToValue(cppObj, CPPType.Bool, value ? 1 : 0, 0, null);
        return this;
    }

    @NotNull
    public StatementPragma toValue(byte value) {
        configToValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public StatementPragma toValue(short value) {
        configToValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public StatementPragma toValue(int value) {
        configToValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public StatementPragma toValue(long value) {
        configToValue(cppObj, CPPType.Int, value, 0, null);
        return this;
    }

    @NotNull
    public StatementPragma toValue(float value) {
        configToValue(cppObj, CPPType.Double, 0, value, null);
        return this;
    }

    @NotNull
    public StatementPragma toValue(double value) {
        configToValue(cppObj, CPPType.Double, 0, value, null);
        return this;
    }

    @NotNull
    public StatementPragma toValue(@Nullable String value) {
        if (value != null) {
            configToValue(cppObj, CPPType.String, 0, 0, value);
        } else {
            configToValue(cppObj, CPPType.Null, 0, 0, null);
        }
        return this;
    }

    private static native void configToValue(long self, int type, long longValue, double doubleValue, String stringValue);
}
