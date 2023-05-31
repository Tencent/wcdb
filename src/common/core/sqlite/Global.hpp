//
// Created by sanhuazhang on 2019/05/19
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

#include "Lock.hpp"
#include "StringView.hpp"
#include <map>

namespace WCDB {

class Global {
public:
    static void initialize();
    static Global& shared();

protected:
    Global();
    Global(const Global&) = delete;
    Global& operator=(const Global&) = delete;

    mutable SharedLock m_lock;

private:
    static void staticAPIExit(int rc);

#pragma mark - Log
public:
    // rc, message
    typedef std::function<void(int, const char*)> LogNotification;
    void setNotificationForLog(const UnsafeStringView& name, const LogNotification& notification);

private:
    static void log(void* parameter, int rc, const char* message);
    void postLogNotification(int rc, const char* message);
    StringViewMap<LogNotification> m_logNotifications;

#ifndef _WIN32
#pragma mark - File Opened
public:
    typedef std::function<void(int /* fd */, const char* /* path */, int /* flags */, int /* mode */)> FileOpenedNotification;
    void setNotificationWhenFileOpened(const UnsafeStringView& name,
                                       const FileOpenedNotification& notification);

private:
    static int open(const char* path, int flags, int mode);
    void postFileOpenedNotification(int fd, const char* path, int flags, int mode);
    StringViewMap<FileOpenedNotification> m_fileOpenedNotifications;
#endif

#pragma mark - Lock
public:
    enum class PagerLock {
        None = 0,
        Shared = 1,
        Reserved = 2,
        Pending = 3,
        Exclusive = 4,
    };
    enum class ShmLock {
        Shared = 4,
        Exclusive = 8,
    };
    typedef std::function<void(const UnsafeStringView& /* path */, PagerLock)> WillLockNotification;
    typedef std::function<void(const UnsafeStringView& /* path */, PagerLock)> LockDidChangeNotification;
    typedef std::function<void(const UnsafeStringView& /* path */, ShmLock, int /* mask */)> WillShmLockNotification;
    typedef std::function<void(const UnsafeStringView& /* path */, void* /* identifier */, int /* sharedMask */, int /* exclMask */)> ShmLockDidChangeNotification;
    void setNotificationForLockEvent(const UnsafeStringView& name,
                                     const WillLockNotification& willLock,
                                     const LockDidChangeNotification& lockDidChange,
                                     const WillShmLockNotification& willShmLock,
                                     const ShmLockDidChangeNotification& shmLockDidChange);

private:
    static void willLock(void* parameter, const char* path, int type);
    void postWillLockNotification(const char* path, int type);

    static void lockDidChange(void* parameter, const char* path, int type);
    void postLockDidChangeNotification(const char* path, int type);

    static void willShmLock(void* parameter, const char* path, int flags, int mask);
    void postWillShmLockNotification(const char* path, int flags, int mask);

    static void
    shmLockDidChange(void* parameter, const char* path, void* identifier, int sharedMask, int exclusiveMask);
    void postShmLockDidChangeNotification(const char* path,
                                          void* identifier,
                                          int sharedMask,
                                          int exclusiveMask);

    struct LockEvent {
        WillLockNotification willLock;
        LockDidChangeNotification lockDidChange;
        WillShmLockNotification willShmLock;
        ShmLockDidChangeNotification shmLockDidChange;
    };
    typedef struct LockEvent LockEvent;
    StringViewMap<LockEvent> m_lockEventNotifications;
};

}; // namespace WCDB
