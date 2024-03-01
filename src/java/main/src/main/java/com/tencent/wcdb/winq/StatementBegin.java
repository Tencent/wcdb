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

import org.jetbrains.annotations.NotNull;

public class StatementBegin extends Statement {
    static class TransactionType {
        final static int Deferred = 0;
        final static int Immediate = 1;
        final static int Exclusive = 2;
    }

    public StatementBegin() {
        cppObj = createCppObj(TransactionType.Deferred);
    }

    StatementBegin(int type) {
        cppObj = createCppObj(type);
    }

    @Override
    protected int getType() {
        return CPPType.CommitSTMT;
    }

    @NotNull
    public static StatementBegin beginDeferred() {
        return new StatementBegin(TransactionType.Deferred);
    }

    @NotNull
    public static StatementBegin beginImmediate() {
        return new StatementBegin(TransactionType.Immediate);
    }

    @NotNull
    public static StatementBegin beginExclusive() {
        return new StatementBegin(TransactionType.Exclusive);
    }

    private static native long createCppObj(int type);
}
