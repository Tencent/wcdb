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

package com.tencent.wcdb.winq;

public enum ColumnType {
    Null(0),
    Integer(1),
    Float(2),
    Text(3),
    BLOB(4);

    private int value;

    private ColumnType(int value) {
        this.value = value;
    }

    public static ColumnType valueOf(int value) {
        switch (value) {
            case 1:
                return Integer;
            case 2:
                return Float;
            case 3:
                return Text;
            case 4:
                return BLOB;
            default:
                return Null;
        }
    }
}
