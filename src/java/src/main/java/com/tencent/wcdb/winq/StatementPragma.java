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

import androidx.annotation.ReturnThis;

public class StatementPragma extends Statement {
    @Override
    protected CPPType getCppType() {
        return CPPType.PragmaSTMT;
    }

    public StatementPragma(){
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public StatementPragma pragma(Pragma pragma){
        configPragma(cppObj, pragma.cppObj);
        return this;
    }

    private native void configPragma(long self, long pragma);

    public StatementPragma of(String schemaName) {
        configSchemaName(cppObj, schemaName);
        return this;
    }

    private native void configSchemaName(long self, String schemaName);

    public StatementPragma withValue(boolean value) {
        configWithValue(cppObj, CPPType.Bool.ordinal(), value ? 1 : 0, 0, null);
        return this;
    }

    public StatementPragma withValue(long value) {
        configWithValue(cppObj, CPPType.Int.ordinal(), value, 0, null);
        return this;
    }

    public StatementPragma withValue(double value) {
        configWithValue(cppObj, CPPType.Double.ordinal(), 0, value, null);
        return this;
    }

    public StatementPragma withValue(String value) {
        if(value != null){
            configWithValue(cppObj, CPPType.String.ordinal(), 0, 0, value);
        } else {
            configWithValue(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }
        return this;
    }

    private native void configWithValue(long self, int type, long longValue, double doubleValue, String stringValue);

    public StatementPragma toValue(boolean value) {
        configToValue(cppObj, CPPType.Bool.ordinal(), value ? 1 : 0, 0, null);
        return this;
    }

    public StatementPragma toValue(long value) {
        configToValue(cppObj, CPPType.Int.ordinal(), value, 0, null);
        return this;
    }

    public StatementPragma toValue(double value) {
        configToValue(cppObj, CPPType.Double.ordinal(), 0, value, null);
        return this;
    }

    public StatementPragma toValue(String value) {
        if(value != null){
            configToValue(cppObj, CPPType.String.ordinal(), 0, 0, value);
        } else {
            configToValue(cppObj, CPPType.Null.ordinal(), 0, 0, null);
        }
        return this;
    }

    private native void configToValue(long self, int type, long longValue, double doubleValue, String stringValue);
}
