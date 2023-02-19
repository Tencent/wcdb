//
// Created by sanhuazhang on 2018/05/25
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

#include <WCDB/Assertion.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/Data.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/FileHandle.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Material.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/SQLite.h>
#include <WCDB/Serialization.hpp>
#include <cstring>

namespace WCDB {

namespace Repair {

Material::~Material() = default;

#pragma mark - Serialization
bool Material::serialize(Serialization &serialization) const
{
    //Header
    if (!serialization.expand(Material::headerSize)) {
        return false;
    }
    serialization.put4BytesUInt(magic);
    serialization.put4BytesUInt(version);

    //Info
    if (!info.serialize(serialization)) {
        return false;
    }

    //Contents
    Serialization encoder;
    for (const auto &element : contents) {
        if (element.first.empty()) {
            markAsEmpty("TableName");
            return false;
        }
        if (element.second.sql.length() == 0) {
            markAsEmpty("SQL");
            return false;
        }
        if (element.second.verifiedPagenos.size() == 0) {
            markAsEmpty("Pageno");
            return false;
        }
        if (!encoder.putSizedString(element.first) || !element.second.serialize(encoder)) {
            return false;
        }
    }
    return serializeData(serialization, encoder.finalize());
}

bool Material::serializeData(Serialization &serialization, const Data &data)
{
    uint32_t checksum = data.empty() ? 0 : data.hash();
    return serialization.put4BytesUInt(checksum) && serialization.putSizedData(data);
}

bool Material::encryptedSerialize(const UnsafeStringView &path,
                                  const UnsafeStringView &salt) const
{
    WCTAssert(path.length() > 0 && salt.size() == saltBytes * 2);
    Data rawData = serialize();
    if (rawData.empty()) {
        return false;
    }

    WCTAssert(m_cipherDelegate != nullptr);
    m_cipherDelegate->closeCipher();
    if (!m_cipherDelegate->openCipherInMemory(true)) {
        setThreadedError(std::move(m_cipherDelegate->getCipherError()));
        return false;
    }

    size_t pageSize = m_cipherDelegate->getCipherPageSize();
    if (pageSize == 0) {
        setThreadedError(std::move(m_cipherDelegate->getCipherError()));
        return false;
    }
    WCTAssert((pageSize & (pageSize - 1)) == 0);

    if (!m_cipherDelegate->setCipherSalt(salt)) {
        setThreadedError(std::move(m_cipherDelegate->getCipherError()));
        return false;
    }

    void *pCodec = m_cipherDelegate->getCipherContext();
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
            Error error(Error::Code::Error,
                        Error::Level::Error,
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
    WCTAssert(salt.compare(StringView::hexString(encryptData.subdata(saltBytes))) == 0);
    FileHandle fileHandle(path);
    if (!fileHandle.open(FileHandle::Mode::OverWrite)) {
        return false;
    }
    bool succeed = fileHandle.write(0, encryptData);
    fileHandle.close();
    FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(path);
    return succeed;
}

void Material::markAsEmpty(const UnsafeStringView &element)
{
    Error error(Error::Code::Empty, Error::Level::Error, "Element of material is empty.");
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign("Element", element);
    Notifier::shared().notify(error);
    setThreadedError(std::move(error));
}

#pragma mark - Deserialization
bool Material::deserialize(Deserialization &deserialization)
{
    //Header
    if (!deserialization.canAdvance(Material::headerSize)) {
        markAsCorrupt("Header");
        return false;
    }
    uint32_t magicValue = deserialization.advance4BytesUInt();
    uint32_t versionValue = deserialization.advance4BytesUInt();
    if (magicValue != Material::magic) {
        markAsCorrupt("Magic");
        return false;
    }
    if (versionValue != 0x01000000) {
        markAsCorrupt("Version");
        return false;
    }

    //Info
    if (!info.deserialize(deserialization)) {
        return false;
    }

    auto decompressed = deserializeData(deserialization);
    if (!decompressed.succeed()) {
        return false;
    }

    Deserialization decoder(decompressed.value());
    while (!decoder.ended()) {
        size_t lengthOfSizedString;
        StringView tableName;
        std::tie(lengthOfSizedString, tableName) = decoder.advanceSizedString();
        if (lengthOfSizedString == 0 || tableName.empty()) {
            markAsCorrupt("TableName");
            return false;
        }

        Content content;
        if (!content.deserialize(decoder)) {
            return false;
        }
        contents[std::move(tableName)] = std::move(content);
    }
    return true;
}

Optional<Data> Material::deserializeData(Deserialization &deserialization)
{
    if (!deserialization.canAdvance(sizeof(uint32_t))) {
        markAsCorrupt("Checksum");
        return NullOpt;
    }
    uint32_t checksum = deserialization.advance4BytesUInt();
    auto intermediate = deserialization.advanceSizedData();
    if (intermediate.first == 0) {
        markAsCorrupt("Content");
        return NullOpt;
    }
    Data data;
    if (!intermediate.second.empty()) {
        data = intermediate.second;
        if (checksum != data.hash()) {
            markAsCorrupt("Checksum");
            return NullOpt;
        }
    } else if (checksum != 0) {
        markAsCorrupt("Checksum");
        return NullOpt;
    }
    return data;
}

bool Material::decryptedDeserialize(const UnsafeStringView &path)
{
    FileHandle fileHandle(path);
    if (!fileHandle.open(FileHandle::Mode::ReadOnly)) {
        return false;
    }

    Data rawData = fileHandle.mapOrReadAllData();
    if (rawData.empty()) {
        return false;
    }

    WCTAssert(m_cipherDelegate != nullptr);
    m_cipherDelegate->closeCipher();
    if (!m_cipherDelegate->openCipherInMemory(true)) {
        setThreadedError(std::move(m_cipherDelegate->getCipherError()));
        return false;
    }

    size_t pageSize = m_cipherDelegate->getCipherPageSize();
    if (pageSize == 0) {
        setThreadedError(std::move(m_cipherDelegate->getCipherError()));
        return false;
    }
    WCTAssert((pageSize & (pageSize - 1)) == 0);
    if (rawData.size() % pageSize != 0) {
        markAsCorrupt("Material");
        return false;
    }

    StringView salt = StringView::hexString(rawData.subdata(saltBytes));
    if (!m_cipherDelegate->setCipherSalt(salt)) {
        setThreadedError(std::move(m_cipherDelegate->getCipherError()));
        return false;
    }

    void *pCodec = m_cipherDelegate->getCipherContext();
    WCTAssert(pCodec != nullptr);

    int reserveBytes = sqlcipher_codec_ctx_get_reservesize(pCodec);
    WCTAssert(reserveBytes > 0);
    size_t usableSize = pageSize - reserveBytes;
    int pageCount = int(rawData.size() / usableSize);
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
            markAsCorrupt(StringView::formatted("Page %d", pageNo));
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
    info.cipherSalt = salt;
    return true;
}

void Material::markAsCorrupt(const UnsafeStringView &element)
{
    Error error(Error::Code::Corrupt, Error::Level::Notice, "Material is corrupted");
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign("Element", element);
    Notifier::shared().notify(error);
    setThreadedError(std::move(error));
}

#pragma mark - Info
Material::Info::Info()
: pageSize(0), reservedBytes(0), walSalt({ 0, 0 }), numberOfWalFrames(0)
{
    static_assert(size == 20, "");
}

Material::Info::~Info() = default;

#pragma mark - Serialization
bool Material::Info::serialize(Serialization &serialization) const
{
    if (!serialization.expand(Info::size)) {
        return false;
    }
    serialization.put4BytesUInt(pageSize);
    serialization.put4BytesUInt(reservedBytes);
    serialization.put4BytesUInt(walSalt.first);
    serialization.put4BytesUInt(walSalt.second);
    serialization.put4BytesUInt(numberOfWalFrames);
    return true;
}

#pragma mark - Deserialization
bool Material::Info::deserialize(Deserialization &deserialization)
{
    if (!deserialization.canAdvance(Info::size)) {
        markAsCorrupt("Info");
        return false;
    }
    pageSize = deserialization.advance4BytesUInt();
    reservedBytes = deserialization.advance4BytesUInt();
    walSalt.first = deserialization.advance4BytesUInt();
    walSalt.second = deserialization.advance4BytesUInt();
    numberOfWalFrames = deserialization.advance4BytesUInt();
    return true;
}

#pragma mark - Content
Material::Content::Content() : sequence(0)
{
}

Material::Content::~Content() = default;

#pragma mark - Serialization
bool Material::Content::serialize(Serialization &serialization) const
{
    if (!serialization.putVarint(sequence) || !serialization.putSizedString(sql)) {
        return false;
    }

    if (!serialization.putVarint(associatedSQLs.size())) {
        return false;
    }
    for (const auto &associatedSQL : associatedSQLs) {
        if (!serialization.putSizedString(associatedSQL)) {
            return false;
        }
    }

    if (!serialization.putVarint(verifiedPagenos.size())) {
        return false;
    }
    for (const auto &element : verifiedPagenos) {
        if (!serialization.putVarint(element.first)
            || !serialization.put4BytesUInt(element.second)) {
            return false;
        }
    }
    return true;
}

#pragma mark - Deserialization
bool Material::Content::deserialize(Deserialization &deserialization)
{
    size_t lengthOfVarint;
    uint64_t varint;
    std::tie(lengthOfVarint, varint) = deserialization.advanceVarint();
    if (lengthOfVarint == 0) {
        markAsCorrupt("Sequence");
        return false;
    }
    sequence = (int64_t) varint;

    size_t lengthOfSizedString;
    std::tie(lengthOfSizedString, sql) = deserialization.advanceSizedString();
    if (lengthOfSizedString == 0 || sql.empty()) {
        markAsCorrupt("SQL");
        return false;
    }

    std::tie(lengthOfVarint, varint) = deserialization.advanceVarint();
    if (lengthOfVarint == 0) {
        markAsCorrupt("SQLs");
        return false;
    }
    int numberOfAssociatedSQLs = (int) varint;
    associatedSQLs.clear();
    StringView buffer;
    for (int i = 0; i < numberOfAssociatedSQLs; ++i) {
        std::tie(lengthOfSizedString, buffer) = deserialization.advanceSizedString();
        if (lengthOfSizedString == 0 || buffer.empty()) {
            markAsCorrupt("SQLs");
            return false;
        }
        associatedSQLs.push_back(std::move(buffer));
    }

    std::tie(lengthOfVarint, varint) = deserialization.advanceVarint();
    if (lengthOfVarint == 0) {
        markAsCorrupt("NumberOfPages");
        return false;
    }
    int numberOfPages = (int) varint;
    if (numberOfPages == 0) {
        markAsCorrupt("NumberOfPages");
        return false;
    }
    verifiedPagenos.clear();
    for (int i = 0; i < numberOfPages; ++i) {
        std::tie(lengthOfVarint, varint) = deserialization.advanceVarint();
        if (lengthOfVarint == 0) {
            markAsCorrupt("Pageno");
            return false;
        }
        if (!deserialization.canAdvance(4)) {
            markAsCorrupt("PageChecksum");
            return false;
        }
        uint32_t checksum = deserialization.advance4BytesUInt();
        verifiedPagenos[(uint32_t) varint] = checksum;
    }
    return true;
}

} // namespace Repair

} //namespace WCDB
