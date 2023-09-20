//
// Created by qiuwenchen on 2022/11/28.
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

#include "StringView.hpp"
#include "WCDBError.hpp"

namespace WCDB {

namespace Repair {

class CipherDelegate {
public:
    virtual ~CipherDelegate() = 0;
    virtual bool isCipherDB() = 0;
    virtual const Error &getCipherError() const = 0;
    virtual bool openCipherInMemory() = 0;
    virtual void closeCipher() = 0;
    virtual void *getCipherContext() = 0;
    virtual size_t getCipherPageSize() = 0;
    virtual Optional<StringView> tryGetSaltFromDatabase(const UnsafeStringView &path) = 0;
    virtual StringView getCipherSalt() = 0;
    virtual bool setCipherSalt(const UnsafeStringView &salt) = 0;
    virtual bool switchCipherSalt(const UnsafeStringView &salt) = 0;
};

class CipherDelegateHolder {
public:
    CipherDelegateHolder();
    virtual ~CipherDelegateHolder() = 0;
    void setCipherDelegate(CipherDelegate *delegate);

protected:
    CipherDelegate *m_cipherDelegate;
};

} //namespace Repair

} //namespace WCDB
