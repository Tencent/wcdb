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
#include <WCDB/StringView.hpp>
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
    virtual std::pair<bool, bool>
    migrationShouldBeOperated(const UnsafeStringView& path) = 0;
    virtual bool backupShouldBeOperated(const UnsafeStringView& path) = 0;
    virtual bool checkpointShouldBeOperated(const UnsafeStringView& path) = 0;
    virtual void integrityShouldBeChecked(const UnsafeStringView& path) = 0;
    virtual void purgeShouldBeOperated() = 0;

    friend class OperationQueue;
};

class OperationQueue final : public AsyncQueue,
                             public AutoMigrateOperator,
                             public AutoBackupOperator,
                             public AutoCheckpointOperator {
public:
    OperationQueue(const UnsafeStringView& name, OperationEvent* event);
    ~OperationQueue();

protected:
    void main() override final;
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
        const StringView path;

        Operation(Type type);
        Operation(Type type, const UnsafeStringView& path);

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

    void onTimed(const Operation& operation, const Parameter& parameter);

    typedef TimedQueue<Operation, Parameter>::Mode AsyncMode;
    void async(const Operation& operation,
               double delay,
               const Parameter& parameter,
               AsyncMode mode = AsyncMode::ForwardOnly);
    TimedQueue<Operation, Parameter> m_timedQueue;

#pragma mark - Record
protected:
    struct Record {
        Record();
        bool registeredForMigration;
        bool registeredForBackup;
        bool registeredForCheckpoint;
    };
    typedef struct Record Record;
    StringViewMap<Record> m_records;

#pragma mark - Migrate
public:
    void registerAsRequiredMigration(const UnsafeStringView& path);
    void registerAsNoMigrationRequired(const UnsafeStringView& path);
    void asyncMigrate(const UnsafeStringView& path) override final;
    void stopMigrate(const UnsafeStringView& path) override final;

protected:
    void asyncMigrate(const UnsafeStringView& path, double delay, int numberOfFailures);
    void doMigrate(const UnsafeStringView& path, int numberOfFailures);

#pragma mark - Backup
public:
    void registerAsRequiredBackup(const UnsafeStringView& path);
    void registerAsNoBackupRequired(const UnsafeStringView& path);

    void asyncBackup(const UnsafeStringView& path) override final;

protected:
    void asyncBackup(const UnsafeStringView& path, double delay);
    void doBackup(const UnsafeStringView& path);

#pragma mark - Checkpoint
public:
    void registerAsRequiredCheckpoint(const UnsafeStringView& path);
    void registerAsNoCheckpointRequired(const UnsafeStringView& path);

    void asyncCheckpoint(const UnsafeStringView& path) override final;

protected:
    void doCheckpoint(const UnsafeStringView& path);

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
    void asyncCheckIntegrity(const UnsafeStringView& path, uint32_t identifier);

    void doCheckIntegrity(const UnsafeStringView& path);

    // identifier of the corrupted database file -> the times of ignored corruption
    // it will be kept forever in memory since the identifier will be changed after removed/recovered
    std::set<uint32_t> m_corrupteds;

#pragma mark - Corrupted
public:
    typedef std::function<void(const UnsafeStringView& path, uint32_t identifier)> CorruptionNotification;
    void setNotificationWhenCorrupted(const UnsafeStringView& path,
                                      const CorruptionNotification& notification);

    bool isFileObservedCorrupted(const UnsafeStringView& path) const;

protected:
    void asyncNotifyCorruption(const UnsafeStringView& path, uint32_t identifier);

    void doNotifyCorruption(const UnsafeStringView& path, uint32_t identifier);

    StringViewMap<CorruptionNotification> m_corruptionNotifications;
};

} // namespace WCDB
