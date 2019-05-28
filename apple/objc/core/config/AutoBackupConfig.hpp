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

#include <WCDB/Config.hpp>
#include <WCDB/TimedQueue.hpp>

namespace WCDB {

class AutoBackupOperator {
public:
    virtual ~AutoBackupOperator() = 0;

    virtual void registerAsRequiredBackup(const String& path) = 0;
    virtual void registerAsNoBackupRequired(const String& path) = 0;

    virtual void asyncBackup(const String& path) = 0;
};

class AutoBackupConfig final : public Config {
public:
    AutoBackupConfig(const std::shared_ptr<AutoBackupOperator>& operator_);

    bool invoke(Handle* handle) override final;
    bool uninvoke(Handle* handle) override final;

protected:
    const String m_identifier;
    void onCheckpointed(const String& path);

    std::shared_ptr<AutoBackupOperator> m_operator;
};

} //namespace WCDB
