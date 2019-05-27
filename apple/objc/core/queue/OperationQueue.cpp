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

#include <WCDB/Assertion.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/OperationQueue.hpp>

namespace WCDB {

OperationEvent::~OperationEvent()
{
}

OperationQueue::OperationQueue(const String& name, OperationEvent* event)
: AsyncQueue(name), m_event(event)
{
}

#pragma mark - Operation
OperationQueue::Operation::Operation(Type type_) : type(type_)
{
}

OperationQueue::Operation::Operation(Type type_, const String& path_)
: type(type_), path(path_)
{
}

bool OperationQueue::Operation::operator==(const Operation& other) const
{
    bool equal = false;
    if (type == other.type) {
        switch (type) {
        case Type::Integrity:
        case Type::Corrupted:
        case Type::Checkpoint:
        case Type::Backup:
        case Type::Migrate:
            equal = path == other.path;
            break;
        default:
            equal = true;
            break;
        }
    }
    return equal;
}

OperationQueue::Parameter::Parameter()
: source(Source::None), frames(0), numberOfFailures(0), identifier(0), numberOfFileDescriptors(0)
{
}

void OperationQueue::loop()
{
    m_timedQueue.loop(std::bind(
    &OperationQueue::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

void OperationQueue::onTimed(const Operation& operation, const Parameter& parameter)
{
    switch (operation.type) {
    case Operation::Type::Migrate:
        WCTInnerAssert(!operation.path.empty());
        WCTInnerAssert(parameter.numberOfFailures < MigrationQueueTolerableFailures);
        doMigrate(operation.path, parameter.numberOfFailures);
        break;
    case Operation::Type::Backup:
        WCTInnerAssert(!operation.path.empty());
        doBackup(operation.path, parameter.frames);
    case Operation::Type::Checkpoint:
        WCTInnerAssert(!operation.path.empty());
        doCheckpoint(operation.path, parameter.frames);
    default:
        break;
    }
}

void OperationQueue::async(const Operation& operation, double delay, const Parameter& parameter)
{
    m_timedQueue.queue(operation, delay, parameter);
    lazyRun();
}

#pragma mark - Record
OperationQueue::Record::Record()
: registeredForMigration(0)
, registeredForBackup(0)
, framesThatBackedUp(0)
, registeredForCheckpoint(0)
{
}

#pragma mark - Migrate
void OperationQueue::registerAsRequiredMigration(const String& path)
{
    LockGuard lockGuard(m_lock);
    int queue = m_records[path].registeredForMigration == 0;
    ++m_records[path].registeredForMigration;
    if (queue) {
        asyncMigrate(path, MigrationQueueTimeIntervalForMigrating, 0);
    }
}

void OperationQueue::registerAsNoMigrationRequired(const String& path)
{
    LockGuard lockGuard(m_lock);
    Record& record = m_records[path];
    --record.registeredForMigration;
    WCTInnerAssert(record.registeredForMigration >= 0);
    if (record.registeredForMigration == 0) {
        Operation operation(Operation::Type::Migrate, path);
        m_timedQueue.remove(operation);
    }
}

void OperationQueue::asyncMigrate(const String& path, double delay, int numberOfFailures)
{
    SharedLockGuard lockGuard(m_lock);
    if (m_records[path].registeredForMigration > 0) {
        Operation operation(Operation::Type::Migrate, path);
        Parameter parameter;
        parameter.numberOfFailures = numberOfFailures;
        async(Operation(Operation::Type::Migrate), delay, parameter);
    }
}

void OperationQueue::doMigrate(const String& path, int numberOfFailures)
{
    bool succeed, done;
    std::tie(succeed, done) = m_event->migrationShouldBeOperated(path);
    if (succeed) {
        if (!done) {
            asyncMigrate(path, MigrationQueueTimeIntervalForMigrating, numberOfFailures);
        }
    } else {
        if (numberOfFailures + 1 < MigrationQueueTolerableFailures) {
            asyncMigrate(path,
                         MigrationQueueTimeIntervalForRetryingAfterFailure,
                         numberOfFailures + 1);
        } else {
            Error error(Error::Code::Notice, Error::Level::Notice, "Async migration stopped due to the error.");
            error.infos.set(ErrorStringKeyPath, path);
            Notifier::shared().notify(error);
        }
    }
}

#pragma mark - Backup
void OperationQueue::registerAsRequiredBackup(const String& path)
{
    LockGuard lockGuard(m_lock);
    ++m_records[path].registeredForBackup;
}

void OperationQueue::registerAsNoBackupRequired(const String& path)
{
    LockGuard lockGuard(m_lock);
    Record& record = m_records[path];
    --record.registeredForBackup;
    WCTInnerAssert(record.registeredForBackup >= 0);
    if (record.registeredForBackup == 0) {
        record.framesThatBackedUp = 0;
        Operation operation(Operation::Type::Backup, path);
        m_timedQueue.remove(operation);
    }
}

void OperationQueue::asyncBackup(const String& path, int frames)
{
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_records.find(path);
    if (iter != m_records.end() && iter->second.registeredForBackup > 0) {
        int recordedFrames = iter->second.framesThatBackedUp;
        if (frames >= recordedFrames + BackupConfigFramesIntervalForCritical // expired too much
            || frames < recordedFrames // restarted
        ) {
            asyncBackup(path, BackupQueueTimeIntervalForCritical, frames);
        } else if (frames >= recordedFrames + BackupConfigFramesIntervalForNonCritical) {
            asyncBackup(path, BackupQueueTimeIntervalForNonCritical, frames);
        }
    }
}

void OperationQueue::asyncBackup(const String& path, double delay, int frames)
{
    WCTInnerAssert(delay > 0);
    Operation operation(Operation::Type::Backup, path);
    Parameter parameter;
    parameter.frames = frames;
    async(operation, delay, parameter);
}

void OperationQueue::doBackup(const String& path, int frames)
{
    bool result = m_event->backupShouldBeOperated(path);
    if (result) {
        LockGuard lockGuard(m_lock);
        auto iter = m_records.find(path);
        if (iter != m_records.end()) {
            if (iter->second.registeredForBackup > 0) {
                iter->second.framesThatBackedUp = frames;
            }
        }
    } else {
        asyncBackup(path, BackupQueueTimeIntervalForRetryingAfterFailure, frames);
    }
}

#pragma mark - Checkpoint
void OperationQueue::registerAsRequiredCheckpoint(const String& path)
{
    LockGuard lockGuard(m_lock);
    ++m_records[path].registeredForCheckpoint;
}

void OperationQueue::registerAsNoCheckpointRequired(const String& path)
{
    LockGuard lockGuard(m_lock);
    Record& record = m_records[path];
    --record.registeredForCheckpoint;
    WCTInnerAssert(record.registeredForCheckpoint >= 0);
    if (record.registeredForCheckpoint == 0) {
        Operation operation(Operation::Type::Checkpoint, path);
        m_timedQueue.remove(operation);
    }
}

void OperationQueue::asyncCheckpoint(const String& path, int frames)
{
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_records.find(path);
    if (iter != m_records.end() && iter->second.registeredForBackup > 0) {
        if (frames >= CheckpointQueueFramesThresholdForCritical) {
            asyncCheckpoint(path, CheckpointQueueTimeIntervalForCritical, frames);
        } else {
            asyncCheckpoint(path, CheckpointQueueTimeIntervalForNonCritical, frames);
        }
    }
}

void OperationQueue::asyncCheckpoint(const String& path, double delay, int frames)
{
    Operation operation(Operation::Type::Checkpoint, path);
    Parameter parameter;
    parameter.frames = frames;
    async(operation, delay, parameter);
}

void OperationQueue::doCheckpoint(const String& path, int frames)
{
    if (!m_event->checkpointShouldBeOperated(path, frames)) {
        // delay retry if failed
        asyncCheckpoint(path, CheckpointQueueTimeIntervalForRetryingAfterFailure, frames);
    }
}

} // namespace WCDB
