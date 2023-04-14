// Created by qiuwenchen on 2023/4/11.
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

public class StatementDelete extends Statement {
    @Override
    protected CPPType getCppType() {
        return CPPType.DeleteSTMT;
    }

    public StatementDelete() {
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public StatementDelete deleteFrom(String tableName) {
        configTable(cppObj, CPPType.String.ordinal(), 0, tableName);
        return this;
    }

    public StatementDelete deleteFrom(QualifiedTable table) {
        configTable(cppObj, table.getCppType().ordinal(), table.cppObj, null);
        return this;
    }

    private native void configTable(long self, int type, long table, String tableName);

    public StatementDelete where(Expression condition) {
        configCondition(cppObj, condition.cppObj);
        return this;
    }

    private native void configCondition(long self, long condition);

    public StatementDelete orderBy(OrderingTerm order) {
        configOrders(cppObj, new long[]{order.cppObj});
        return this;
    }

    public StatementDelete orderBy(OrderingTerm[] orders) {
        if(orders == null || orders.length == 0) {
            return this;
        }
        long[] cppOrders = new long[orders.length];
        for(int i = 0; i < orders.length; i++) {
            cppOrders[i] = orders[i].cppObj;
        }
        configOrders(cppObj, cppOrders);
        return this;
    }

    private native void configOrders(long self, long[] orders);

    public StatementDelete limit(long from, long to) {
        configLimitRange(cppObj, CPPType.Int.ordinal(), from, CPPType.Int.ordinal(), to);
        return this;
    }

    public StatementDelete limit(long from, ExpressionConvertible to) {
        configLimitRange(cppObj, CPPType.Int.ordinal(), from, to.asIdentifier().getCppType().ordinal(), to.asIdentifier().cppObj);
        return this;
    }

    public StatementDelete limit(ExpressionConvertible from, ExpressionConvertible to) {
        configLimitRange(cppObj, from.asIdentifier().getCppType().ordinal(), from.asIdentifier().cppObj, to.asIdentifier().getCppType().ordinal(), to.asIdentifier().cppObj);
        return this;
    }

    public StatementDelete limit(ExpressionConvertible from, long to) {
        configLimitRange(cppObj, from.asIdentifier().getCppType().ordinal(), from.asIdentifier().cppObj, CPPType.Int.ordinal(), to);
        return this;
    }

    private native void configLimitRange(long self, int fromType, long from, int toType, long to);

    public StatementDelete limit(long count) {
        configLimitCount(cppObj, CPPType.Int.ordinal(), count);
        return this;
    }

    public StatementDelete limit(ExpressionConvertible count) {
        configLimitCount(cppObj, count.asIdentifier().getCppType().ordinal(), count.asIdentifier().cppObj);
        return this;
    }

    private native void configLimitCount(long self, int type, long count);

    public StatementDelete offset(long offset) {
        configOffset(cppObj, CPPType.Int.ordinal(), offset);
        return this;
    }

    public StatementDelete offset(ExpressionConvertible offset) {
        configOffset(cppObj, offset.asIdentifier().getCppType().ordinal(), offset.asIdentifier().cppObj);
        return this;
    }

    private native void configOffset(long self, int type, long count);
}
