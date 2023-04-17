// Created by qiuwenchen on 2023/4/7.
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

package com.tencent.wcdbtest.winq;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import com.tencent.wcdb.winq.*;
import static com.tencent.wcdbtest.base.TestTool.winqEqual;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class TableOrSubqueryTest {
    @Test
    public void test() {
        winqEqual(new TableOrSubquery("testTable"), "testTable");
        winqEqual(new TableOrSubquery("testTable").schema("testSchema"), "testSchema.testTable");
        winqEqual(new TableOrSubquery("testTable").indexedBy("testIndex"), "testTable INDEXED BY testIndex");
        winqEqual(new TableOrSubquery("testTable").notIndexd(), "testTable NOT INDEXED");
        winqEqual(new TableOrSubquery("testTable").as("testAliasTable"), "testTable AS testAliasTable");
        winqEqual(TableOrSubquery.function("testFunction"), "testFunction()");
        winqEqual(TableOrSubquery.function("testFunction")
                .argument(1).argument("abc").argument((ExpressionConvertible)null).argument(new BindParameter(1)),
                "testFunction(1, 'abc', NULL, ?1)");
        winqEqual(TableOrSubquery.function("testFunction").as("testAliasTable"), "testFunction() AS testAliasTable");
        winqEqual(new TableOrSubquery(new String[]{"testTable1, testTable2"}), "(testTable1, testTable2)");
    }
}
