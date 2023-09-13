//
// Created by 陈秋文 on 2023/9/9.
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

#include "CipherHandle.hpp"
#include "Assertion.hpp"

namespace WCDB {

#pragma mark - Cipher

CipherHandle::CipherHandle() : InnerHandle()
{
}

CipherHandle::~CipherHandle() = default;

const Error &CipherHandle::getCipherError() const
{
    return InnerHandle::getError();
}

bool CipherHandle::openCipherInMemory()
{
    WCTAssert(getPath().equal(":memory:") || !isOpened());
    InnerHandle::setPath(":memory:");
    return InnerHandle::open();
}

void CipherHandle::closeCipher()
{
    InnerHandle::close();
}

bool CipherHandle::isCipherDB()
{
    if (!isOpened()) {
        openCipherInMemory();
    }
    return InnerHandle::getRawCipherKey().size() > 0;
}

void *CipherHandle::getCipherContext()
{
    if (!isOpened()) {
        return nullptr;
    }
    return AbstractHandle::getCipherContext();
}

size_t CipherHandle::getCipherPageSize()
{
    return AbstractHandle::getCipherPageSize();
}

StringView CipherHandle::getCipherSalt()
{
    return AbstractHandle::getCipherSalt();
}

bool CipherHandle::setCipherSalt(const UnsafeStringView &salt)
{
    return AbstractHandle::setCipherSalt(salt);
}

} // namespace WCDB
