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

#include "CipherConfig.hpp"
#include "Assertion.hpp"
#include "InnerHandle.hpp"

namespace WCDB {

CipherConfig::CipherConfig(const UnsafeData& cipher, int pageSize, int cipherVersion)
: Config(), m_key(cipher), m_pageSize(pageSize), m_cipherVersion(cipherVersion)
{
}

CipherConfig::~CipherConfig() = default;

bool CipherConfig::invoke(InnerHandle* handle)
{
    bool ret = false;
    {
        SharedLockGuard lockGuard(m_lock);
        if (!m_rawKey.empty()) {
            ret = handle->setCipherKey(m_rawKey);
        } else {
            ret = handle->setCipherKey(m_key);
        }
    }

    if (ret && m_cipherVersion != 0) {
        ret = handle->execute(
        StatementPragma().pragma(Pragma::cipherCompatibility()).to(m_cipherVersion));
    }
    if (ret) {
        ret = handle->setCipherPageSize(m_pageSize);
    }
    return ret;
}

void CipherConfig::trySaveRawKey(InnerHandle* handle)
{
    {
        SharedLockGuard lockGuard(m_lock);
        if (!m_rawKey.empty()) {
            return;
        }
    }
    LockGuard lockGuard(m_lock);
    if (!m_rawKey.empty()) {
        return;
    }
    m_rawKey = handle->getRawCipherKey();
    if (!m_rawKey.empty()) {
        m_key = Data();
    }
}

} //namespace WCDB
