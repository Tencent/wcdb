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

#ifndef Tracer_hpp
#define Tracer_hpp

#include <functional>
#include <list>
#include <string>

#pragma GCC visibility push(hidden)

namespace WCDB {

class Handle;

class Tracer {
public:
    struct Footprint {
        std::string sql;
        unsigned int frequency;
        Footprint(const std::string &sql);
    };
    typedef struct Footprint Footprint;

    using Footprints = std::list<Footprint>;
    typedef std::function<void(const Footprints &, const int64_t &)>
        PerformanceTraceCallback;

    void tracePerformance(void *handle, const PerformanceTraceCallback &trace);

    typedef std::function<void(const std::string &)> SQLTraceCallback;

    void traceSQL(void *handle, const SQLTraceCallback &trace);

protected:
    static int TraceCallback(unsigned int flag, void *M, void *P, void *X);

    void reportPerformance();
    void addPerformanceTrace(const std::string &sql, const int64_t &cost);
    void reportSQL(const std::string &sql);

    void setupTrace(void *handle);

    PerformanceTraceCallback m_performanceTrace;
    SQLTraceCallback m_sqlTrace;

    Footprints m_footprints;
    int64_t m_cost;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* Tracer_hpp */
