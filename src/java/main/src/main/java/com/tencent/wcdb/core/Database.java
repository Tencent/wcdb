// Created by qiuwenchen on 2023/3/30.
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

import java.nio.ByteBuffer;
import java.util.ArrayList;

public class Database extends HandleORMOperation {
    private Database() {
    }

    public Database(String path) {
        cppObj = createDatabase(path);
    }

    private native long createDatabase(String path);

    public String getPath() {
        return getPath(cppObj);
    }

    private native String getPath(long self);

    public ArrayList<String> getPaths() {
        return getPaths(cppObj);
    }

    private native ArrayList<String> getPaths(long self);

    public void setTag(long tag) {
        setTag(cppObj, tag);
    }

    private native void setTag(long self, long tag);

    public long getTag() {
        return getTag(cppObj);
    }

    private native long getTag(long self);

    public boolean canOpen() {
        return canOpen(cppObj);
    }

    private native boolean canOpen(long self);

    public boolean isOpened() {
        return isOpened(cppObj);
    }

    private native boolean isOpened(long self);

    public interface CloseCallBack {
        void onClose() throws WCDBException;
    }

    private static void onClose(CloseCallBack callBack) throws WCDBException {
        callBack.onClose();
    }

    public void close(CloseCallBack callBack) {
        close(cppObj, callBack);
    }

    public void close() {
        close(cppObj, null);
    }

    private native void close(long self, CloseCallBack callBack);

    public void blockade() {
        blockade(cppObj);
    }

    private native void blockade(long self);

    public void unblockade() {
        unblockade(cppObj);
    }

    private native void unblockade(long self);

    public boolean isBlockaded() {
        return isBlockaded(cppObj);
    }

    private native boolean isBlockaded(long self);

    public void purge() {
        purge(cppObj);
    }

    private native void purge(long self);

    public static native void purgeAll();

    WCDBException createException() {
        return new WCDBException(getError(cppObj));
    }

    private native long getError(long self);


    @Override
    public Handle getHandle() {
        return new Handle(this);
    }

    native long getHandle(long self);

    public enum CipherVersion {
        defaultVersion, version1, version2, version3, version4;
    }

    public void setCipherKey(byte[] key) {
        setCipherKey(key, 4096);
    }

    public void setCipherKey(byte[] key, int pageSize) {
        setCipherKey(key, pageSize, CipherVersion.defaultVersion);
    }

    public void setCipherKey(byte[] key, int pageSize, CipherVersion version) {
        setCipherKey(cppObj, key, pageSize, version.ordinal());
    }

    private native void setCipherKey(long self, byte[] key, int pageSize, int version);

    public static void setDefaultCipherVersion(CipherVersion version) {
        setDefaultCipherVersion(version.ordinal());
    }

    private static native void setDefaultCipherVersion(int version);

    public interface Config {
        void onInvocation(Handle handle) throws WCDBException;
    }

    private static boolean onConfig(long cppHandle, Config config) {
        Handle handle = new Handle(cppHandle, null);
        boolean ret = true;
        try {
            config.onInvocation(handle);
        } catch (WCDBException e) {
            ret = false;
        }
        return ret;
    }

    public enum ConfigPriority {
        low, default_, high, highest;
    }

    public void setConfig(String configName, Config invocation) {
        setConfig(configName, invocation, ConfigPriority.default_);
    }

    public void setConfig(String configName, Config invocation, ConfigPriority priority) {
        setConfig(configName, invocation, null, priority);
    }

    public void setConfig(String configName, Config invocation, Config unInvocation, ConfigPriority priority) {
        int cppPriority = 0;
        switch (priority) {
            case low:
                cppPriority = 100;
                break;
            case default_:
                cppPriority = 0;
                break;
            case high:
                cppPriority = -100;
                break;
            case highest:
                cppPriority = -2147483648;
                break;
        }
        setConfig(cppObj, configName, invocation, unInvocation, cppPriority);
    }

    private native void setConfig(long self, String configName, Config invocation, Config unInvocation, int priority);

    public interface PerformanceTracer {
        void onTrace(String path, long handleId, String sql, double time);
    }

    private static void onTracePerformance(PerformanceTracer tracer, String path, long handleId, String sql, double time) {
        tracer.onTrace(path, handleId, sql, time);
    }

    public static native void globalTracePerformance(PerformanceTracer tracer);

    public void tracePerformance(PerformanceTracer tracer) {
        tracePerformance(cppObj, tracer);
    }

    private native void tracePerformance(long self, PerformanceTracer tracer);

    public interface SQLTracer {
        void onTrace(String path, long handleId, String sql);
    }

    private static void onTraceSQL(SQLTracer tracer, String path, long handleId, String sql) {
        tracer.onTrace(path, handleId, sql);
    }

    public static native void globalTraceSQL(SQLTracer tracer);

    public void traceSQL(SQLTracer tracer) {
        traceSQL(cppObj, tracer);
    }

    private native void traceSQL(long self, SQLTracer tracer);

    public interface ExceptionTracer {
        void onTrace(WCDBException exception);
    }

    private static void onTraceException(ExceptionTracer tracer, long cppError) {
        tracer.onTrace(new WCDBException(cppError));
    }

    public static native void globalTraceException(ExceptionTracer tracer);

    public void traceException(ExceptionTracer tracer) {
        traceException(cppObj, tracer);
    }

    private native void traceException(long self, ExceptionTracer tracer);

    public enum Operation {
        Create, SetTag, OpenHandle;
    }

    public interface OperationTracer {
        void onTrace(Database database, Operation operation);
    }

    private static void onTraceOperation(OperationTracer tracer, long cppDatabase, int cppOperation) {
        Database database = new Database();
        database.cppObj = cppDatabase;
        Operation operation = Operation.Create;
        switch (cppOperation) {
            case 1:
                operation = Operation.SetTag;
                break;
            case 2:
                operation = Operation.OpenHandle;
                break;
        }
        tracer.onTrace(database, operation);
    }

    public static native void globalTraceDatabaseOperation(OperationTracer tracer);

    public void removeFiles() throws WCDBException {
        if(!removeFiles(cppObj)) {
            throw createException();
        }
    }

    private native boolean removeFiles(long self);

    public void moveFile(String destination) throws WCDBException{
        if(!moveFile(cppObj, destination)) {
            throw createException();
        }
    }

    private native boolean moveFile(long self, String destination);

    public long getFileSize() throws WCDBException {
        long ret = getFileSize(cppObj);
        if(ret < 0) {
            throw  createException();
        }
        return ret;
    }

    private native long getFileSize(long self);

    public interface CorruptionNotification {
        void onCorrupted(Database database);
    }

    private static void onCorrupted(CorruptionNotification monitor, long cppDatabase) {
        Database database = new Database();
        database.cppObj = cppDatabase;
        monitor.onCorrupted(database);
    }

    public void setNotificationWhenCorrupted(CorruptionNotification monitor) {
        setNotificationWhenCorrupted(cppObj, monitor);
    }

    private native void setNotificationWhenCorrupted(long self, CorruptionNotification monitor);

    public boolean checkIfCorrupted() {
        return checkIfCorrupted(cppObj);
    }

    private native boolean checkIfCorrupted(long self);

    public boolean checkIfIsAlreadyCorrupted() {
        return checkIfIsAlreadyCorrupted(cppObj);
    }

    private native boolean checkIfIsAlreadyCorrupted(long self);

    public void enableAutoBackup(boolean enable) {
        enableAutoBackup(cppObj, enable);
    }

    private native void enableAutoBackup(long self, boolean enable);

    public void backup() throws WCDBException {
        if(!backup(cppObj)) {
            throw createException();
        }
    }

    private native boolean backup(long self);

    public interface BackupFilter {
        boolean tableShouldBeBackup(String tableName);
    }

    private static boolean checkTableShouldBeBackup(BackupFilter filter, String tableName) {
        return filter.tableShouldBeBackup(tableName);
    }

    public void filterBackup(BackupFilter filter) {
        filterBackup(cppObj, filter);
    }

    private native void filterBackup(long self, BackupFilter filter);

    public interface RetrieveProgressMonitor {
        void onProgressUpdate(double percentage, double increment);
    }

    private static void onRetrieveProgressUpdate(RetrieveProgressMonitor progress, double percentage, double increment) {
        progress.onProgressUpdate(percentage, increment);
    }

    public double retrieve(RetrieveProgressMonitor monitor) throws WCDBException {
        double score = retrieve(cppObj, monitor);
        if(score < 0) {
            throw createException();
        }
        return score;
    }

    private native double retrieve(long self, RetrieveProgressMonitor monitor);

    public void deposit() throws WCDBException {
        if(!deposit(cppObj)) {
            throw createException();
        }
    }

    private native boolean deposit(long self);

    public void removeDepositedFiles() throws WCDBException {
        if(!removeDepositedFiles(cppObj)) {
            throw createException();
        }
    }

    private native boolean removeDepositedFiles(long self);

    public boolean containDepositedFiles() {
        return containDepositedFiles(cppObj);
    }

    private native boolean containDepositedFiles(long self);

    public void passiveCheckpoint() throws WCDBException {
        if(!passiveCheckpoint(cppObj)) {
            throw createException();
        }
    }

    private native boolean passiveCheckpoint(long self);

    public void truncateCheckpoint() throws WCDBException {
        if(!truncateCheckpoint(cppObj)) {
            throw createException();
        }
    }

    private native boolean truncateCheckpoint(long self);

    public static class MigrationInfo {
        public String database;
        public String table;
        public String sourceDatabase;
        public String sourceTable;
    }

    public interface MigrationFilter {
        void filterMigrate(MigrationInfo info);
    }

    private static String[] filterMigrate(MigrationFilter filter, String database, String table) {
        MigrationInfo info = new MigrationInfo();
        info.database = database;
        info.table = table;
        filter.filterMigrate(info);
        if(info.sourceTable != null && info.sourceTable.length() > 0 &&
                info.sourceDatabase != null && info.sourceDatabase.length() > 0) {
            return new String[]{info.sourceDatabase, info.sourceTable};
        }
        return null;
    }

    public void filterMigration(MigrationFilter filter) {
        filterMigration(cppObj, filter);
    }

    private native void filterMigration(long self, MigrationFilter filter);

    public void stepMigration() throws WCDBException {
        if(!stepMigration(cppObj)) {
            throw createException();
        }
    }

    private native boolean stepMigration(long self);

    public void enableAutoMigration(boolean enable) {
        enableAutoMigration(cppObj, enable);
    }

    private native void enableAutoMigration(long self, boolean enable);

    public interface MigrationNotification {
        void onMigrated(Database database, MigrationInfo info);
    }

    private static void onTableMigrated(MigrationNotification notification, long cppDatabase, String databasePath, String table, String sourceDatabase, String sourceTable) {
        Database database = new Database();
        database.cppObj = cppDatabase;
        MigrationInfo info = null;
        if(databasePath != null && databasePath.length() > 0) {
            info = new MigrationInfo();
            info.database = databasePath;
            info.table = table;
            info.sourceTable = sourceTable;
            info.sourceDatabase = sourceDatabase;
        }
        notification.onMigrated(database, info);
    }

    public void setNotificationWhenMigrated(MigrationNotification notification) {
        setNotificationWhenMigrated(cppObj, notification);
    }

    private native void setNotificationWhenMigrated(long self, MigrationNotification notification);

    public boolean isMigrated() {
        return isMigrated(cppObj);
    }

    private native boolean isMigrated(long self);

    @Override
    boolean autoInvalidateHandle() {
        return true;
    }

    @Override
    Database getDatabase() {
        return this;
    }
}
