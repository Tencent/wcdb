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

#ifndef HandlePool_hpp
#define HandlePool_hpp

#include <WCDB/Abstract.h>
#include <WCDB/ConcurrentList.hpp>
#include <WCDB/Config.hpp>
#include <WCDB/CoreError.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/RecyclableHandle.hpp>
#include <WCDB/String.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <unordered_map>

namespace WCDB {

class HandlePool {
#pragma mark - Initialize
public:
    friend class HandlePools;

    HandlePool() = delete;
    HandlePool(const HandlePool &) = delete;
    HandlePool &operator=(const HandlePool &) = delete;

    HandlePool(const std::string &path, const Configs &configs);

#pragma mark - Basic
public:
    using Tag = Handle::Tag;
    void setTag(const Tag &tag);
    Tag getTag() const;
    const std::string path;

protected:
    std::atomic<Tag> m_tag;

#pragma mark - Error
public:
    const Error &getThreadedError() const;
    void setThreadedError(const HandleError &error) const;
    void setThreadedError(const FileError &error) const;
    void setThreadedError(const CoreError &error) const;

protected:
    void setAndReportCoreError(const std::string &message);
    using ThreadedErrors =
        std::unordered_map<const HandlePool *, std::shared_ptr<Error>>;
    static ThreadLocal<ThreadedErrors> s_threadedErrors;
    std::shared_ptr<Error> &getThreadedErrors() const;

#pragma mark - Config
public:
    void setConfig(const std::string &name, const Config::Callback &callback);
    void setConfig(const Config &config);

protected:
    Configs m_configs;

#pragma mark - Handle
public:
    RecyclableHandle flowOut();
    bool canFlowOut();

    /**
     It will be trigger when pool is blockaded and handle is generated and configured successfully.
     */
    typedef std::function<void(Handle *)> BlockadeCallback;
    void blockade();
    bool blockadeUntilDone(const BlockadeCallback &onBlockaded);
    bool isBlockaded() const;
    void unblockade();

    typedef std::function<void(void)> DrainedCallback;
    void drain(const HandlePool::DrainedCallback &onDrained);
    bool isDrained();

    void purgeFreeHandles();

protected:
    virtual bool willConfigurateHandle(Handle *handle);
    std::shared_ptr<ConfiguredHandle> generateConfiguredHandle();
    std::shared_ptr<ConfiguredHandle> flowOutConfiguredHandle();
    virtual std::shared_ptr<Handle> generateHandle();

    SharedLock m_sharedLock;

    void flowBack(const std::shared_ptr<ConfiguredHandle> &handleWrap);

    ConcurrentList<ConfiguredHandle> m_handles;
    std::atomic<int> m_aliveHandleCount;
    static const int s_hardwareConcurrency;
    static const int s_maxConcurrency;
};

} //namespace WCDB

#endif /* HandlePool_hpp */
