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

public class ResultColumn extends Identifier implements ResultColumnConvertible {
    @Override
    protected int getType() {
        return CPPType.ResultColumn;
    }

    protected ResultColumn(long cppObj) {
        this.cppObj = cppObj;
    }

    public ResultColumn(@NotNull ResultColumnConvertible resultColumnConvertible) {
        cppObj = createCppObj(Identifier.getCppType(resultColumnConvertible), CppObject.get(resultColumnConvertible), null);
    }

    public ResultColumn(@NotNull String columnName) {
        cppObj = createCppObj(CPPType.String, 0, columnName);
    }

    private static native long createCppObj(int type, long convertibleObj, String columnName);

    @NotNull
    public ResultColumn as(String alias) {
        configAlias(cppObj, alias);
        return this;
    }

    private static native void configAlias(long resultColumn, String alias);
}
