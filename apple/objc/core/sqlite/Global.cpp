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
#include <WCDB/Global.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/SQLite.h>
#include <fcntl.h>

static_assert((int) WCDB::Global::LockType::None == SQLITE_LOCK_NONE, "");
static_assert((int) WCDB::Global::LockType::Shared == SQLITE_LOCK_SHARED, "");
static_assert((int) WCDB::Global::LockType::Reserved == SQLITE_LOCK_RESERVED, "");
static_assert((int) WCDB::Global::LockType::Pending == SQLITE_LOCK_PENDING, "");
static_assert((int) WCDB::Global::LockType::Exclusive == SQLITE_LOCK_EXCLUSIVE, "");

static_assert((int) WCDB::Global::ShmLockType::Shared == SQLITE_SHM_SHARED, "");
static_assert((int) WCDB::Global::ShmLockType::Exclusive == SQLITE_SHM_EXCLUSIVE, "");

namespace WCDB {

Global &Global::shared()
{
    static Global *s_shared = new Global;
    return *s_shared;
}

Global::Global()
{
    {
        int rc = sqlite3_config(SQLITE_CONFIG_LOG, Global::log, this);
        WCTInnerAssert(rc == SQLITE_OK);
        staticAPIExit(rc);
    }

    {
        int rc = sqlite3_os_hook(
        Global::preLock, Global::postUnlock, Global::preShmLock, Global::postShmUnlock);
        WCTInnerAssert(rc == SQLITE_OK);
        staticAPIExit(rc);
    }
}

void Global::staticAPIExit(int rc)
{
    if (Error::isError(rc)) {
        Error error;
        error.level = Error::Level::Fatal;
        error.setSQLiteCode(rc);
        Notifier::shared().notify(error);
    }
}

#pragma mark - Config
void Global::enableMultithread()
{
    int rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    WCTInnerAssert(rc == SQLITE_OK);
    staticAPIExit(rc);
}

//void Global::setMemoryMapSize(int64_t defaultSizeLimit, int64_t maximumAllowedSizeLimit)
//{
//    int rc = sqlite3_config(SQLITE_CONFIG_MMAP_SIZE,
//                   (sqlite3_int64) defaultSizeLimit,
//                   (sqlite3_int64) maximumAllowedSizeLimit);
//    WCTInnerAssert(rc == SQLITE_OK);
//    staticAPIExit(rc);
//}

void Global::enableMemoryStatus(bool enable)
{
    int rc = sqlite3_config(SQLITE_CONFIG_MEMSTATUS, (int) enable);
    WCTInnerAssert(rc == SQLITE_OK);
    staticAPIExit(rc);
}

#pragma mark - Log
void Global::setNotificationForLog(const String &name, const LogNotification &notification)
{
    LockGuard lockGuard(m_lock);
    if (notification != nullptr) {
        m_logNotifications[name] = notification;
    } else {
        m_logNotifications.erase(name);
    }
}

void Global::postLogNotification(int rc, const char *message)
{
    SharedLockGuard lockGuard(m_lock);
    for (const auto &iter : m_logNotifications) {
        WCTInnerAssert(iter.second != nullptr);
        iter.second(rc, message);
    }
}

void Global::log(void *parameter, int rc, const char *message)
{
    Global *global = static_cast<Global *>(parameter);
    global->postLogNotification(rc, message);
}

#pragma mark - File Opened
void Global::setNotificationWhenFileOpened(const String &name,
                                           const FileOpenedNotification &notification)
{
    LockGuard lockGuard(m_lock);
    if (notification != nullptr) {
        m_fileOpenedNotifications[name] = notification;
    } else {
        m_fileOpenedNotifications.erase(name);
    }
}

int Global::opened(const char *path, int flags, int mode)
{
    int fd = open(path, flags, mode);
    Global::shared().postFileOpenedNotification(fd, path, flags, mode);
    return fd;
}

void Global::postFileOpenedNotification(int fd, const char *path, int flags, int mode)
{
    SharedLockGuard lockGuard(m_lock);
    for (const auto &iter : m_fileOpenedNotifications) {
        WCTInnerAssert(iter.second != nullptr);
        iter.second(fd, path, flags, mode);
    }
}

#pragma mark - Lock
void Global::setNotificationForLocking(const String &name, const LockNotification &notification)
{
    LockGuard lockGuard(m_lock);
    if (notification != nullptr) {
        m_lockNotifications[name] = notification;
    } else {
        m_lockNotifications.erase(name);
    }
}

void Global::setNotificationForShmLocking(const String &name, const ShmLockNotification &notification)
{
    LockGuard lockGuard(m_lock);
    if (notification != nullptr) {
        m_shmLockNotifications[name] = notification;
    } else {
        m_shmLockNotifications.erase(name);
    }
}

void Global::postLockNotification(const String &path, LockAction action, LockType lockType)
{
    SharedLockGuard lockGuard(m_lock);
    for (const auto &iter : m_lockNotifications) {
        WCTInnerAssert(!path.empty());
        WCTInnerAssert(iter.second != nullptr);
        iter.second(path, action, lockType);
    }
}

void Global::postShmLockNotification(
const String &path, LockAction action, ShmLockType lockType, int offset, int n)
{
    SharedLockGuard lockGuard(m_lock);
    for (const auto &iter : m_shmLockNotifications) {
        WCTInnerAssert(!path.empty());
        WCTInnerAssert(iter.second != nullptr);
        iter.second(path, action, lockType, offset, n);
    }
}

void Global::preLock(const char *path, int lockType)
{
    WCTInnerAssert(lockType == (int) LockType::None || lockType == (int) LockType::Shared
                   || lockType == (int) LockType::Reserved || lockType == (int) LockType::Pending
                   || lockType == (int) LockType::Exclusive);
    Global::shared().postLockNotification(path, LockAction::Lock, (LockType) lockType);
}

void Global::postUnlock(const char *path, int lockType)
{
    WCTInnerAssert(lockType == (int) LockType::None || lockType == (int) LockType::Shared
                   || lockType == (int) LockType::Reserved || lockType == (int) LockType::Pending
                   || lockType == (int) LockType::Exclusive);
    Global::shared().postLockNotification(path, LockAction::Unlock, (LockType) lockType);
}

void Global::preShmLock(const char *path, int offset, int n, int lockType)
{
    WCTInnerAssert(lockType == (int) ShmLockType::Shared
                   || lockType == (int) ShmLockType::Exclusive);
    Global::shared().postShmLockNotification(
    path, LockAction::Lock, (ShmLockType) lockType, offset, n);
}

void Global::postShmUnlock(const char *path, int offset, int n, int lockType)
{
    WCTInnerAssert(lockType == (int) ShmLockType::Shared
                   || lockType == (int) ShmLockType::Exclusive);
    Global::shared().postShmLockNotification(
    path, LockAction::Unlock, (ShmLockType) lockType, offset, n);
}

} // namespace WCDB
