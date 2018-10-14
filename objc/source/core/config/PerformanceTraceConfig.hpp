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

#ifndef PerformanceTraceConfig_hpp
#define PerformanceTraceConfig_hpp

#include <WCDB/Config.hpp>
#include <WCDB/Lock.hpp>

namespace WCDB {

class PerformanceTracer {
public:
    PerformanceTracer();
    virtual ~PerformanceTracer();

    using Notification = Handle::PerformanceNotification;
    virtual void setNotification(const Notification &notification);

protected:
    bool invoke(Handle *handle);
    bool uninvoke(Handle *handle);

private:
    const std::string m_identifier;
    Notification m_notification;
};

class PerformanceTraceConfig final : public Config, public PerformanceTracer {
public:
    PerformanceTraceConfig(const Notification &notification);

    bool invoke(Handle *handle) override final;
    bool uninvoke(Handle *handle) override final;

private:
    using PerformanceTracer::setNotification;
};

class ShareablePerformanceTraceConfig final : public Config, public PerformanceTracer {
public:
    ShareablePerformanceTraceConfig();

    bool invoke(Handle *handle) override final;
    bool uninvoke(Handle *handle) override final;

    void setNotification(const Notification &notification) override final;

protected:
    mutable SharedLock m_lock;
};

} //namespace WCDB

#endif /* PerformanceTraceConfig_hpp */
