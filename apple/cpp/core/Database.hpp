//
// Created by qiuwenchen on 2022/8/3.
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

#pragma once
#include <WCDB/AuxiliaryFunctionModule.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/FTSTokenizerUtil.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/HandleORMOperation.hpp>
#include <WCDB/Statement.hpp>
#include <WCDB/TokenizerModule.hpp>

namespace WCDB {

class Database final : public HandleORMOperation {
public:
    Database(const UnsafeStringView &path);
    Database() = delete;
    Database(const Database &);
    Database &operator=(const Database &);
    ~Database() override final;

private:
    Database(InnerDatabase *database);
    RecyclableHandle getHandleHolder() override final;
    Recyclable<InnerDatabase *> getDatabaseHolder() override final;
    Recyclable<InnerDatabase *> m_databaseHolder;
    InnerDatabase *m_innerDatabase;

#pragma mark - Basic
public:
    void setTag(const long &tag);
    long getTag() const;
    const StringView &getPath() const;
    const Error &getError() const;

    Handle getHandle();

    bool canOpen() const;
    bool isOpened() const;

    typedef std::function<void(void)> ClosedCallback;
    void close(const ClosedCallback &onClosed = nullptr);

    void blockade();
    bool isBlockaded() const;
    void unblockade();

    bool truncateCheckpoint();
    bool passiveCheckpoint();

#pragma mark - Monitor
public:
    /**
     @brief You can register a reporter to monitor all errors.
     */
    typedef std::function<void(const Error &)> ErrorNotification;
    static void globalTraceError(ErrorNotification trace);
    void traceError(ErrorNotification trace);

    /**
     @brief You can register a tracer to monitor the performance of all SQLs.
     It returns
     1. Every SQL executed by the database.
     2. Time consuming in seconds.
     3. Path of database.
     4. The id of the handle executing this SQL.
     Note that:
     1. You should register trace before all db operations.
     2. Global tracer and db tracer do not interfere with each other.
     
     @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
     */
    typedef std::function<void(const UnsafeStringView &path, const UnsafeStringView &sql, double cost, const void *handle)> PerformanceNotification;
    static void globalTracePerformance(PerformanceNotification trace);
    void tracePerformance(PerformanceNotification trace);

    /**
     @brief You can register a tracer to monitor the execution of all SQLs.
     It returns a prepared or executed SQL.
     Note that you should register trace before all db operations.
     
     @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
     */
    typedef std::function<void(const UnsafeStringView &path, const UnsafeStringView &sql, const void *handle)> SQLNotification;
    static void globalTraceSQL(SQLNotification trace);
    void traceSQL(SQLNotification trace);

    enum Operation : short {
        Create = 0,
        SetTag,
        OpenHandle,
    };
    typedef std::function<void(Database &database, Operation operation)> DBOperationTrace;
    static void globalTraceDatabaseOperation(DBOperationTrace callback);

#pragma mark - File
public:
    /**
     @brief Remove all database-related files.
     @warning You should call it on a closed database. Otherwise you will get a warning.
     @return YES if all files are removed.
     */
    bool removeFiles();

    /**
     @brief Move files of this database to other directory.
     @param directory destination
     @return true if all files are moved.
     */
    bool moveFiles(const UnsafeStringView &directory);

    /**
     @brief Paths to all database-related files.
     @return paths
     */
    std::list<StringView> getPaths() const;

    /**
     @brief Get the space used by the database files.
     @warning You should call it on a closed database. Otherwise you will get a warning.
     @return The sum of files size in bytes.
     */
    std::optional<size_t> getFilesSize() const;

    /**
    @brief Set the default directory for temporary database files. If not set, an existing directory will be selected as the temporary database files directory in the following order:
        1. TMPDIR environment value;
        2. /Var/tmp;
        3. /usr/tmp;
        4. /tmp;
        5. the current directory;
    */

#pragma mark - FTS
public:
    void enableAutoMergeFTS5Index(bool enable);

    void addTokenizer(const UnsafeStringView &tokenize);

    static void
    registerTokenizer(const UnsafeStringView &name, const TokenizerModule &module);

    void addAuxiliaryFunction(const UnsafeStringView &functionName);

    static void registerAuxiliaryFunction(const UnsafeStringView &name,
                                          const FTS5AuxiliaryFunctionModule &module);

    typedef BaseTokenizerUtil::UnicodeChar UnicodeChar;
    typedef BaseTokenizerUtil::SymbolDetector SymbolDetector;
    static void configSymbolDetector(SymbolDetector detector);

    typedef BaseTokenizerUtil::UnicodeNormalizer UnicodeNormalizer;
    static void configUnicodeNormalizer(UnicodeNormalizer normalizer);

    typedef BaseTokenizerUtil::PinYinConverter PinYinConverter;
    static void configPinyinConverter(PinYinConverter converter);

    typedef BaseTokenizerUtil::TraditionalChineseConverter TraditionalChineseConverter;
    static void configTraditionalChineseConverter(TraditionalChineseConverter converter);

#pragma mark - Memory
    /**
     @brief Purge all free memory of this database.
     WCDB will cache and reuse some sqlite handles to improve performance.
     The max count of free sqlite handles is same as the number of concurrent threads supported by the hardware implementation.
     You can call it to save some memory.
     */
    void purge();

    /**
     @brief Purge all free memory of all databases.
     Note that WCDB will call this interface automatically while it receives memory warning on iOS.
     @see purge
     */
    static void purgeAll();

#pragma mark - Repair
    typedef std::function<void(Database &)> CorruptionNotification;
    void setNotificationWhenCorrupted(CorruptionNotification onCorrupted);

    bool checkIfCorrupted();
    bool isAlreadyCorrupted();

    void enableAutoBackup(bool enable);
    bool backup();
    typedef std::function<bool(const UnsafeStringView &table)> BackupFilter;
    void filterBackup(BackupFilter tableShouldBeBackedUp);

    bool deposit();
    bool removeDeposited();
    bool containsDeposited() const;

    typedef std::function<void(double progress, double increment)> ProgressUpdateCallback;
    double retrieve(ProgressUpdateCallback onProgressUpdated);

#pragma mark - Config
    /**
     @brief Set cipher key for a database.
     For an encrypted database, you must call it before all other operation.
     The cipher page size defaults to 4096 in WCDB, but it defaults to 1024 in other databases. So for an existing database created by other database framework, you should set it to 1024. Otherwise, you'd better to use cipher page size with 4096 or simply call setCipherKey: interface to get better performance.
     @param cipherKey Cipher key.
     @param cipherPageSize Cipher Page Size
     */
    void setCipherKey(const UnsafeData &cipherKey, int cipherPageSize = 4096);

    /**
     @brief Set config for this database.
     @warning Since WCDB is a multi-handle database, an executing handle will not apply this config immediately. Instead, all handles will run this config before its next operation.
     */
    using Invocation = std::function<bool(Handle &)>;
    enum Priority : int {
        High = -100,
        Default = 0,
        Low = 100,
    };
    void setConfig(const UnsafeStringView &name,
                   Invocation invocation,
                   Invocation unInvocation = nullptr,
                   Priority priority = Priority::Default);

    void removeConfig(const UnsafeStringView &name);

#pragma mark - Migration
    typedef struct MigrationInfo {
        StringView database;
        StringView table;
        StringView sourceDatabase;
        StringView sourceTable;
    } MigrationInfo;
    typedef std::function<void(MigrationInfo &)> MigrationFilter;
    void filterMigration(MigrationFilter filter);
    bool stepMigration();
    void enableAutoMigration(bool enable);

    typedef std::function<void(Database &database, const MigrationInfo &info)> MigratedCallback;
    void setNotificationWhenMigrated(MigratedCallback onMigrated);
    bool isMigrated() const;
#pragma mark - version
    static const StringView getVersion();
    static const StringView getSourceId();
};

} //namespace WCDB
