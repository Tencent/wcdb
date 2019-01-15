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

#ifndef _WCDB_BACKUPCONFIG_HPP
#define _WCDB_BACKUPCONFIG_HPP

#include <WCDB/Config.hpp>
#include <WCDB/TimedQueue.hpp>

namespace WCDB {

class BackupQueue;

class BackupConfig final : public Config {
public:
    BackupConfig(const std::shared_ptr<BackupQueue>& queue);

    bool invoke(Handle* handle) override final;
    bool uninvoke(Handle* handle) override final;

protected:
    const String m_identifier;
    bool onCommitted(const String& path, int frames);
    void onCheckpointed(const String& path);

    std::shared_ptr<BackupQueue> m_queue;
};

} //namespace WCDB

#endif /* _WCDB_BACKUPCONFIG_HPP */
