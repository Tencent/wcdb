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
#include <WCDB/Error.hpp>
#include <WCDB/RWLock.hpp>
#include <WCDB/Recyclable.hpp>
#include <WCDB/RecyclableHandle.hpp>
#include <WCDB/String.hpp>
#include <unordered_map>

namespace WCDB {

class HandlePool;
typedef Recyclable<std::shared_ptr<HandlePool>> RecyclableHandlePool;

class HandlePool {
public:
    static RecyclableHandlePool GetPool(const std::string &path);
    static RecyclableHandlePool GetExistingPool(Tag tag);
    static RecyclableHandlePool GetExistingPool(const std::string &path);

    static void PurgeFreeHandlesInAllPool();

protected:
    static RecyclableHandlePool GetExistingPool(
        const std::unordered_map<
            std::string,
            std::pair<std::shared_ptr<HandlePool>, int>>::iterator &iter);

protected:
    static std::unordered_map<std::string,
                              std::pair<std::shared_ptr<HandlePool>, int>>
        s_pools; //path->{pool, reference}
    static std::mutex s_mutex;

public:
    std::atomic<Tag> tag;
    const std::string path;

    RecyclableHandle flowOut();
    bool fillOne();
    const Error &getThreadedError() const;
    void setAndReportThreadedError(const Error &error) const;
    void resetThreadedError() const;

    bool isDrained();
    typedef std::function<void(void)> OnDrained;
    void blockade();
    void drain(const HandlePool::OnDrained &onDrained);
    void unblockade();
    bool isBlockaded() const;

    void purgeFreeHandles();

    void setConfig(const std::string &name, const Config::Callback &callback);
    void setConfig(const Config &config);

    ~HandlePool();

protected:
    HandlePool(const std::string &path, const Configs &configs);
    HandlePool(const HandlePool &) = delete;
    HandlePool &operator=(const HandlePool &) = delete;

    std::shared_ptr<ConfiguredHandle> generate();

    Configs m_configs;
    RWLock m_rwlock;

    void flowBack(const std::shared_ptr<ConfiguredHandle> &handleWrap);

    ConcurrentList<ConfiguredHandle> m_handles;
    std::atomic<int> m_aliveHandleCount;
    static const int s_hardwareConcurrency;
    static const int s_maxConcurrency;

    static ThreadLocal<std::unordered_map<const HandlePool *, Error>>
        s_threadedErrors;
};

} //namespace WCDB

#endif /* HandlePool_hpp */
