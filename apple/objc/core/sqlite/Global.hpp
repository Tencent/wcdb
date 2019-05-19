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

#ifndef __WCDB_GLOBAL_HPP
#define __WCDB_GLOBAL_HPP

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

#pragma mark - Config
public:
    static void enableMultithread();
    static void enableMemoryStatus(bool enable);
    //    static void setMemoryMapSize(int64_t defaultSizeLimit, int64_t maximumAllowedSizeLimit);

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
    static int opened(const char *path, int flags, int mode);
    void postFileOpenedNotification(int fd, const char *path, int flags, int mode);
    std::map<String, FileOpenedNotification> m_fileOpenedNotifications;
};

}; // namespace WCDB

#endif /* __WCDB_GLOBAL_HPP */
