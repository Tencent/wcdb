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
#include "FileManager.hpp"
#include "SQLite.h"
#include <string.h>

namespace WCDB {

#pragma mark - Cipher

CipherHandle::CipherHandle() : InnerHandle(), m_isInitializing(false)
{
}

CipherHandle::~CipherHandle() = default;

const Error &CipherHandle::getCipherError() const
{
    return InnerHandle::getError();
}

bool CipherHandle::execute(const Statement &statement)
{
    //Filter out encryption-irrelevant statements during the initialization phase
    if (m_isInitializing && !statement.getDescription().hasPrefix("PRAGMA cipher_")
        && !statement.getDescription().hasPrefix("PRAGMA kdf_iter")) {
        return true;
    } else {
        return InnerHandle::execute(statement);
    }
}

bool CipherHandle::setCipherKey(const UnsafeData &data)
{
    if (m_isInitializing && data.size() == 99) {
        UnsafeStringView cipher
        = UnsafeStringView((const char *) data.buffer(), data.size());
        if (cipher.hasPrefix("x'") && cipher.hasSuffix("'")) {
            // In this case, the cipher is made up of a 64-bit raw key and a 32-bit salt,
            // and we need to remove the salt part.
            // Please see: https://www.zetetic.net/sqlcipher/sqlcipher-api/#key
            void *buffer = malloc(67 * sizeof(unsigned char));
            if (buffer == nullptr) {
                notifyError(Error::Code::NoMemory, "", "Malloc memory for cipher fail");
                return false;
            }
            memcpy(buffer, data.buffer(), 66 * sizeof(unsigned char));
            ((char *) buffer)[66] = '\'';
            bool ret
            = InnerHandle::setCipherKey(UnsafeData((unsigned char *) buffer, 67));
            free(buffer);
            return ret;
        }
    }
    return InnerHandle::setCipherKey(data);
}

Optional<StringView> CipherHandle::tryGetSaltFromDatabase(const UnsafeStringView &path)
{
    Optional<StringView> result;
    auto size = FileManager::getFileSize(path);
    if (size.failed()) {
        return result;
    }
    int saltLength = 16;
    if (size.value() < saltLength) {
        return StringView();
    }
    FileHandle handle(path);
    if (!handle.open(FileHandle::Mode::ReadOnly)) {
        return result;
    }
    Data saltData = handle.read(saltLength);
    if (saltData.size() != saltLength) {
        return result;
    }
    if (strncmp("SQLite format 3\000", (const char *) saltData.buffer(), saltLength) == 0) {
        return StringView();
    }
    return StringView::hexString(saltData);
}

bool CipherHandle::openCipherInMemory()
{
    InnerHandle::setPath(":memory:");
    m_isInitializing = true;
    bool ret = InnerHandle::open();
    m_isInitializing = false;
    return ret;
}

void CipherHandle::closeCipher()
{
    InnerHandle::close();
}

bool CipherHandle::isCipherDB()
{
    WCTAssert(isOpened());
    return InnerHandle::hasCipher();
}

void *CipherHandle::getCipherContext()
{
    WCTAssert(isOpened());
    return AbstractHandle::getCipherContext();
}

size_t CipherHandle::getCipherPageSize()
{
    WCTAssert(isOpened());
    return AbstractHandle::getCipherPageSize();
}

StringView CipherHandle::getCipherSalt()
{
    WCTAssert(isOpened());
    return AbstractHandle::getCipherSalt();
}

bool CipherHandle::setCipherSalt(const UnsafeStringView &salt)
{
    WCTAssert(isOpened());
    return AbstractHandle::setCipherSalt(salt);
}

bool CipherHandle::switchCipherSalt(const UnsafeStringView &salt)
{
    closeCipher();
    if (!openCipherInMemory()) {
        return false;
    }
    return AbstractHandle::setCipherSalt(salt);
}

} // namespace WCDB
