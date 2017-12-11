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

#ifndef ticker_hpp
#define ticker_hpp

#include <string>
#include <vector>

namespace WCDB {

class Ticker {
public:
    Ticker();
    void tick();
    void pause();

    //in seconds
    std::vector<double> getElapseTimes() const;
    double getElapseTime() const;

    std::string log() const;

protected:
    static double secondsFromElapse(const uint64_t &elapse);

    uint64_t m_base;
    std::vector<uint64_t> m_elapses;
};

class ScopedTicker {
public:
    ScopedTicker(std::shared_ptr<Ticker> &ticker);
    ~ScopedTicker();

protected:
    std::shared_ptr<Ticker> &m_ticker;
};

} //namespace WCDB

#endif /* ticker_hpp */
