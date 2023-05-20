// Created by qiuwenchen on 2023/5/10.
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

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.winq.Pragma;
import com.tencent.wcdb.winq.StatementPragma;
import com.tencent.wcdbtest.base.WrappedValue;
import com.tencent.wcdbtest.base.DatabaseTestCase;

import org.junit.After;
import static org.junit.Assert.*;
import org.junit.Test;

public class ConfigTest extends DatabaseTestCase {
    String configName = "testConfig";
    @After
    public void teardown() {
        database.setConfig(configName, null);
        super.teardown();
    }

    @Test
    public void testConfig() {
        final StatementPragma setSecureDelete = new StatementPragma().pragma(Pragma.secureDelete()).toValue(true);
        final StatementPragma unsetSecureDelete = new StatementPragma().pragma(Pragma.secureDelete()).toValue(false);
        final StatementPragma getSecureDelete = new StatementPragma().pragma(Pragma.secureDelete());
        final WrappedValue uninvoked = new WrappedValue();
        database.setConfig(configName, new Database.Config() {
            @Override
            public void onInvocation(Handle handle) throws WCDBException {
                handle.execute(setSecureDelete);
            }
        }, new Database.Config() {
            @Override
            public void onInvocation(Handle handle) throws WCDBException {
                uninvoked.boolValue = true;
                handle.execute(unsetSecureDelete);
            }
        }, Database.ConfigPriority.low);
        expectMode = Expect.SomeSQLs;
        doTestSQL("PRAGMA secure_delete = TRUE", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.close();
                assertTrue(database.canOpen());
            }
        });
        assertTrue(database.getValueFromStatement(getSecureDelete).getBool());
        database.setConfig(configName, null);
        assertTrue(database.canOpen());
        assertTrue(uninvoked.boolValue);
        assertFalse(database.getValueFromStatement(getSecureDelete).getBool());
    }

    @Test
    public void testConfigFail() {
        database.setConfig(configName, new Database.Config() {
            @Override
            public void onInvocation(Handle handle) throws WCDBException {
                throw new WCDBException();
            }
        });
        assertFalse(database.canOpen());
    }

    @Test
    public void tesUnInvokeFail() {
        database.setConfig(configName, new Database.Config() {
            @Override
            public void onInvocation(Handle handle) throws WCDBException {
            }
        }, new Database.Config() {
            @Override
            public void onInvocation(Handle handle) throws WCDBException {
                throw new WCDBException();
            }
        }, Database.ConfigPriority.low);
        assertTrue(database.canOpen());
        database.setConfig(configName, null);
        assertFalse(database.canOpen());
    }

    @Test
    public void testOrderConfig() {
        String config1 = configName + "_1";
        String config2 = configName + "_2";
        String config3 = configName + "_3";
        final WrappedValue step = new WrappedValue();
        database.setConfig(config1, new Database.Config() {
            @Override
            public void onInvocation(Handle handle) throws WCDBException {
                assertEquals(step.intValue,1);
                step.intValue++;
            }
        });

        database.setConfig(config2, new Database.Config() {
            @Override
            public void onInvocation(Handle handle) throws WCDBException {
                assertEquals(step.intValue, 2);
                step.intValue++;
            }
        }, null, Database.ConfigPriority.low);

        database.setConfig(config3, new Database.Config() {
            @Override
            public void onInvocation(Handle handle) throws WCDBException {
                assertEquals(step.intValue, 0);
                step.intValue++;
            }
        }, null, Database.ConfigPriority.high);
        assertTrue(database.canOpen());
        assertEquals(step.intValue, 3);
    }

    @Test
    public void testCipher() {
        byte[] cipher = "123".getBytes();
        byte[] wrongCipher = "456".getBytes();
        database.setCipherKey(cipher);
        assertTrue(database.canOpen());
        database.close();
        database.setCipherKey(wrongCipher);
        assertFalse(database.canOpen());
    }

    @Test
    public void testCipherWithPageSize() {
        byte[] cipher = "123".getBytes();
        int pageSize = 8 * 1024;
        int wrongPageSize = 16 * 1024;
        database.setCipherKey(cipher, pageSize);
        assertTrue(database.canOpen());
        database.close();
        database.setCipherKey(cipher, wrongPageSize);
        assertFalse(database.canOpen());
    }

    @Test
    public void testCipherWithDifferentVersion() {
        byte[] cipher = "123".getBytes();
        int pageSize = 4096;
        database.setCipherKey(cipher, pageSize, Database.CipherVersion.version3);
        assertTrue(database.canOpen());

        database.close();
        database.setCipherKey(cipher, pageSize);
        assertFalse(database.canOpen());

        Database.setDefaultCipherVersion(Database.CipherVersion.version3);
        assertTrue(database.canOpen());

        database.removeFiles();
        database.setCipherKey(cipher, pageSize, Database.CipherVersion.version4);
        assertTrue(database.canOpen());
        database.close();

        database.setCipherKey(cipher, pageSize);
        assertFalse(database.canOpen());
        Database.setDefaultCipherVersion(Database.CipherVersion.version4);
        assertTrue(database.canOpen());
    }
}
