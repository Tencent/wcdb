//
// Created by 陈秋文 on 2023/9/3.
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

#include "EncryptedSerialization.hpp"
#include "Assertion.hpp"
#include "FileHandle.hpp"
#include "FileManager.hpp"
#include "Notifier.hpp"
#include "SQLite.h"
#include <string.h>

namespace WCDB {

namespace Repair {

#pragma mark - Serializable
EncryptedSerializable::~EncryptedSerializable() = default;

Data EncryptedSerializable::encryptedSerialize() const
{
    Data rawData = serialize();
    if (rawData.empty()) {
        return false;
    }

    CipherDelegate *cipherDelegate = getCipherDelegate();
    WCTAssert(cipherDelegate != nullptr);
    size_t pageSize = cipherDelegate->getCipherPageSize();
    if (pageSize == 0) {
        setThreadedError(std::move(cipherDelegate->getCipherError()));
        return false;
    }
    WCTAssert((pageSize & (pageSize - 1)) == 0);

    StringView salt = cipherDelegate->getCipherSalt();
    WCTAssert(salt.length() == saltBytes * 2);

    void *pCodec = cipherDelegate->getCipherContext();
    WCTAssert(pCodec != nullptr);

    int reserveBytes = sqlcipher_codec_ctx_get_reservesize(pCodec);
    WCTAssert(reserveBytes > 0);
    size_t usableSize = pageSize - reserveBytes;
    int pageCount = int((rawData.size() + saltBytes - 1) / usableSize) + 1;
    size_t totalSize = pageCount * pageSize;

    Data encryptData(totalSize);
    if (encryptData.size() != totalSize) {
        return false;
    }

    memset(encryptData.buffer(), 0, encryptData.size());
    unsigned char *encryptAddr = encryptData.buffer();
    unsigned char *dataAddr = rawData.buffer();

    for (int pageNo = 1; pageNo <= pageCount; pageNo++) {
        if (pageNo == 1) {
            size_t length = usableSize > rawData.size() + saltBytes ?
                            rawData.size() :
                            usableSize - saltBytes;
            memcpy(encryptAddr + saltBytes, dataAddr, length);
        } else {
            size_t length = pageNo * usableSize > rawData.size() + saltBytes ?
                            rawData.size() + saltBytes - (pageNo - 1) * usableSize :
                            usableSize;
            memcpy(encryptAddr, dataAddr, length);
        }

        unsigned char *pData
        = (unsigned char *) sqlite3Codec(pCodec, encryptAddr, pageNo, 6);
        WCTAssert(pData != nullptr);
        if (*pData == 0 && memcmp(pData, pData + 1, pageSize - 1) == 0) {
            Error error(Error::Code::Corrupt,
                        Error::Level::Warning,
                        StringView::formatted(
                        "fail to encrypt data at page %d, total page %d", pageNo, pageCount));
            Notifier::shared().notify(error);
            setThreadedError(std::move(error));
            return false;
        }
        memcpy(encryptAddr, pData, pageSize);
        encryptAddr += pageSize;
        if (pageNo == 1) {
            dataAddr += usableSize - saltBytes;
        } else {
            dataAddr += usableSize;
        }
    }
    StringView salt2 = StringView::hexString(encryptData.subdata(saltBytes));
    WCTAssert(salt2.compare(salt) == 0);
    return encryptData;
}

bool EncryptedSerializable::encryptedSerialize(const UnsafeStringView &path) const
{
    WCTAssert(path.length() > 0);

    Data encryptData = encryptedSerialize();
    FileHandle fileHandle(path);
    if (!fileHandle.open(FileHandle::Mode::OverWrite)) {
        return false;
    }
    bool succeed = fileHandle.write(encryptData);
    fileHandle.close();
    FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(path);
    return succeed;
};

#pragma mark - Deserializable
DecryptedDeserializable::~DecryptedDeserializable() = default;

bool DecryptedDeserializable::decryptedDeserialize(Data &rawData, bool reloadSalt)
{
    CipherDelegate *cipherDelegate = getCipherDelegate();
    WCTAssert(cipherDelegate != nullptr);

    size_t pageSize = cipherDelegate->getCipherPageSize();
    if (pageSize == 0) {
        setThreadedError(std::move(cipherDelegate->getCipherError()));
        return false;
    }
    WCTAssert((pageSize & (pageSize - 1)) == 0);
    if (rawData.size() % pageSize != 0) {
        decryptFail("Data");
        return false;
    }

    if (reloadSalt) {
        StringView salt = StringView::hexString(rawData.subdata(saltBytes));
        if (!cipherDelegate->switchCipherSalt(salt)) {
            setThreadedError(std::move(cipherDelegate->getCipherError()));
            return false;
        }
    }

    void *pCodec = cipherDelegate->getCipherContext();
    WCTAssert(pCodec != nullptr);

    int reserveBytes = sqlcipher_codec_ctx_get_reservesize(pCodec);
    WCTAssert(reserveBytes > 0);
    int pageCount = int(rawData.size() / pageSize);
    size_t usableSize = pageSize - reserveBytes;
    size_t totalSize = pageCount * usableSize - saltBytes;

    Data decryptData(totalSize);
    if (decryptData.size() != totalSize) {
        return false;
    }

    unsigned char *rawAdder = rawData.buffer();
    unsigned char *dataAdder = decryptData.buffer();
    for (int pageNo = 1; pageNo <= pageCount; pageNo++) {
        unsigned char *pData
        = (unsigned char *) sqlite3Codec(pCodec, rawAdder, pageNo, 4);
        if (pData == nullptr) {
            decryptFail(StringView::formatted("Page %d", pageNo));
            return false;
        }
        if (pageNo == 1) {
            memcpy(dataAdder, pData + saltBytes, usableSize - saltBytes);
            dataAdder += usableSize - saltBytes;
        } else {
            memcpy(dataAdder, pData, usableSize);
            dataAdder += usableSize;
        }
        rawAdder += pageSize;
    }
    if (!deserialize(decryptData)) {
        return false;
    }
    return true;
}

bool DecryptedDeserializable::decryptedDeserialize(const UnsafeStringView &path, bool reloadSalt)
{
    FileHandle fileHandle(path);
    if (!fileHandle.open(FileHandle::Mode::ReadOnly)) {
        return false;
    }

    Data rawData = fileHandle.mapOrReadAllData();
    if (rawData.empty()) {
        return false;
    }
    return decryptedDeserialize(rawData, reloadSalt);
}

} //namespace Repair

} //namespace WCDB
