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

#ifndef HandlePools_hpp
#define HandlePools_hpp

#include <WCDB/RecyclableHandlePool.hpp>

namespace WCDB {

class HandlePools {
public:
    HandlePools(const HandlePools &) = delete;
    HandlePools &operator=(const HandlePools &) = delete;

    static HandlePools *defaultPools();

    RecyclableHandlePool getPool(const std::string &path);
    RecyclableHandlePool getExistingPool(HandlePool::Tag tag);
    RecyclableHandlePool getExistingPool(const std::string &path);

    typedef std::function<std::shared_ptr<HandlePool>(const std::string &)>
        HandlePoolExtraGenerator;
    void setExtraGenerator(const std::string &path,
                           const HandlePoolExtraGenerator &generator);

    void purge();

    void drain();

protected:
    HandlePools();

    std::shared_ptr<HandlePool> generate(const std::string &path);

    RecyclableHandlePool getExistingPool(
        const std::unordered_map<
            std::string,
            std::pair<std::shared_ptr<HandlePool>, int>>::iterator &iter);

    std::unordered_map<std::string, std::pair<std::shared_ptr<HandlePool>, int>>
        m_pools; //path->{pool, reference}
    std::mutex m_mutex;

    std::unordered_map<std::string, HandlePoolExtraGenerator> m_extraGenerators;
};

} //namespace WCDB

#endif /* HandlePools_hpp */
