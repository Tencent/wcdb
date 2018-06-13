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

#include <WCDB/Assertion.hpp>
#include <WCDB/CipherConfig.hpp>

namespace WCDB {

CipherConfig::CipherConfig(const Data &cipher, int pageSize)
    : Config(CipherConfig::name), m_key(cipher.copy()), m_pageSize(pageSize)
{
}

const Data &CipherConfig::getKey() const
{
    return m_key;
}

bool CipherConfig::invoke(Handle *handle)
{
    return handle->setCipherKey(m_key) &&
           handle->execute(StatementPragma()
                               .pragma(Pragma::cipherPageSize())
                               .to(m_pageSize));
}

} //namespace WCDB
