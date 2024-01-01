//
// Created by sanhuazhang on 2019/05/26
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
#include "StringView.hpp"
#include <map>
#include <memory>

namespace WCDB {

class AutoMigrateOperator {
public:
    virtual ~AutoMigrateOperator() = 0;
    virtual void asyncMigrate(const UnsafeStringView &path) = 0;
    virtual void stopMigrate(const UnsafeStringView &path) = 0;
};

class AutoMigrateConfig final : public Config {
public:
    AutoMigrateConfig(const std::shared_ptr<AutoMigrateOperator> &operator_);
    ~AutoMigrateConfig() override;

    bool invoke(InnerHandle *handle) override final;
    bool uninvoke(InnerHandle *handle) override final;

protected:
    std::shared_ptr<AutoMigrateOperator> m_operator;

    std::atomic<int> &getOrCreateRegister(const UnsafeStringView &path);

private:
    SharedLock m_lock;
    StringViewMap<std::atomic<int>> m_registers;
};

} //namespace WCDB
