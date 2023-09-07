// Created by chenqiuwen on 2023/6/24.
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

package com.tencent.wcdbtest.base

import com.tencent.wcdb.base.WCDBException
import com.tencent.wcdb.core.Database
import org.junit.Before
import org.junit.BeforeClass
import org.junit.Rule
import org.junit.rules.TemporaryFolder
import java.io.File

open class BaseTestCase {
    var currentDirectory: String? = null
    @Before
    @Throws(WCDBException::class)
    open fun setup() {
        currentDirectory = tempFolder.root.path + File.separator + javaClass.simpleName
        logInfo("Current directory: $currentDirectory")
    }

    @get:Rule
    var tempFolder = TemporaryFolder()

    companion object {
        @BeforeClass
        @JvmStatic
        fun globalSetUp() {
            Database.globalTracePerformance(null)
            Database.globalTraceSQL(null)
            Database.globalTraceException { exception ->
                if (exception.level.ordinal >= WCDBException.Level.Error.ordinal) {
                    logError(exception.toString())
                } else {
                    logInfo(exception.toString())
                }
                assert(exception.level != WCDBException.Level.Fatal)
            }
        }

        fun logInfo(msg: String?, vararg args: Any?) {
            var log = msg
            if (args.isNotEmpty()) {
                log = String.format(msg!!, *args)
            }
            println(log)
        }

        fun logError(msg: String?, vararg args: Any?) {
            var log = msg
            if (args.isNotEmpty()) {
                log = String.format(msg!!, *args)
            }
            System.err.println(log)
        }

        fun sleep(millisecond: Long) {
            try {
                Thread.sleep(millisecond)
            } catch (e: InterruptedException) {
                logError("sleep fail with error $e")
            }
        }
    }
}