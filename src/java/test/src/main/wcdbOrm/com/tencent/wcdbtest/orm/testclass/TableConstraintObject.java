// Created by qiuwenchen on 2023/5/12.
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

package com.tencent.wcdbtest.orm.testclass;

import com.tencent.wcdb.*;

@WCDBTableCoding(
        multiPrimaries = @MultiPrimary(columns = {"multiPrimary1", "multiPrimary2", "multiPrimary3"}),
        multiUnique = @MultiUnique(columns = {"multiUnique1", "multiUnique2", "multiUnique3"}),
        multiIndexes = {
                @MultiIndexes(name = "specifiedNameIndex", columns = {"multiIndex1", "multiIndex2", "multiIndex3"}),
                @MultiIndexes(columns = {"multiIndex1", "multiIndex2"})
        }
)
public class TableConstraintObject {
    @WCDBField
    int multiPrimary1;
    @WCDBField
    int multiPrimary2;
    @WCDBField
    int multiPrimary3;
    @WCDBField
    int multiUnique1;
    @WCDBField
    int multiUnique2;
    @WCDBField
    int multiUnique3;
    @WCDBField
    int multiIndex1;
    @WCDBField
    int multiIndex2;
    @WCDBField
    int multiIndex3;
}
