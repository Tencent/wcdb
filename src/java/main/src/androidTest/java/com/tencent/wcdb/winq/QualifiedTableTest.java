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

import androidx.test.ext.junit.runners.AndroidJUnit4;
import static com.tencent.wcdb.base.TestTool.winqEqual;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class QualifiedTableTest {
    @Test
    public void test() {
        winqEqual(new QualifiedTable("testTable"), "testTable");
        winqEqual(new QualifiedTable("testTable").of("testSchema"), "testSchema.testTable");
        winqEqual(new QualifiedTable("testTable").of(new Schema("testSchema")).as("testAlias"),
                "testSchema.testTable AS testAlias");
        winqEqual(new QualifiedTable("testTable").indexed("testIndex"), "testTable INDEXED BY testIndex");
        winqEqual(new QualifiedTable("testTable").notIndexed(), "testTable NOT INDEXED");
    }
}
