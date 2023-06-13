// Created by qiuwenchen on 2023/5/16.
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

public class CommonTableExpression extends Identifier {
    @Override
    protected CPPType getCppType() {
        return CPPType.CommonTableExpression;
    }

    public CommonTableExpression(String tableName) {
        cppObj = createCPPObject(tableName);
    }

    private native long createCPPObject(String tableName);

    public CommonTableExpression column(Column column) {
        configColumn(cppObj, column.getCppObj());
        return this;
    }

    private native void configColumn(long self, long column);

    public CommonTableExpression as(StatementSelect select) {
        configSelect(cppObj, select.getCppObj());
        return this;
    }

    private native void configSelect(long self, long select);

}
