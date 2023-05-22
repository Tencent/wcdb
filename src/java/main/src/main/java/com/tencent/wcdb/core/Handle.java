// Created by qiuwenchen on 2023/4/19.
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
package com.tencent.wcdb.core;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.winq.Statement;

public class Handle extends HandleORMOperation {
    private PreparedStatement mainStatement = null;
    private final Database database;

    Handle(Database database) {
        this.database = database;
    }

    Handle(long cppObj, Database database) {
        this.cppObj = cppObj;
        this.database = database;
    }

    public long getCppHandle() throws WCDBException {
        if(cppObj == 0) {
            assert database != null;
            cppObj = database.getHandle(database.getCppObj());
            if(!checkValid(cppObj)) {
                throw database.createException();
            }
        }
        return cppObj;
    }

    WCDBException createException() {
        return new WCDBException(getError(cppObj));
    }

    native long getError(long self);

    static native boolean checkValid(long self);

    public PreparedStatement getOrCreatePreparedStatement(Statement statement) throws WCDBException {
        PreparedStatement preparedStatement = new PreparedStatement(getOrCreatePreparedStatement(getCppHandle(), statement.getCppObj()));
        if(!preparedStatement.checkPrepared()) {
            throw createException();
        }
        return preparedStatement;
    }

    native long getOrCreatePreparedStatement(long self, long statement);

    public PreparedStatement getOrCreatePreparedStatement(String sql) throws WCDBException {
        PreparedStatement preparedStatement = new PreparedStatement(getOrCreatePreparedStatementWithSQL(getCppHandle(), sql));
        if(!preparedStatement.checkPrepared()) {
            throw createException();
        }
        return preparedStatement;
    }

    native long getOrCreatePreparedStatementWithSQL(long self, String sql);

    public PreparedStatement preparedWithMainStatement(Statement statement) throws WCDBException {
        if(mainStatement == null) {
            mainStatement = new PreparedStatement(getMainStatement(getCppHandle()));
            mainStatement.autoFinalize = true;
        }
        mainStatement.prepare(statement);
        return mainStatement;
    }

    public PreparedStatement preparedWithMainStatement(String sql) throws WCDBException {
        if(mainStatement == null) {
            mainStatement = new PreparedStatement(getMainStatement(getCppHandle()));
            mainStatement.autoFinalize = true;
        }
        mainStatement.prepare(sql);
        return mainStatement;
    }

    native long getMainStatement(long self);

    public void finalizeAllStatements() {
        if(cppObj > 0){
            finalizeAllStatements(cppObj);
        }
    }

    native void finalizeAllStatements(long self);

    public void invalidate() {
        mainStatement = null;
        if(cppObj != 0) {
            releaseCPPObject(cppObj);
            cppObj = 0;
        }
    }

    native int tableExist(long self, String tableName);

    native boolean execute(long self, long statement);

    native boolean executeSQL(long self, String sql);

    public int getChanges() throws WCDBException {
        return getChanges(getCppHandle());
    }

    native int getChanges(long self);

    public int getTotalChanges() throws WCDBException {
        return getTotalChanges(getCppHandle());
    }

    native int getTotalChanges(long self);

    public long getLastInsertedRowId() throws WCDBException {
        return getLastInsertedRowId(getCppHandle());
    }

    native long getLastInsertedRowId(long self);

    native boolean isInTransaction(long self);

    native boolean beginTransaction(long self);

    native boolean commitTransaction(long self);

    native void rollbackTransaction(long self);

    native boolean beginNestedTransaction(long self);

    native boolean commitNestedTransaction(long self);

    native void rollbackNestedTransaction(long self);

    @Override
    Handle getHandle() {
        return this;
    }

    @Override
    Database getDatabase() {
        return database;
    }

    @Override
    boolean autoInvalidateHandle() {
        return false;
    }

    private boolean onTransaction(long cppHandle, Transaction transaction) {
        Handle handle = new Handle(cppHandle, database);
        boolean ret;
        try {
            ret = transaction.insideTransaction(handle);
        } catch (WCDBException e) {
            ret = false;
        }
        return ret;
    }

    native boolean runTransaction(long self, Transaction transaction);

    native boolean runNestedTransaction(long self, Transaction transaction);

    private int onPausableTransaction(long cppHandle, PausableTransaction transaction, boolean isNewTransaction) {
        Handle handle = new Handle(cppHandle, database);
        int ret;
        try {
            ret = transaction.insideTransaction(handle, isNewTransaction) ? 1 : 0;
        } catch (WCDBException e) {
            ret = 2;
        }
        return ret;
    }

    native boolean runPausableTransaction(long self, PausableTransaction transaction);

}
