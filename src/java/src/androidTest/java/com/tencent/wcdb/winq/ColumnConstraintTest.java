package com.tencent.wcdb.winq;// Created by chenqiuwen on 2023/4/6.
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

import static com.tencent.wcdb.TestUtility.TestTool.winqEqual;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class ColumnConstraintTest {
    @Test
    public void test() {
        winqEqual(new ColumnConstraint().primaryKey(), "PRIMARY KEY");
        winqEqual(new ColumnConstraint("testColumnConstraint").primaryKey(),
                "CONSTRAINT testColumnConstraint PRIMARY KEY");
        winqEqual(new ColumnConstraint().primaryKey().order(Order.Asc), "PRIMARY KEY ASC");
        winqEqual(new ColumnConstraint().primaryKey().conflict(ConflictAction.Abort),
                "PRIMARY KEY ON CONFLICT ABORT");
        winqEqual(new ColumnConstraint().primaryKey().autoIncrement(), "PRIMARY KEY AUTOINCREMENT");
        winqEqual(new ColumnConstraint("testColumnConstraint").notNull(),
                "CONSTRAINT testColumnConstraint NOT NULL");
        winqEqual(new ColumnConstraint().notNull().conflict(ConflictAction.Abort),
                "NOT NULL ON CONFLICT ABORT");
        winqEqual(new ColumnConstraint("testColumnConstraint").unique(),
                "CONSTRAINT testColumnConstraint UNIQUE");
        winqEqual(new ColumnConstraint().defaultTo(1), "DEFAULT 1");
        winqEqual(new ColumnConstraint().defaultTo(false), "DEFAULT FALSE");
        winqEqual(new ColumnConstraint().defaultTo("abc"), "DEFAULT 'abc'");
        winqEqual(new ColumnConstraint().defaultTo((ExpressionConvertible)null), "DEFAULT NULL");
        winqEqual(new ColumnConstraint().collate("BINARY"), "COLLATE BINARY");
    }
}
