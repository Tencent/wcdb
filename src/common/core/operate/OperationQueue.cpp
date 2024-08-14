//
// Created by sanhuazhang on 2019/06/06
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

#include "OperationQueue.hpp"
#include "Assertion.hpp"
#include "CommonCore.hpp"
#include "CoreConst.h"
#include "CrossPlatform.h"
#include "FileManager.hpp"
#include "Global.hpp"
#include "Notifier.hpp"
#include <fcntl.h>

namespace WCDB {

OperationEvent::~OperationEvent() = default;

OperationQueue::OperationQueue(const UnsafeStringView& name, OperationEvent* event)
: AsyncQueue(name)
, m_event(event)
, m_observerForMemoryWarning(registerNotificationWhenMemoryWarning())
{
    Notifier::shared().setNotification(
    0, name, std::bind(&OperationQueue::handleError, this, std::placeholders::_1));
#ifndef _WIN32
    Global::shared().setNotificationWhenFileOpened(
    name,
    std::bind(&OperationQueue::observatedThatFileOpened,
              this,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3,
              std::placeholders::_4));
#endif
}

OperationQueue::~OperationQueue()
{
#ifndef _WIN32
    Global::shared().setNotificationWhenFileOpened(name, nullptr);
#endif
    Notifier::shared().unsetNotification(name);

    unregisterNotificationWhenMemoryWarning(m_observerForMemoryWarning);
}

void OperationQueue::stopAllDatabaseEvent(const UnsafeStringView& path)
{
    LockGuard lockGuard(m_lock);
    Operation integerity(Operation::Type::Integrity, path);
    m_timedQueue.remove(integerity);

    Operation checkpoint(Operation::Type::Checkpoint, path);
    m_timedQueue.remove(checkpoint);

    Operation backup(Operation::Type::Backup, path);
    m_timedQueue.remove(backup);

    Operation migrate(Operation::Type::Migrate, path);
    m_timedQueue.remove(migrate);

    Operation compress(Operation::Type::Compress, path);
    m_timedQueue.remove(compress);

    Operation mergeIndex(Operation::Type::MergeIndex, path);
    m_timedQueue.remove(mergeIndex);
}

void OperationQueue::stop()
{
    m_timedQueue.stop();
}

void OperationQueue::main()
{
    m_timedQueue.loop(std::bind(
    &OperationQueue::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

void OperationQueue::handleError(const Error& error)
{
    if (error.level < Error::Level::Warning || !error.isCorruption() || isExiting()) {
        return;
    }

    const auto& infos = error.infos;

    auto iter = infos.find(UnsafeStringView(ErrorStringKeyPath));
    if (iter == infos.end() || iter->second.getType() != Value::Type::Text) {
        // make sure no empty path will be added into queue
        return;
    }

    const UnsafeStringView& path = iter->second.textValue();
    if (path.empty()) {
        return;
    }

    if (path.compare(m_skipIntegrityCheckPath.getOrCreate()) == 0) {
        return;
    }

    auto optionalIdentifier = FileManager::getFileIdentifier(path);
    if (!optionalIdentifier.succeed()) {
        return;
    }
    uint32_t identifier = optionalIdentifier.value();

    bool fromIntegrity = false;
    auto actionIter = infos.find(UnsafeStringView(ErrorStringKeyType));
    if (actionIter != infos.end() && actionIter->second.textValue() == ErrorTypeIntegrity) {
        fromIntegrity = true;
    }

    if (fromIntegrity) {
        LockGuard lockGuard(m_lock);
        bool emplaced = m_corrupteds.emplace(identifier).second;
        if (emplaced
            && m_corruptionNotifications.find(path) != m_corruptionNotifications.end()) {
            asyncNotifyCorruption(path, identifier);
        }
    } else {
        // check integrity to
        // 1. find out the real corrupted database for multi-schemas
        // 2. avoid wrongly report by backup
        asyncCheckIntegrity(path, identifier);
    }
}
#ifndef _WIN32
void OperationQueue::observatedThatFileOpened(int fd, const char* path, int flags, int mode)
{
    WCDB_UNUSED(mode);

    if (fd != -1) {
        if ((flags & O_CREAT) != 0) {
            FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(path);
        }

        int possibleNumberOfActiveFileDescriptors = fd + 1;
        if (possibleNumberOfActiveFileDescriptors
            > maxAllowedNumberOfFileDescriptors() * OperationQueueRateForTooManyFileDescriptors) {
            Parameter parameter;
            parameter.source = Parameter::Source::FileDescriptorsWarning;
            parameter.numberOfFileDescriptors = possibleNumberOfActiveFileDescriptors;
            asyncPurge(parameter);
        }
    } else if (errno == EMFILE) {
        Parameter parameter;
        parameter.source = Parameter::Source::OutOfMaxAllowedFileDescriptors;
        asyncPurge(parameter);
    }
}
#endif

#pragma mark - Operation
OperationQueue::Operation::Operation(Type type_) : type(type_)
{
}

OperationQueue::Operation::Operation(Type type_, const UnsafeStringView& path_)
: type(type_), path(path_)
{
}

bool OperationQueue::Operation::operator==(const Operation& other) const
{
    bool equal = false;
    if (type == other.type) {
        if (type == Type::Purge) {
            equal = true;
        } else {
            equal = (path == other.path);
        }
    }
    return equal;
}

OperationQueue::Parameter::Parameter()
: source(Source::Other), numberOfFailures(0), identifier(0), numberOfFileDescriptors(0)
{
}

void OperationQueue::onTimed(const Operation& operation, const Parameter& parameter)
{
    executeOperationWithAutoMemoryRelease([&]() {
        if (operation.type != Operation::Type::NotifyCorruption) {
            CommonCore::shared().setThreadedErrorIgnorable(true);
        }
        switch (operation.type) {
        case Operation::Type::Migrate:
            doMigrate(operation.path, parameter.numberOfFailures);
            break;
        case Operation::Type::Compress:
            doCompress(operation.path, parameter.numberOfFailures);
            break;
        case Operation::Type::Checkpoint:
            doCheckpoint(operation.path);
            break;
        case Operation::Type::Purge:
            WCTAssert(operation.path.empty());
            doPurge(parameter);
            break;
        case Operation::Type::Integrity:
            doCheckIntegrity(operation.path);
            break;
        case Operation::Type::NotifyCorruption:
            doNotifyCorruption(operation.path, parameter.identifier);
            break;
        case Operation::Type::MergeIndex:
            doMergeFTSIndex(operation.path, parameter.newTables, parameter.modifiedTables);
            break;
        case Operation::Type::Backup:
            doBackup(operation.path);
            break;
        }
        if (operation.type != Operation::Type::NotifyCorruption) {
            CommonCore::shared().setThreadedErrorIgnorable(false);
        }
    });
}

void OperationQueue::async(const Operation& operation, double delay, const Parameter& parameter, AsyncMode mode)
{
    m_timedQueue.queue(operation, delay, parameter, mode);
}

#pragma mark - Record
OperationQueue::Record::Record()
: registeredForMigration(false)
, registeredForCompression(false)
, registeredForBackup(false)
, registeredForCheckpoint(false)
{
}

#pragma mark - Migrate
void OperationQueue::registerAsRequiredMigration(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    m_records[path].registeredForMigration = true;
}

void OperationQueue::registerAsNoMigrationRequired(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    m_records[path].registeredForMigration = false;
    Operation operation(Operation::Type::Migrate, path);
    m_timedQueue.remove(operation);
}

void OperationQueue::asyncMigrate(const UnsafeStringView& path)
{
    asyncMigrate(path, OperationQueueTimeIntervalForMigration, 0);
}

void OperationQueue::stopMigrate(const UnsafeStringView& path)
{
    LockGuard lockGuard(m_lock);
    Operation operation(Operation::Type::Migrate, path);
    m_timedQueue.remove(operation);
}

void OperationQueue::asyncMigrate(const UnsafeStringView& path, double delay, int numberOfFailures)
{
    WCTAssert(!path.empty());
    WCTAssert(numberOfFailures >= 0
              && numberOfFailures < OperationQueueTolerableFailuresForMigration);

    SharedLockGuard lockGuard(m_lock);
    if (m_records[path].registeredForMigration) {
        Operation operation(Operation::Type::Migrate, path);
        Parameter parameter;
        parameter.numberOfFailures = numberOfFailures;
        async(operation, delay, parameter);
    }
}

void OperationQueue::doMigrate(const UnsafeStringView& path, int numberOfFailures)
{
    WCTAssert(!path.empty());
    WCTAssert(numberOfFailures >= 0
              && numberOfFailures < OperationQueueTolerableFailuresForMigration);

    auto done = m_event->migrationShouldBeOperated(path);
    if (done.succeed()) {
        if (!done.value()) {
            asyncMigrate(path, OperationQueueTimeIntervalForMigration, numberOfFailures);
        }
    } else {
        if (numberOfFailures + 1 < OperationQueueTolerableFailuresForMigration) {
            asyncMigrate(
            path, OperationQueueTimeIntervalForRetringAfterFailure, numberOfFailures + 1);
        } else {
            Error error(Error::Code::Notice,
                        Error::Level::Notice,
                        "Auto migration is stopped due to too many errors.");
            error.infos.insert_or_assign(ErrorStringKeyPath, path);
            error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeMigrate);
            Notifier::shared().notify(error);
        }
    }
}

#pragma mark - Compress
void OperationQueue::registerAsRequiredCompression(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    m_records[path].registeredForCompression = true;
}

void OperationQueue::registerAsNoCompressionRequired(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    m_records[path].registeredForCompression = false;
    Operation operation(Operation::Type::Compress, path);
    m_timedQueue.remove(operation);
}

void OperationQueue::asyncCompress(const UnsafeStringView& path)
{
    asyncCompress(path, OperationQueueTimeIntervalForCompression, 0);
}

void OperationQueue::stopCompress(const UnsafeStringView& path)
{
    LockGuard lockGuard(m_lock);
    Operation operation(Operation::Type::Compress, path);
    m_timedQueue.remove(operation);
}

void OperationQueue::asyncCompress(const UnsafeStringView& path, double delay, int numberOfFailures)
{
    WCTAssert(!path.empty());
    WCTAssert(numberOfFailures >= 0
              && numberOfFailures < OperationQueueTolerableFailuresForCompression);

    SharedLockGuard lockGuard(m_lock);
    if (m_records[path].registeredForCompression) {
        Operation operation(Operation::Type::Compress, path);
        Parameter parameter;
        parameter.numberOfFailures = numberOfFailures;
        async(operation, delay, parameter);
    }
}

void OperationQueue::doCompress(const UnsafeStringView& path, int numberOfFailures)
{
    WCTAssert(!path.empty());
    WCTAssert(numberOfFailures >= 0
              && numberOfFailures < OperationQueueTolerableFailuresForCompression);

    auto done = m_event->compressionShouldBeOperated(path);
    if (done.succeed()) {
        if (!done.value()) {
            asyncCompress(path, OperationQueueTimeIntervalForCompression, numberOfFailures);
        }
    } else {
        if (numberOfFailures + 1 < OperationQueueTolerableFailuresForCompression) {
            asyncCompress(
            path, OperationQueueTimeIntervalForRetringAfterFailure, numberOfFailures + 1);
        } else {
            Error error(Error::Code::Notice,
                        Error::Level::Notice,
                        "Auto compression is stopped due to too many errors.");
            error.infos.insert_or_assign(ErrorStringKeyPath, path);
            error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeCompress);
            Notifier::shared().notify(error);
        }
    }
}

#pragma mark - Merge FTS Index
void OperationQueue::registerAsRequiredMergeFTSIndex(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    m_records[path].registeredForMergeFTSIndex = true;
}

void OperationQueue::registerAsNoMergeFTSIndexRequired(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    m_records[path].registeredForMergeFTSIndex = false;
    Operation operation(Operation::Type::MergeIndex, path);
    m_timedQueue.remove(operation);
}

void OperationQueue::asyncMergeFTSIndex(const UnsafeStringView& path,
                                        TableArray newTables,
                                        TableArray modifiedTables)
{
    SharedLockGuard lockGuard(m_lock);
    if (m_records[path].registeredForMergeFTSIndex) {
        Operation operation(Operation::Type::MergeIndex, path);
        Parameter parameter;
        parameter.newTables = newTables;
        parameter.modifiedTables = modifiedTables;
        async(operation, OperationQueueTimeIntervalForMergeFTSIndex, parameter);
    }
}

void OperationQueue::doMergeFTSIndex(const UnsafeStringView& path,
                                     TableArray newTables,
                                     TableArray modifiedTables)
{
    WCTAssert(!path.empty());

    m_event->mergeFTSIndexShouldBeOperated(path, newTables, modifiedTables);
}

#pragma mark - Backup
bool OperationQueue::isAutoBackup(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    SharedLockGuard lockGuard(m_lock);
    return m_records[path].registeredForBackup;
}

void OperationQueue::registerAsRequiredBackup(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    m_records[path].registeredForBackup = true;
}

void OperationQueue::registerAsNoBackupRequired(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    m_records[path].registeredForBackup = false;
    Operation operation(Operation::Type::Backup, path);
    m_timedQueue.remove(operation);
}

void OperationQueue::asyncBackup(const UnsafeStringView& path, bool incremental)
{
    WCTAssert(!path.empty());

    SharedLockGuard lockGuard(m_lock);
    auto iter = m_records.find(path);
    if (iter != m_records.end() && iter->second.registeredForBackup) {
        asyncBackup(path, incremental ? 0 : OperationQueueTimeIntervalForBackup);
    }
}

void OperationQueue::asyncBackup(const UnsafeStringView& path, double delay)
{
    WCTAssert(!path.empty());

    Operation operation(Operation::Type::Backup, path);
    Parameter parameter; // no use
    async(operation, delay, parameter);
}

void OperationQueue::doBackup(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    m_event->backupShouldBeOperated(path);
}

#pragma mark - Checkpoint
void OperationQueue::registerAsRequiredCheckpoint(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    m_records[path].registeredForCheckpoint = true;
}

void OperationQueue::registerAsNoCheckpointRequired(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    m_records[path].registeredForCheckpoint = false;

    Operation operation(Operation::Type::Checkpoint, path);
    m_timedQueue.remove(operation);
}

void OperationQueue::asyncCheckpoint(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    SharedLockGuard lockGuard(m_lock);
    auto iter = m_records.find(path);
    if (iter != m_records.end() && iter->second.registeredForCheckpoint) {
        Operation operation(Operation::Type::Checkpoint, path);
        Parameter parameter;
        double checkPointInterval = OperationQueueTimeIntervalForCheckpoint;
        auto config = CommonCore::shared().getABTestConfig("clicfg_wcdb_checkpoint_interval");
        if (config.valueOrDefault().length() > 0) {
            checkPointInterval = std::max(checkPointInterval, atof(config->data()));
            checkPointInterval = std::min(checkPointInterval, 600.0);
        }
        async(operation, checkPointInterval, parameter, AsyncMode::ForwardOnly);
    }
}

void OperationQueue::doCheckpoint(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    m_event->checkpointShouldBeOperated(path);
}

#pragma mark - Purge
#ifndef _WIN32
int OperationQueue::maxAllowedNumberOfFileDescriptors()
{
    const int s_maxAllowedNumberOfFileDescriptors = std::max(getdtablesize(), 1024);
    return s_maxAllowedNumberOfFileDescriptors;
}
#endif

void OperationQueue::asyncPurge(const Parameter& parameter)
{
    WCTAssert(parameter.source != Parameter::Source::Other);

    SharedLockGuard lockGuard(m_lock);
    if (SteadyClock::timeIntervalSinceSteadyClockToNow(m_lastPurge)
        > OperationQueueTimeIntervalForPurgingAgain) {
        Operation operation(Operation::Type::Purge);
        async(operation, 0, parameter);
    }
}

void OperationQueue::doPurge(const Parameter& parameter)
{
    switch (parameter.source) {
    case Parameter::Source::MemoryWarning: {
        Error error(Error::Code::Warning, Error::Level::Warning, "Purge due to memory warning.");
        Notifier::shared().notify(error);
    } break;
#ifndef _WIN32
    case Parameter::Source::FileDescriptorsWarning: {
        Error error(Error::Code::Warning, Error::Level::Warning, "Purge due to file descriptors warning.");
        error.infos.insert_or_assign("MaxAllowedFileDescriptors",
                                     maxAllowedNumberOfFileDescriptors());
        error.infos.insert_or_assign("FileDescriptor", parameter.numberOfFileDescriptors);
        Notifier::shared().notify(error);
    } break;
    default: {
        WCTAssert(parameter.source == Parameter::Source::OutOfMaxAllowedFileDescriptors);
        Error error(Error::Code::Warning,
                    Error::Level::Warning,
                    "Purge due to out of max allowed file descriptors.");
        error.infos.insert_or_assign("MaxAllowedFileDescriptors",
                                     maxAllowedNumberOfFileDescriptors());
        Notifier::shared().notify(error);
    } break;
#endif
    }

    m_event->purgeShouldBeOperated();

    LockGuard lockGuard(m_lock);
    WCTAssert(SteadyClock::now() > m_lastPurge);
    m_lastPurge = SteadyClock::now();
}

void OperationQueue::asyncPurgeWhenMemoryWarning()
{
    Parameter parameter;
    parameter.source = Parameter::Source::MemoryWarning;
    this->asyncPurge(parameter);
}

#pragma mark - Check Integrity
void OperationQueue::skipIntegrityCheck(const UnsafeStringView& path)
{
    m_skipIntegrityCheckPath.getOrCreate() = path;
}

void OperationQueue::asyncCheckIntegrity(const UnsafeStringView& path, uint32_t identifier)
{
    WCTAssert(!path.empty());
    WCTAssert(identifier != 0);

    SharedLockGuard lockGuard(m_lock);
    if (m_corrupteds.find(identifier) == m_corrupteds.end()) {
        Operation operation(Operation::Type::Integrity, path);
        Parameter parameter; // useless
        async(operation, 0, parameter);
    }
}

void OperationQueue::doCheckIntegrity(const UnsafeStringView& path)
{
    WCTAssert(!path.empty());

    m_event->integrityShouldBeChecked(path);
}

#pragma mark - Corrupted
void OperationQueue::setNotificationWhenCorrupted(const UnsafeStringView& path,
                                                  const CorruptionNotification& notification)
{
    WCTAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    if (notification != nullptr) {
        m_corruptionNotifications[path] = notification;
    } else {
        m_corruptionNotifications.erase(path);
    }
}

bool OperationQueue::isFileObservedCorrupted(const UnsafeStringView& path) const
{
    WCTAssert(!path.empty());

    bool corrupted = false;
    auto identifier = FileManager::getFileIdentifier(path);
    if (identifier.succeed()) {
        SharedLockGuard lockGuard(m_lock);
        corrupted = m_corrupteds.find(identifier.value()) != m_corrupteds.end();
    }
    return corrupted;
}

void OperationQueue::asyncNotifyCorruption(const UnsafeStringView& path, uint32_t identifier)
{
    WCTAssert(!path.empty());
    WCTAssert(identifier != 0);

    Operation operation(Operation::Type::NotifyCorruption, path);
    Parameter parameter;
    parameter.identifier = identifier;
    async(operation, 0, parameter);
}

void OperationQueue::doNotifyCorruption(const UnsafeStringView& path, uint32_t identifier)
{
    WCTAssert(!path.empty());
    WCTAssert(identifier != 0);

    CorruptionNotification notification;
    {
        SharedLockGuard lockGuard(m_lock);
        auto iter = m_corruptionNotifications.find(path);
        if (iter != m_corruptionNotifications.end()) {
            WCTAssert(iter->second != nullptr);
            notification = iter->second;
        }
    }
    notification(path, identifier);
}

} // namespace WCDB
