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

namespace WCDB {

Global &Global::shared()
{
    static Global *s_shared = new Global;
    return *s_shared;
}

Global::Global()
{
    int rc = sqlite3_config(SQLITE_CONFIG_LOG, Global::log, this);
    WCTInnerAssert(rc == SQLITE_OK);
    staticAPIExit(rc);
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

} // namespace WCDB
