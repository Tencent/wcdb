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

import java.util.Arrays;

public class WindowDef extends Identifier {
    @Override
    protected CPPType getCppType() {
        return CPPType.WindowDef;
    }

    public WindowDef() {
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public WindowDef partitionBy(String columnName) {
        assert columnName != null;
        if(columnName == null) {
            return this;
        }
        configPartitions(cppObj, new int[]{CPPType.String.ordinal()}, null, null, new String[]{columnName});
        return this;
    }

    public WindowDef partitionBy(ExpressionConvertible expression) {
        assert expression != null;
        if(expression == null) {
            return this;
        }
        configPartitions(cppObj, new int[]{expression.asIdentifier().getCppType().ordinal()},
                new long[]{expression.asIdentifier().getCppObj()}, null, null);
        return this;
    }

    public WindowDef partitionBy(String[] columnNames) {
        assert columnNames != null && columnNames.length > 0;
        if(columnNames == null || columnNames.length == 0){
            return this;
        }
        int[] types = new int[columnNames.length];
        Arrays.fill(types, CPPType.String.ordinal());
        configPartitions(cppObj, types, null, null, columnNames);
        return this;
    }

    public WindowDef partitionBy(ExpressionConvertible[] expressions) {
        assert expressions != null && expressions.length > 0;
        if(expressions == null || expressions.length == 0) {
            return this;
        }
        int[] types = new int[expressions.length];
        long[] cppObjs = new long[expressions.length];
        for(int i = 0; i < expressions.length; i++) {
            types[i] = expressions[i].asIdentifier().getCppType().ordinal();
            cppObjs[i] = expressions[i].asIdentifier().getCppObj();
        }
        configPartitions(cppObj, types, cppObjs, null, null);
        return this;
    }

    private native void configPartitions(long self, int[] types, long[] columns, double[] unused, String[] columnNames);

    public WindowDef orderBy(OrderingTerm order) {
        configOrders(cppObj, new long[]{order.getCppObj()});
        return this;
    }

    public WindowDef orderBy(OrderingTerm[] orders) {
        if(orders == null || orders.length == 0) {
            return this;
        }
        long[] cppOrders = new long[orders.length];
        for(int i = 0; i < orders.length; i++) {
            cppOrders[i] = orders[i].getCppObj();
        }
        configOrders(cppObj, cppOrders);
        return this;
    }

    private native void configOrders(long self, long[] orders);

    public WindowDef frameSpec(FrameSpec frameSpec) {
        configFrameSpec(cppObj, frameSpec.getCppObj());
        return this;
    }

    private native void configFrameSpec(long self, long frameSpec);
}
