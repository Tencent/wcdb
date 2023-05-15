// Created by chenqiuwen on 2023/4/6.
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

public class OrderingTerm extends Identifier{
    @Override
    protected CPPType getCppType() {
        return CPPType.OrderingTerm;
    }

    public OrderingTerm(ExpressionConvertible expression) {
        cppObj = createCppObj(expression.asIdentifier().getCppType().ordinal(),
                expression.asIdentifier().getCppObj());
    }

    private native long createCppObj(int type, long expression);

    public OrderingTerm collate(String collation) {
        collate(cppObj, collation);
        return this;
    }

    private native void collate(long orderingTerm, String collation);

    public OrderingTerm order(Order order) {
        order(cppObj, order.ordinal() + 1);
        return this;
    }

    private native void order(long orderingTerm, int order);

    static public OrderingTerm ascendingRowid() {
        return Column.rowId().order(Order.Asc);
    }

    static public OrderingTerm descendingRowid() {
        return Column.rowId().order(Order.Desc);
    }
}
