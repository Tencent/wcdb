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

#include <WCDB/AsyncQueue.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/String.hpp>
#include <WCDB/Time.hpp>
#include <WCDB/TimedQueue.hpp>
#include <map>
#include <set>

#include <WCDB/AutoBackupConfig.hpp>
#include <WCDB/AutoCheckpointConfig.hpp>
#include <WCDB/AutoMigrateConfig.hpp>

namespace WCDB {

class OperationEvent {
public:
    virtual ~OperationEvent() = 0;

protected:
    virtual std::pair<bool, bool> migrationShouldBeOperated(const String& path) = 0;
    virtual bool backupShouldBeOperated(const String& path) = 0;
    virtual bool checkpointShouldBeOperated(const String& path, int frames) = 0;
    virtual void integrityShouldBeChecked(const String& path) = 0;
    virtual void purgeShouldBeOperated() = 0;

    friend class OperationQueue;
};

class OperationQueue final : public AsyncQueue,
                             public AutoMigrateOperator,
                             public AutoBackupOperator,
                             public AutoCheckpointOperator {
public:
    OperationQueue(const String& name, OperationEvent* event);
    ~OperationQueue();

protected:
    void handleError(const Error& error);
    void observatedThatFileOpened(int fd, const char* path, int flags, int mode);

private:
    OperationEvent* m_event;
    mutable SharedLock m_lock;

#pragma mark - Operation
protected:
    struct Operation {
    public:
        enum class Type {
            Integrity,
            Purge,
            NotifyCorruption,
            Checkpoint,
            Backup,
            Migrate,
        };

        const Type type;
        const String path;

        Operation(Type type);
        Operation(Type type, const String& path);

        bool operator==(const Operation& other) const;
    };
    typedef struct Operation Operation;

    struct Parameter {
        Parameter();
        enum class Source {
            Other = -1,
            MemoryWarning,
            FileDescriptorsWarning,
            OutOfMaxAllowedFileDescriptors,
        } source;

        int frames;
        int numberOfFailures;
        uint32_t identifier;
        uint32_t numberOfFileDescriptors;
    };
    typedef struct Parameter Parameter;

    void loop() override final;
    void onTimed(const Operation& operation, const Parameter& parameter);

    void async(const Operation& operation, double delay, const Parameter& parameter);
    TimedQueue<Operation, Parameter> m_timedQueue;

#pragma mark - Record
protected:
    struct Record {
        Record();
        int registeredForMigration;
        int registeredForBackup;
        int registeredForCheckpoint;
    };
    typedef struct Record Record;
    std::map<String, Record> m_records;

#pragma mark - Migrate
public:
    void registerAsRequiredMigration(const String& path) override final;
    void registerAsNoMigrationRequired(const String& path) override final;

protected:
    void asyncMigrate(const String& path, double delay, int numberOfFailures);
    void doMigrate(const String& path, int numberOfFailures);

#pragma mark - Backup
public:
    void registerAsRequiredBackup(const String& path) override final;
    void registerAsNoBackupRequired(const String& path) override final;

    void asyncBackup(const String& path) override final;

protected:
    void asyncBackup(const String& path, double delay);
    void doBackup(const String& path);

#pragma mark - Checkpoint
public:
    void registerAsRequiredCheckpoint(const String& path) override final;
    void registerAsNoCheckpointRequired(const String& path) override final;

    void asyncCheckpoint(const String& path, int frames) override final;

protected:
    void asyncCheckpoint(const String& path, double delay, int frames);
    void doCheckpoint(const String& path, int frames);

#pragma mark - Purge
protected:
    void asyncPurge(const Parameter& parameter);

    void doPurge(const Parameter& parameter);

    static int maxAllowedNumberOfFileDescriptors();

    void* registerNotificationWhenMemoryWarning();
    void unregisterNotificationWhenMemoryWarning(void* observer);

    void* m_observerForMemoryWarning;
    SteadyClock m_lastPurge;

#pragma mark - Integrity
protected:
    void asyncCheckIntegrity(const String& path, uint32_t identifier);

    void doCheckIntegrity(const String& path);

    // identifier of the corrupted database file -> the times of ignored corruption
    // it will be kept forever in memory since the identifier will be changed after removed/recovered
    std::set<uint32_t> m_corrupteds;

#pragma mark - Corrupted
public:
    typedef std::function<void(const String& path, uint32_t identifier)> CorruptionNotification;
    void setNotificationWhenCorrupted(const String& path,
                                      const CorruptionNotification& notification);

    bool isFileObservedCorrupted(const String& path) const;

protected:
    void asyncNotifyCorruption(const String& path, uint32_t identifier);

    void doNotifyCorruption(const String& path, uint32_t identifier);

    std::map<String, CorruptionNotification> m_corruptionNotifications;
};

} // namespace WCDB
