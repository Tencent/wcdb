// Created by chenqiuwen on 2023/8/11.
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

package com.tencent.wcdbtest.database;

import static org.junit.Assert.assertTrue;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.TableTestCase;
import com.tencent.wcdbtest.base.TestObject;
import com.tencent.wcdbtest.base.WrappedValue;

import org.junit.Test;

import java.util.List;

public class HandleTest extends TableTestCase {
    @Test
    public void testCancellationSignal() throws WCDBException, InterruptedException {
        createTable();
        TestObject[] objects = RandomTool.autoIncrementTestCaseObjects(10000);
        database.insertObjects(objects, DBTestObject.allFields(), tableName);

        WrappedValue hasTestInterrupt = new WrappedValue();
        Handle.CancellationSignal signal = new Handle.CancellationSignal();
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                Handle handle = database.getHandle();
                try {
                    handle.attachCancellationSignal(signal);
                    List<TestObject> objects = handle.getAllObjects(DBTestObject.allFields(), tableName);
                } catch (WCDBException e) {
                    if(e.level == WCDBException.Level.Error && e.code == WCDBException.Code.Interrupt) {
                        hasTestInterrupt.boolValue = true;
                    }
                } finally {
                    handle.invalidate();
                }
            }
        });
        thread.start();
        sleep(10);
        signal.cancel();
        thread.join();
        assertTrue(hasTestInterrupt.boolValue);
    }
}
