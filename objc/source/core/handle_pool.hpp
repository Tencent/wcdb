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

#ifndef handle_pool_hpp
#define handle_pool_hpp

#include <WCDB/abstract.h>
#include <WCDB/concurrent_list.hpp>
#include <WCDB/config.hpp>
#include <WCDB/error.hpp>
#include <WCDB/handle_recyclable.hpp>
#include <WCDB/recyclable.hpp>
#include <WCDB/rwlock.hpp>
#include <WCDB/utility.hpp>
#include <unordered_map>

namespace WCDB {

class HandlePool;
typedef Recyclable<std::shared_ptr<HandlePool>> RecyclableHandlePool;

class HandlePool {
public:
    static RecyclableHandlePool GetPool(const std::string &path,
                                        const Configs &defaultConfigs);
    static RecyclableHandlePool GetPool(Tag tag);
    static void PurgeFreeHandlesInAllPool();

protected:
    static std::unordered_map<std::string,
                              std::pair<std::shared_ptr<HandlePool>, int>>
        s_pools; //path->{pool, reference}
    static std::mutex s_mutex;

public:
    std::atomic<Tag> tag;
    const std::string path;

    RecyclableHandle flowOut(Error &error);
    bool fillOne(Error &error);

    bool isDrained();
    typedef std::function<void(void)> OnDrained;
    void blockade();
    void drain(HandlePool::OnDrained onDrained);
    void unblockade();
    bool isBlockaded() const;

    void purgeFreeHandles();

    void setConfig(const std::string &name,
                   const Config &config,
                   Configs::Order order);
    void setConfig(const std::string &name, const Config &config);

protected:
    HandlePool(const std::string &path, const Configs &configs);
    HandlePool(const HandlePool &) = delete;
    HandlePool &operator=(const HandlePool &) = delete;

    std::shared_ptr<HandleWrap> generate(Error &error);

    bool invoke(std::shared_ptr<HandleWrap> &handleWrap, Error &error);

    Configs m_configs;
    RWLock m_rwlock;

    void flowBack(const std::shared_ptr<HandleWrap> &handleWrap);

    ConcurrentList<HandleWrap> m_handles;
    std::atomic<int> m_aliveHandleCount;
    static const int s_hardwareConcurrency;
    static const int s_maxConcurrency;
};

} //namespace WCDB

#endif /* handle_pool_hpp */
