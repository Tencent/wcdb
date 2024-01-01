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
#include "Data.hpp"
#include "Lock.hpp"
#include "WINQ.h"

namespace WCDB {

class CipherConfig final : public Config {
public:
    CipherConfig(const UnsafeData &cipher, int pageSize, int cipherVersion);
    ~CipherConfig() override;

    bool invoke(InnerHandle *handle) override final;
    void trySaveRawKey(InnerHandle *handle);

protected:
    Data m_key;
    Data m_rawKey;
    mutable SharedLock m_lock;
    const int m_pageSize;
    const int m_cipherVersion = 4;
};

} //namespace WCDB
