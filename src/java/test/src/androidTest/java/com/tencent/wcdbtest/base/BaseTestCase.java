// Created by chenqiuwen on 2023/5/7.
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

package com.tencent.wcdbtest.base;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.rules.TemporaryFolder;

import java.io.File;

public class BaseTestCase {
    public String currentDirectory;
    @BeforeClass
    public static void globalSetUp() {
        Database.globalTracePerformance(null);
        Database.globalTraceSQL(null);
        Database.globalTraceException(new Database.ExceptionTracer() {
            @Override
            public void onTrace(WCDBException exception) {
                if(exception.level.ordinal() >= WCDBException.Level.Error.ordinal()) {
                    logError(exception.toString());
                } else {
                    logInfo(exception.toString());
                }
                assert exception.level != WCDBException.Level.Fatal;
            }
        });
    }

    @Before
    public void setup() throws WCDBException {
        currentDirectory = tempFolder.getRoot().getPath() + File.separator + getClass().getSimpleName();
        logInfo("Current directory: " + currentDirectory);
    }

    public static void logInfo(String msg, Object... args) {
        String log = msg;
        if(args != null && args.length > 0) {
            log = String.format(msg, args);
        }
        System.out.println(log);
    }

    public static void logError(String msg, Object... args) {
        String log = msg;
        if(args != null && args.length > 0) {
            log = String.format(msg, args);
        }
        System.err.println(log);
    }

    public static void sleep(long millisecond) {
        try {
            Thread.sleep(millisecond);
        } catch (InterruptedException e) {
            BaseTestCase.logError("sleep fail with error " + e);
        }
    }

    @Rule
    public TemporaryFolder tempFolder = new TemporaryFolder();
}
