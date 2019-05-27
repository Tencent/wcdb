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
#include <WCDB/TimedQueue.hpp>

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

    friend class OperationQueue;
};

class OperationQueue final : public AsyncQueue,
                             public AutoMigrateOperator,
                             public AutoBackupOperator,
                             public AutoCheckpointOperator {
public:
    OperationQueue(const String& name, OperationEvent* event);

private:
    OperationEvent* m_event;
    SharedLock m_lock;

#pragma mark - Operation
protected:
    struct Operation {
    public:
        enum class Type {
            Integrity,
            Purge,
            Corrupted,
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
            None,
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
        int framesThatBackedUp;

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

    void asyncBackup(const String& path, int frames) override final;

protected:
    void asyncBackup(const String& path, double delay, int frames);
    void doBackup(const String& path, int frames);

#pragma mark - Checkpoint
public:
    void registerAsRequiredCheckpoint(const String& path) override final;
    void registerAsNoCheckpointRequired(const String& path) override final;

    void asyncCheckpoint(const String& path, int frames) override final;

protected:
    void asyncCheckpoint(const String& path, double delay, int frames);
    void doCheckpoint(const String& path, int frames);
};

} // namespace WCDB
