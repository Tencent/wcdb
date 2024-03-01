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
#include "IncrementalMaterial.hpp"
#include "Lock.hpp"
#include "StringView.hpp"

namespace WCDB {

class AbstractHandle;

class AutoBackupOperator {
public:
    virtual ~AutoBackupOperator() = 0;
    virtual void asyncBackup(const UnsafeStringView& path, bool incremental) = 0;
};

class AutoBackupConfig final : public Config {
public:
    AutoBackupConfig(const std::shared_ptr<AutoBackupOperator>& operator_);
    ~AutoBackupConfig() override;

    bool invoke(InnerHandle* handle) override final;
    bool uninvoke(InnerHandle* handle) override final;

    void tryRegisterIncrementalMaterial(const UnsafeStringView& path,
                                        SharedIncrementalMaterial material);
    SharedIncrementalMaterial tryGetIncrementalMaterial(const UnsafeStringView& path);

private:
    void onCheckpointBegin(AbstractHandle* handle,
                           uint32_t nBackFill,
                           uint32_t mxFrame,
                           uint32_t salt1,
                           uint32_t salt2);
    void onCheckpointPage(AbstractHandle* handle, uint32_t pageNo, const UnsafeData& data);
    void onCheckpointFinish(AbstractHandle* handle,
                            uint32_t nBackFill,
                            uint32_t mxFrame,
                            uint32_t salt1,
                            uint32_t salt2);

    void clearMaterial(const UnsafeStringView& path);

    mutable SharedLock m_lock;

    const StringView m_identifier;
    std::shared_ptr<AutoBackupOperator> m_operator;
    StringViewMap<SharedIncrementalMaterial> m_materials;
    typedef Repair::IncrementalMaterial::Pages Pages;
    StringViewMap<Pages> m_checkpointPages;
};

} //namespace WCDB
