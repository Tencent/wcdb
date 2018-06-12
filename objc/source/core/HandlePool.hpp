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
#include <WCDB/Attachment.hpp>
#include <WCDB/ConcurrentList.hpp>
#include <WCDB/Config.hpp>
#include <WCDB/ErrorProne.hpp>
#include <WCDB/Factory.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/RecyclableHandle.hpp>
#include <WCDB/ThreadedErrors.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class HandlePool : private ThreadedErrorProne {
#pragma mark - Initialize
public:
    HandlePool() = delete;
    HandlePool(const HandlePool &) = delete;
    HandlePool &operator=(const HandlePool &) = delete;

    HandlePool(const std::string &path,
               const std::shared_ptr<const Configs> &configs);

    virtual ~HandlePool();

#pragma mark - Basic
public:
    using Tag = Handle::Tag;
    void setTag(const Tag &tag);
    Tag getTag() const;
    const std::string path;

protected:
    std::atomic<Tag> m_tag;

#pragma mark - Config
public:
    void setConfig(const std::shared_ptr<Config> &config);
    void removeConfig(const std::string &name);

protected:
    std::shared_ptr<const Configs> m_configs;

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

    typedef std::function<bool(const HandlePool &)>
        FirstHandleWillGenerateCallback;
    void setNotificationWhenFirstHandleWillGenerate(
        const FirstHandleWillGenerateCallback &firstHandleWillGenerate);

protected:
    std::shared_ptr<ConfiguredHandle> generateConfiguredHandle();
    std::shared_ptr<ConfiguredHandle> flowOutConfiguredHandle();
    void flowBackConfiguredHandle(
        const std::shared_ptr<ConfiguredHandle> &configuredHandle);
    virtual std::shared_ptr<Handle> generateHandle();

    SharedLock m_sharedLock;

    void flowBack(const std::shared_ptr<ConfiguredHandle> &handleWrap);

    ConcurrentList<ConfiguredHandle> m_handles;
    std::atomic<int> m_aliveHandleCount;
    static int hardwareConcurrency();
    static int maxConcurrency();
    FirstHandleWillGenerateCallback m_firstHandleWillGenerate;

#pragma mark - HandlePoolRelated
    friend class HandlePoolRelated;

#pragma mark - Attachment
public:
    Attachment attachment;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* HandlePool_hpp */
