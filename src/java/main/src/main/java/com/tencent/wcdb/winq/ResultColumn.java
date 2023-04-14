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

public class ResultColumn extends Identifier implements ResultColumnConvertible {
    @Override
    protected CPPType getCppType() {
        return CPPType.ResultColumn;
    }


    public ResultColumn(ResultColumnConvertible resultColumnConvertible) {
        cppObj = createCppObj(resultColumnConvertible.asIdentifier().getCppType().ordinal(), resultColumnConvertible.asIdentifier().cppObj, null);
    }

    public ResultColumn(String columnName) {
        cppObj = createCppObj(CPPType.String.ordinal(), 0, columnName);
    }

    private native long createCppObj(int type, long convertibleObj, String columnName);

    public ResultColumn as(String alias) {
        configAlias(cppObj, alias);
        return this;
    }

    private native void configAlias(long resultColumn, String alias);
}
