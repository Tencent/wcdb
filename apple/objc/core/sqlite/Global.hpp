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

#include <WCDB/Lock.hpp>
#include <WCDB/String.hpp>
#include <map>

namespace WCDB {

class Global {
public:
    static Global &shared();

protected:
    Global();
    Global(const Global &) = delete;
    Global &operator=(const Global &) = delete;

    mutable SharedLock m_lock;

private:
    static void staticAPIExit(int rc);

#pragma mark - Log
public:
    // rc, message
    typedef std::function<void(int, const char *)> LogNotification;
    void setNotificationForLog(const String &name, const LogNotification &notification);

private:
    static void log(void *parameter, int rc, const char *message);
    void postLogNotification(int rc, const char *message);
    std::map<String, LogNotification> m_logNotifications;

#pragma mark - File Opened
public:
    typedef std::function<void(int /* fd */, const char * /* path */, int /* flags */, int /* mode */)> FileOpenedNotification;
    void setNotificationWhenFileOpened(const String &name,
                                       const FileOpenedNotification &notification);

private:
    static int open(const char *path, int flags, int mode);
    void postFileOpenedNotification(int fd, const char *path, int flags, int mode);
    std::map<String, FileOpenedNotification> m_fileOpenedNotifications;

#pragma mark - Lock
public:
    enum class LockAction {
        Lock,
        Unlock,
    };

    enum class LockType {
        None = 0,
        Shared = 1,
        Reserved = 2,
        Pending = 3,
        Exclusive = 4,
    };

    enum class ShmLockType {
        Shared = 4,
        Exclusive = 8,
    };

    typedef std::function<void(const String &, LockAction, LockType)> LockNotification;
    void setNotificationForLocking(const String &name, const LockNotification &notification);

    typedef std::function<void(const String &, LockAction, ShmLockType, int, int)> ShmLockNotification;
    void setNotificationForShmLocking(const String &name,
                                      const ShmLockNotification &notification);

private:
    static void preLock(const char *path, int lockType);
    static void postUnlock(const char *path, int lockType);
    static void preShmLock(const char *path, int offset, int n, int lockType);
    static void postShmUnlock(const char *path, int offset, int n, int lockType);

    void postLockNotification(const String &path, LockAction action, LockType lockType);
    void postShmLockNotification(
    const String &path, LockAction action, ShmLockType lockType, int offset, int n);

    std::map<String, LockNotification> m_lockNotifications;
    std::map<String, ShmLockNotification> m_shmLockNotifications;
};

}; // namespace WCDB
