// Created by chenqiuwen on 2023/4/9.
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

public class StatementBegin extends Statement {
    enum TransactionType {
        Deferred, Immediate, Exclusive
    }

    public StatementBegin() {
        cppObj = createCppObj(TransactionType.Deferred.ordinal());
    }

    StatementBegin(TransactionType type) {
        cppObj = createCppObj(type.ordinal());
    }

    @Override
    protected CPPType getCppType() {
        return CPPType.CommitSTMT;
    }

    public static StatementBegin beginDeferred() {
        return new StatementBegin(TransactionType.Deferred);
    }

    public static StatementBegin beginImmediate() {
        return new StatementBegin(TransactionType.Immediate);
    }

    public static StatementBegin beginExclusive() {
        return new StatementBegin(TransactionType.Exclusive);
    }

    private static native long createCppObj(int type);
}
