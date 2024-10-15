// Created by chenqiuwen on 2023/6/10.
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

import java.util.Arrays;

public class WindowDef extends Identifier {
    @Override
    protected int getType() {
        return CPPType.WindowDef;
    }

    public WindowDef() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    @NotNull
    public WindowDef partitionBy(@Nullable String... columnNames) {
        if (columnNames == null || columnNames.length == 0) {
            return this;
        }
        int[] types = new int[columnNames.length];
        Arrays.fill(types, CPPType.String);
        configPartitions(cppObj, types, null, null, columnNames);
        return this;
    }

    @NotNull
    public WindowDef partitionBy(@Nullable ExpressionConvertible... expressions) {
        if (expressions == null || expressions.length == 0) {
            return this;
        }
        int[] types = new int[expressions.length];
        long[] cppObjs = new long[expressions.length];
        for (int i = 0; i < expressions.length; i++) {
            types[i] = Identifier.getCppType(expressions[i]);
            cppObjs[i] = CppObject.get(expressions[i]);
        }
        configPartitions(cppObj, types, cppObjs, null, null);
        return this;
    }

    private static native void configPartitions(long self, int[] types, long[] columns, double[] unused, String[] columnNames);

    @NotNull
    public WindowDef orderBy(@Nullable OrderingTerm... orders) {
        if (orders == null || orders.length == 0) {
            return this;
        }
        long[] cppOrders = new long[orders.length];
        for (int i = 0; i < orders.length; i++) {
            cppOrders[i] = CppObject.get(orders[i]);
        }
        configOrders(cppObj, cppOrders);
        return this;
    }

    private static native void configOrders(long self, long[] orders);

    @NotNull
    public WindowDef frameSpec(@Nullable FrameSpec frameSpec) {
        configFrameSpec(cppObj, CppObject.get(frameSpec));
        return this;
    }

    private static native void configFrameSpec(long self, long frameSpec);
}
