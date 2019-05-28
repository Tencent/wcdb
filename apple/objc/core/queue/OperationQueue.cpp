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
#include <WCDB/FileManager.hpp>
#include <WCDB/Global.hpp>
#include <WCDB/OperationQueue.hpp>
#include <fcntl.h>
#include <unistd.h>

namespace WCDB {

OperationEvent::~OperationEvent()
{
}

OperationQueue::OperationQueue(const String& name, OperationEvent* event)
: AsyncQueue(name)
, m_event(event)
, m_observerForMemoryWarning(registerNotificationWhenMemoryWarning())
{
    Notifier::shared().setNotification(
    0, name, std::bind(&OperationQueue::handleError, this, std::placeholders::_1));

    Global::shared().setNotificationWhenFileOpened(
    name,
    std::bind(&OperationQueue::observatedThatFileOpened,
              this,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3,
              std::placeholders::_4));
}

OperationQueue::~OperationQueue()
{
    Global::shared().setNotificationWhenFileOpened(name, nullptr);

    Notifier::shared().unsetNotification(name);

    unregisterNotificationWhenMemoryWarning(m_observerForMemoryWarning);
}

void OperationQueue::handleError(const Error& error)
{
    if (!error.isCorruption() || isExiting()) {
        return;
    }

    const auto& infos = error.infos.getStrings();

    auto iter = infos.find(ErrorStringKeyPath);
    if (iter == infos.end() || iter->second.empty()) {
        // make sure no empty path will be added into queue
        return;
    }
    const String& path = iter->second;

    bool succeed;
    uint32_t identifier;
    std::tie(succeed, identifier) = FileManager::getFileIdentifier(path);
    if (!succeed) {
        return;
    }

    bool fromIntegrity = false;
    const auto& stringInfos = error.infos.getStrings();
    auto actionIter = stringInfos.find(ErrorStringKeyAction);
    if (actionIter != stringInfos.end() && actionIter->second == ErrorActionIntegrity) {
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
        asyncCheckIntegrity(path, identifier);
    }
}

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
        if (type == Type::Purge) {
            equal = true;
        } else {
            equal = (path == other.path);
        }
    }
    return equal;
}

OperationQueue::Parameter::Parameter()
: source(Source::Other)
, frames(0)
, numberOfFailures(0)
, identifier(0)
, numberOfFileDescriptors(0)
, critical(false)
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
        doMigrate(operation.path, parameter.numberOfFailures);
        break;
    case Operation::Type::Checkpoint:
        doCheckpoint(operation.path, parameter.critical);
        break;
    case Operation::Type::Purge:
        WCTInnerAssert(operation.path.empty());
        doPurge(parameter);
        break;
    case Operation::Type::Integrity:
        doCheckIntegrity(operation.path);
        break;
    case Operation::Type::NotifyCorruption:
        doNotifyCorruption(operation.path, parameter.identifier);
        break;
    default:
        WCTInnerAssert(operation.type == Operation::Type::Backup);
        doBackup(operation.path);
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
: registeredForMigration(0), registeredForBackup(0), registeredForCheckpoint(0)
{
}

#pragma mark - Migrate
void OperationQueue::registerAsRequiredMigration(const String& path)
{
    WCTInnerAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    int queue = m_records[path].registeredForMigration == 0;
    ++m_records[path].registeredForMigration;
    if (queue) {
        asyncMigrate(path, OperationQueueTimeIntervalForMigration, 0);
    }
}

void OperationQueue::registerAsNoMigrationRequired(const String& path)
{
    WCTInnerAssert(!path.empty());

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
    WCTInnerAssert(!path.empty());
    WCTInnerAssert(numberOfFailures >= 0
                   && numberOfFailures < OperationQueueTolerableFailuresForMigration);

    SharedLockGuard lockGuard(m_lock);
    if (m_records[path].registeredForMigration > 0) {
        Operation operation(Operation::Type::Migrate, path);
        Parameter parameter;
        parameter.numberOfFailures = numberOfFailures;
        async(operation, delay, parameter);
    }
}

void OperationQueue::doMigrate(const String& path, int numberOfFailures)
{
    WCTInnerAssert(!path.empty());
    WCTInnerAssert(numberOfFailures >= 0
                   && numberOfFailures < OperationQueueTolerableFailuresForMigration);

    bool succeed, done;
    std::tie(succeed, done) = m_event->migrationShouldBeOperated(path);
    if (succeed) {
        if (!done) {
            asyncMigrate(path, OperationQueueTimeIntervalForMigration, numberOfFailures);
        }
    } else {
        if (numberOfFailures + 1 < OperationQueueTolerableFailuresForMigration) {
            asyncMigrate(path,
                         OperationQueueTimeIntervalForRetryingMigrationAfterFailure,
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
    WCTInnerAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    ++m_records[path].registeredForBackup;
}

void OperationQueue::registerAsNoBackupRequired(const String& path)
{
    WCTInnerAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    Record& record = m_records[path];
    --record.registeredForBackup;
    WCTInnerAssert(record.registeredForBackup >= 0);
    if (record.registeredForBackup == 0) {
        Operation operation(Operation::Type::Backup, path);
        m_timedQueue.remove(operation);
    }
}

void OperationQueue::asyncBackup(const String& path)
{
    WCTInnerAssert(!path.empty());

    SharedLockGuard lockGuard(m_lock);
    auto iter = m_records.find(path);
    if (iter != m_records.end() && iter->second.registeredForBackup > 0) {
        asyncBackup(path, OperationQueueTimeIntervalForBackup);
    }
}

void OperationQueue::asyncBackup(const String& path, double delay)
{
    WCTInnerAssert(!path.empty());

    Operation operation(Operation::Type::Backup, path);
    Parameter parameter; // no use
    async(operation, delay, parameter);
}

void OperationQueue::doBackup(const String& path)
{
    WCTInnerAssert(!path.empty());

    if (!m_event->backupShouldBeOperated(path)) {
        asyncBackup(path, OperationQueueTimeIntervalForRetryingBackupAfterFailure);
    }
}

#pragma mark - Checkpoint
void OperationQueue::registerAsRequiredCheckpoint(const String& path)
{
    WCTInnerAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    ++m_records[path].registeredForCheckpoint;
}

void OperationQueue::registerAsNoCheckpointRequired(const String& path)
{
    WCTInnerAssert(!path.empty());

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
    WCTInnerAssert(!path.empty());

    SharedLockGuard lockGuard(m_lock);
    auto iter = m_records.find(path);
    if (iter != m_records.end() && iter->second.registeredForCheckpoint > 0) {
        if (frames >= OperationQueueFramesThresholdForCriticalCheckpoint) {
            asyncCheckpoint(path, OperationQueueTimeIntervalForCriticalCheckpoint, true);
        } else {
            asyncCheckpoint(path, OperationQueueTimeIntervalForNonCriticalCheckpoint, false);
        }
    }
}

void OperationQueue::asyncCheckpoint(const String& path, double delay, bool critical)
{
    WCTInnerAssert(!path.empty());

    Operation operation(Operation::Type::Checkpoint, path);
    Parameter parameter;
    parameter.critical = critical;
    async(operation, delay, parameter);
}

void OperationQueue::doCheckpoint(const String& path, bool critical)
{
    WCTInnerAssert(!path.empty());

    if (!m_event->checkpointShouldBeOperated(path, critical)) {
        // delay retry if failed
        asyncCheckpoint(
        path, OperationQueueTimeIntervalForRetryingCheckpointAfterFailure, critical);
    }
}

#pragma mark - Purge
int OperationQueue::maxAllowedNumberOfFileDescriptors()
{
    const int s_maxAllowedNumberOfFileDescriptors = std::max(getdtablesize(), 1024);
    return s_maxAllowedNumberOfFileDescriptors;
}

void OperationQueue::asyncPurge(const Parameter& parameter)
{
    WCTInnerAssert(parameter.source != Parameter::Source::Other);

    SharedLockGuard lockGuard(m_lock);
    if (SteadyClock::now().timeIntervalSinceSteadyClock(m_lastPurge)
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
    case Parameter::Source::FileDescriptorsWarning: {
        Error error(Error::Code::Warning, Error::Level::Warning, "Purge due to file descriptors warning.");
        error.infos.set("MaxAllowedFileDescriptors", maxAllowedNumberOfFileDescriptors());
        error.infos.set("FileDescriptor", parameter.numberOfFileDescriptors);
        Notifier::shared().notify(error);
    } break;
    default: {
        WCTInnerAssert(parameter.source == Parameter::Source::OutOfMaxAllowedFileDescriptors);
        Error error(Error::Code::Warning,
                    Error::Level::Warning,
                    "Purge due to out of max allowed file descriptors.");
        error.infos.set("MaxAllowedFileDescriptors", maxAllowedNumberOfFileDescriptors());
        Notifier::shared().notify(error);
    } break;
    }

    m_event->purgeShouldBeOperated();

    LockGuard lockGuard(m_lock);
    WCTInnerAssert(SteadyClock::now() > m_lastPurge);
    m_lastPurge = SteadyClock::now();
}

#pragma mark - Check Integrity
void OperationQueue::asyncCheckIntegrity(const String& path, uint32_t identifier)
{
    WCTInnerAssert(!path.empty());
    WCTInnerAssert(identifier != 0);

    SharedLockGuard lockGuard(m_lock);
    if (m_corrupteds.find(identifier) == m_corrupteds.end()) {
        Operation operation(Operation::Type::Integrity, path);
        Parameter parameter; // useless
        async(operation, 0, parameter);
    }
}

void OperationQueue::doCheckIntegrity(const String& path)
{
    WCTInnerAssert(!path.empty());

    m_event->integrityShouldBeChecked(path);
}

#pragma mark - Corrupted
void OperationQueue::setNotificationWhenCorrupted(const String& path,
                                                  const CorruptionNotification& notification)
{
    WCTInnerAssert(!path.empty());

    LockGuard lockGuard(m_lock);
    if (notification != nullptr) {
        m_corruptionNotifications[path] = notification;
    } else {
        m_corruptionNotifications.erase(path);
    }
}

bool OperationQueue::isFileObservedCorrupted(const String& path) const
{
    WCTInnerAssert(!path.empty());

    bool corrupted = false;
    bool succeed;
    uint32_t identifier;
    std::tie(succeed, identifier) = FileManager::getFileIdentifier(path);
    if (succeed) {
        SharedLockGuard lockGuard(m_lock);
        corrupted = m_corrupteds.find(identifier) != m_corrupteds.end();
    }
    return corrupted;
}

void OperationQueue::asyncNotifyCorruption(const String& path, uint32_t identifier)
{
    WCTInnerAssert(!path.empty());
    WCTInnerAssert(identifier != 0);

    Operation operation(Operation::Type::NotifyCorruption, path);
    Parameter parameter;
    parameter.identifier = identifier;
    async(operation, 0, parameter);
}

void OperationQueue::doNotifyCorruption(const String& path, uint32_t identifier)
{
    WCTInnerAssert(!path.empty());
    WCTInnerAssert(identifier != 0);

    CorruptionNotification notification;
    {
        SharedLockGuard lockGuard(m_lock);
        auto iter = m_corruptionNotifications.find(path);
        if (iter != m_corruptionNotifications.end()) {
            WCTInnerAssert(iter->second != nullptr);
            notification = iter->second;
        }
    }
    notification(path, identifier);
}

} // namespace WCDB
