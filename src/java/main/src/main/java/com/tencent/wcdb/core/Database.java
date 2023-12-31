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

import com.tencent.wcdb.base.CppObject;
import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.orm.Field;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.Expression;

import java.security.PrivilegedExceptionAction;
import java.util.Dictionary;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Database extends HandleORMOperation {
    private Database() {
    }

    public Database(String path) {
        cppObj = createDatabase(path);
    }

    private static native long createDatabase(String path);

    public String getPath() {
        return getPath(cppObj);
    }

    private static native String getPath(long self);

    public List<String> getPaths() {
        return getPaths(cppObj);
    }

    private static native List<String> getPaths(long self);

    public void setTag(long tag) {
        setTag(cppObj, tag);
    }

    private static native void setTag(long self, long tag);

    public long getTag() {
        return getTag(cppObj);
    }

    private static native long getTag(long self);

    public boolean canOpen() {
        return canOpen(cppObj);
    }

    private static native boolean canOpen(long self);

    public boolean isOpened() {
        return isOpened(cppObj);
    }

    private static native boolean isOpened(long self);

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

    private static native void close(long self, CloseCallBack callBack);

    public void blockade() {
        blockade(cppObj);
    }

    private static native void blockade(long self);

    public void unblockade() {
        unblockade(cppObj);
    }

    private static native void unblockade(long self);

    public boolean isBlockaded() {
        return isBlockaded(cppObj);
    }

    private static native boolean isBlockaded(long self);

    public void purge() {
        purge(cppObj);
    }

    private static native void purge(long self);

    public static native void purgeAll();

    public int getNumberOfAliveHandle() {
        return getNumberOfAliveHandle(cppObj);
    }

    private static native int getNumberOfAliveHandle(long self);

    public static native void releaseSQLiteMemory(int bytes);

    public static native void setSoftHeapLimit(long limit);

    WCDBException createException() {
        return WCDBException.createException(getError(cppObj));
    }
    static WCDBException createThreadedException() {
        return WCDBException.createException(getThreadedError());
    }

    private static native long getError(long self);
    private static native long getThreadedError();

    public Handle getHandle() {
        return new Handle(this, false);
    }

    @Override
    public Handle getHandle(boolean writeHint) {
        return new Handle(this, writeHint);
    }

    static native long getHandle(long self, boolean writeHint);

    public void removeFiles() throws WCDBException {
        if(!removeFiles(cppObj)) {
            throw createException();
        }
    }

    private static native boolean removeFiles(long self);

    public void moveFile(String destination) throws WCDBException{
        if(!moveFile(cppObj, destination)) {
            throw createException();
        }
    }

    private static native boolean moveFile(long self, String destination);

    public long getFileSize() throws WCDBException {
        long ret = getFileSize(cppObj);
        if(ret < 0) {
            throw  createException();
        }
        return ret;
    }

    private static native long getFileSize(long self);

    public enum CipherVersion {
        defaultVersion, version1, version2, version3, version4
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

    private static native void setCipherKey(long self, byte[] key, int pageSize, int version);

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
        low, default_, high, highest
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
            case high:
                cppPriority = -100;
                break;
            case highest:
                cppPriority = -2147483648;
                break;
        }
        setConfig(cppObj, configName, invocation, unInvocation, cppPriority);
    }

    private static native void setConfig(long self, String configName, Config invocation, Config unInvocation, int priority);

    public static class PerformanceInfo {
        public int tablePageReadCount;
        public int tablePageWriteCount;
        public int indexPageReadCount;
        public int indexPageWriteCount;
        public int overflowPageReadCount;
        public int overflowPageWriteCount;
        public long costInNanoseconds;
    }

    public interface PerformanceTracer {
        void onTrace(long tag, String path, long handleId, String sql, PerformanceInfo info);
    }

    private static void onTracePerformance(PerformanceTracer tracer, long tag, String path, long handleId, String sql, long costInNanoseconds, int[] infoValues) {
        PerformanceInfo info = null;
        if(infoValues != null && infoValues.length == 6){
            info = new PerformanceInfo();
            info.tablePageReadCount = infoValues[0];
            info.tablePageWriteCount = infoValues[1];
            info.indexPageReadCount = infoValues[2];
            info.indexPageWriteCount = infoValues[3];
            info.overflowPageReadCount = infoValues[4];
            info.overflowPageWriteCount = infoValues[5];
            info.costInNanoseconds = costInNanoseconds;
        }
        tracer.onTrace(tag, path, handleId, sql, info);
    }

    public static native void globalTracePerformance(PerformanceTracer tracer);

    public void tracePerformance(PerformanceTracer tracer) {
        tracePerformance(cppObj, tracer);
    }
    private static native void tracePerformance(long self, PerformanceTracer tracer);

    public interface SQLTracer {
        void onTrace(long tag, String path, long handleId, String sql, String info);
    }

    private static void onTraceSQL(SQLTracer tracer, long tag,  String path, long handleId, String sql, String info) {
        tracer.onTrace(tag, path, handleId, sql, info);
    }

    public static native void globalTraceSQL(SQLTracer tracer);

    public void traceSQL(SQLTracer tracer) {
        traceSQL(cppObj, tracer);
    }

    private static native void traceSQL(long self, SQLTracer tracer);

    public void setFullSQLTraceEnable(boolean enable) {
        setFullSQLTraceEnable(cppObj, enable);
    }

    private static native void setFullSQLTraceEnable(long self, boolean enable);

    public interface ExceptionTracer {
        void onTrace(WCDBException exception);
    }

    private static void onTraceException(ExceptionTracer tracer, long cppError) {
        tracer.onTrace(WCDBException.createException(cppError));
    }

    public static native void globalTraceException(ExceptionTracer tracer);

    public void traceException(ExceptionTracer tracer) {
        traceException(cppObj, tracer);
    }

    private static native void traceException(long self, ExceptionTracer tracer);

    public enum Operation {
        Create, SetTag, OpenHandle
    }

    public interface OperationTracer {
        void onTrace(Database database, Operation operation, HashMap<String, Value> info);
    }

    public static String OperationInfoKeyHandleCount = "HandleCount";
    public static String OperationInfoKeyOpenTime = "OpenTime";
    public static String OperationInfoKeyOpenCPUTime = "OpenCPUTime";
    public static String OperationInfoKeySchemaUsage = "SchemaUsage";
    public static String OperationInfoKeyTableCount = "TableCount";
    public static String OperationInfoKeyIndexCount = "IndexCount";
    public static String OperationInfoKeyTriggerCount = "TriggerCount";

    private static void onTraceOperation(OperationTracer tracer, long cppDatabase, int cppOperation, long info) {
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
        HashMap<String, Value> javaInfo = new HashMap();
        enumerateInfo(javaInfo, info);
        tracer.onTrace(database, operation, javaInfo);
    }

    private static native void enumerateInfo(HashMap<String, Value> javaInfo, long cppInfo);
    private static void onEnumerateInfo(HashMap<String, Value> javaInfo,
                                        String key,
                                        int type,
                                        long intValue,
                                        double doubleValue,
                                        String stringValue) {
        if(type == 3) {
            javaInfo.put(key, new Value(intValue));
        } else if (type == 5) {
            javaInfo.put(key, new Value(doubleValue));
        } else if (type == 6)  {
            javaInfo.put(key, new Value(stringValue));
        }
    }

    public static native void globalTraceDatabaseOperation(OperationTracer tracer);

    public interface BusyTracer {
        void onTrace(long tag, String path, long tid, String sql);
    }

    public static native void globalTraceBusy(BusyTracer tracer, double timeOut);

    private static void onBusyTrace(BusyTracer tracer, long tag, String path, long tid, String sql) {
        tracer.onTrace(tag, path, tid, sql);
    }

    public void addTokenizer(String tokenizer) {
        addTokenizer(cppObj, tokenizer);
    }

    private static native void addTokenizer(long self, String tokenizer);

    public void addAuxiliaryFunction(String auxiliaryFunction) {
        addAuxiliaryFunction(cppObj, auxiliaryFunction);
    }

    private static native void addAuxiliaryFunction(long self, String auxiliaryFunction);

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

    private static native void setNotificationWhenCorrupted(long self, CorruptionNotification monitor);

    public boolean checkIfCorrupted() {
        return checkIfCorrupted(cppObj);
    }

    private static native boolean checkIfCorrupted(long self);

    public boolean checkIfIsAlreadyCorrupted() {
        return checkIfIsAlreadyCorrupted(cppObj);
    }

    private static native boolean checkIfIsAlreadyCorrupted(long self);

    public void enableAutoBackup(boolean enable) {
        enableAutoBackup(cppObj, enable);
    }

    private static native void enableAutoBackup(long self, boolean enable);

    public void backup() throws WCDBException {
        if(!backup(cppObj)) {
            throw createException();
        }
    }

    private static native boolean backup(long self);

    public interface BackupFilter {
        boolean tableShouldBeBackup(String tableName);
    }

    private static boolean checkTableShouldBeBackup(BackupFilter filter, String tableName) {
        return filter.tableShouldBeBackup(tableName);
    }

    public void filterBackup(BackupFilter filter) {
        filterBackup(cppObj, filter);
    }

    private static native void filterBackup(long self, BackupFilter filter);

    public interface ProgressMonitor {
        boolean onProgressUpdate(double percentage, double increment);
    }

    private static boolean onProgressUpdate(ProgressMonitor progress, double percentage, double increment) {
        return progress.onProgressUpdate(percentage, increment);
    }

    public double retrieve(ProgressMonitor monitor) throws WCDBException {
        double score = retrieve(cppObj, monitor);
        if(score < 0) {
            throw createException();
        }
        return score;
    }

    private static native double retrieve(long self, ProgressMonitor monitor);

    public void vacuum(ProgressMonitor monitor) throws WCDBException {
        if(!vacuum(cppObj, monitor)) {
            throw createException();
        }
    }

    private static native boolean vacuum(long self, ProgressMonitor monitor);

    public void deposit() throws WCDBException {
        if(!deposit(cppObj)) {
            throw createException();
        }
    }

    private static native boolean deposit(long self);

    public void removeDepositedFiles() throws WCDBException {
        if(!removeDepositedFiles(cppObj)) {
            throw createException();
        }
    }

    private static native boolean removeDepositedFiles(long self);

    public boolean containDepositedFiles() {
        return containDepositedFiles(cppObj);
    }

    private static native boolean containDepositedFiles(long self);

    public void passiveCheckpoint() throws WCDBException {
        if(!passiveCheckpoint(cppObj)) {
            throw createException();
        }
    }

    private static native boolean passiveCheckpoint(long self);

    public void truncateCheckpoint() throws WCDBException {
        if(!truncateCheckpoint(cppObj)) {
            throw createException();
        }
    }

    public void setAutoCheckpointEnable(boolean enable) {
        setAutoCheckpointEnable(cppObj, enable);
    }

    private static native void setAutoCheckpointEnable(long self, boolean enable);

    private static native boolean truncateCheckpoint(long self);

    public static class MigrationInfo {
        public String table;
        public String sourceTable;
        public Expression filterCondition;
    }

    public static native void setAutoCheckpointMinFrames(int frames);

    public interface MigrationFilter {
        void filterMigrate(MigrationInfo info);
    }

    private static void filterMigrate(MigrationFilter filter, long cppInfoSetter, long cppInfo, String table) {
        MigrationInfo info = new MigrationInfo();
        info.table = table;
        filter.filterMigrate(info);
        setMigrationInfo(cppInfoSetter, cppInfo, info.sourceTable, CppObject.get(info.filterCondition));
    }

    private static native void setMigrationInfo(long cppInfoSetter, long cppInfo, String sourceTable, long filterCondition);

    public void addMigrationSource(String sourcePath, MigrationFilter filter) {
        addMigrationSource(sourcePath, null, filter);
    }

    public void addMigrationSource(String sourcePath, byte[] sourceCipher, MigrationFilter filter) {
        addMigrationSource(cppObj, sourcePath, sourceCipher, filter);
    }

    private static native void addMigrationSource(long self, String sourcePath, byte[] sourceCipher, MigrationFilter filter);

    public void stepMigration() throws WCDBException {
        if(!stepMigration(cppObj)) {
            throw createException();
        }
    }

    private static native boolean stepMigration(long self);

    public void enableAutoMigration(boolean enable) {
        enableAutoMigration(cppObj, enable);
    }

    private static native void enableAutoMigration(long self, boolean enable);

    public interface MigrationNotification {
        void onMigrated(Database database, MigrationInfo info);
    }

    private static void onTableMigrated(MigrationNotification notification, long cppDatabase, String table, String sourceTable) {
        Database database = new Database();
        database.cppObj = cppDatabase;
        MigrationInfo info = null;
        if(table != null && table.length() > 0) {
            info = new MigrationInfo();
            info.table = table;
            info.sourceTable = sourceTable;
        }
        notification.onMigrated(database, info);
    }

    public void setNotificationWhenMigrated(MigrationNotification notification) {
        setNotificationWhenMigrated(cppObj, notification);
    }

    private static native void setNotificationWhenMigrated(long self, MigrationNotification notification);

    public boolean isMigrated() {
        return isMigrated(cppObj);
    }

    private static native boolean isMigrated(long self);

    public static byte[] trainDictWithString(List<String> samples, byte dictId) throws WCDBException {
        byte[] dict = trainDict(samples.toArray(new String[0]), dictId);
        if(dict == null || dict.length == 0) {
            throw createThreadedException();
        }
        return dict;
    }

    private static native byte[] trainDict(String[] samples, byte dictId);

    public static byte[] trainDictWithData(List<byte[]> samples, byte dictId) throws WCDBException {
        byte[] dict = trainDict(samples.toArray(new byte[0][]), dictId);
        if(dict == null || dict.length == 0) {
            throw createThreadedException();
        }
        return dict;
    }

    private static native byte[] trainDict(byte[][] samples, byte dictId);

    public static void registerDict(byte[] dict, byte dictId) throws WCDBException {
        if(!nativeRegisterDict(dict, dictId)){
            throw createThreadedException();
        }
    }

    private static native boolean nativeRegisterDict(byte[] dict, byte dictId);

    public static final long DictDefaultMatchValue = Long.MAX_VALUE;
    public static class CompressionInfo {
        public String table;

        public <T> void addZSTDNormalCompress(Field<T> field) {
            Database.addZSTDNormalCompress(cppInfo, CppObject.get(field));
        }

        public <T> void addZSTDDictCompress(Field<T> field, byte dictId) {
            Database.addZSTDDictCompress(cppInfo, CppObject.get(field), dictId);
        }

        public <T> void addZSTDMultiDictCompress(Field<T> field, Field<T> matchField, Map<Long, Byte> dicts) {
            long[] values = new long[dicts.size()];
            byte[] dictIds = new byte[dicts.size()];
            int index = 0;
            for(Map.Entry<Long, Byte> entry : dicts.entrySet()) {
                values[index] = entry.getKey();
                dictIds[index] = entry.getValue();
                index++;
            }
            Database.addZSTDMultiDictCompress(cppInfo, CppObject.get(field), CppObject.get(matchField), values, dictIds);
        }

        CompressionInfo(long cppInfo, String table) {
            this.cppInfo = cppInfo;
            this.table = table;
        }
        private long cppInfo;
    }

    private static native void addZSTDNormalCompress(long cppInfo, long cppColumn);
    private static native void addZSTDDictCompress(long cppInfo, long cppColumn, byte dictId);
    private static native void addZSTDMultiDictCompress(long cppInfo, long cppColumn, long cppMatchColumn, long[] values, byte[] dictIds);

    public interface CompressionFilter {
        void filterCompress(CompressionInfo info);
    }

    private static void filterCompress(CompressionFilter filter, long cppInfo, String table) {
        CompressionInfo info = new CompressionInfo(cppInfo, table);
        filter.filterCompress(info);
    }

    public void setCompression(CompressionFilter filter) {
        setCompression(cppObj, filter);
    }

    private static native void setCompression(long self, CompressionFilter filter);

    public void disableCompressNewData(boolean disable) {
        disableCompressNewData(cppObj, disable);
    }

    private static native void disableCompressNewData(long self, boolean disable);

    public void stepCompression() throws WCDBException {
        if(!stepCompression(cppObj)){
            throw createException();
        }
    }

    private static native boolean stepCompression(long self);

    public void enableAutoCompression(boolean enable) {
        enableAutoCompression(cppObj, enable);
    }

    private static native void enableAutoCompression(long self, boolean enable);

    public interface CompressionNotification {
        void onCompressed(Database database, String table);
    }

    private static void onTableCompressed(CompressionNotification notification, long cppDatabase, String table) {
        Database database = new Database();
        database.cppObj = cppDatabase;
        notification.onCompressed(database, table);
    }

    public void setNotificationWhenCompressed(CompressionNotification notification) {
        setNotificationWhenCompressed(cppObj, notification);
    }

    private static native void setNotificationWhenCompressed(long self, CompressionNotification notification);

    public boolean isCompressed() {
        return isCompressed(cppObj);
    }

    private static native boolean isCompressed(long self);

    @Override
    boolean autoInvalidateHandle() {
        return true;
    }

    @Override
    Database getDatabase() {
        return this;
    }
}
