//
// Created by sanhuazhang on 2019/05/02
//

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

#pragma once

#include "Config.hpp"
#include "InnerHandle.hpp"
#include "Lock.hpp"

namespace WCDB {

class PerformanceTracer {
public:
    PerformanceTracer();
    virtual ~PerformanceTracer() = 0;

    using Notification = InnerHandle::PerformanceNotification;

protected:
    void setNotification(const Notification &notification);
    bool invoke(InnerHandle *handle);
    bool uninvoke(InnerHandle *handle);

    const StringView m_identifier;
    Notification m_notification;
};

class PerformanceTraceConfig final : public Config, public PerformanceTracer {
public:
    PerformanceTraceConfig(const Notification &notification);
    ~PerformanceTraceConfig() override;

    bool invoke(InnerHandle *handle) override final;
    bool uninvoke(InnerHandle *handle) override final;

private:
    using PerformanceTracer::setNotification;
};

class ShareablePerformanceTraceConfig final : public Config, public PerformanceTracer {
public:
    ShareablePerformanceTraceConfig();
    ~ShareablePerformanceTraceConfig() override;

    bool invoke(InnerHandle *handle) override final;
    bool uninvoke(InnerHandle *handle) override final;

    void setNotification(const Notification &notification);

protected:
    mutable SharedLock m_lock;
};

} //namespace WCDB
