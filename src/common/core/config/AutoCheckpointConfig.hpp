//
// Created by sanhuazhang on 2019/05/27
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
#include "Lock.hpp"
#include "Statement.hpp"

namespace WCDB {

class AutoCheckpointOperator {
public:
    virtual ~AutoCheckpointOperator() = 0;

    virtual void asyncCheckpoint(const UnsafeStringView &path) = 0;
};

class AutoCheckpointConfig final : public Config {
public:
    AutoCheckpointConfig(const std::shared_ptr<AutoCheckpointOperator> &operator_);
    ~AutoCheckpointConfig() override;

    bool invoke(InnerHandle *handle) override final;
    bool uninvoke(InnerHandle *handle) override final;
    void setMinFrames(int frame);

protected:
    const StringView m_identifier;
    bool onCommitted(const UnsafeStringView &path, int pages);
    void log(int rc, const char *message);

    int m_minFrames;
    std::shared_ptr<AutoCheckpointOperator> m_operator;
    Statement m_disableAutoCheckpoint;
    StringViewMap<int> m_frames;
    mutable SharedLock m_lock;
};

} //namespace WCDB
