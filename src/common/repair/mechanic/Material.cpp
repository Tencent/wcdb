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

#include "Material.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "Data.hpp"
#include "FileHandle.hpp"
#include "FileManager.hpp"
#include "Notifier.hpp"
#include "SQLite.h"
#include "Serialization.hpp"
#include "WCDBError.hpp"
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
    for (const auto &element : contentsList) {
        if (element.tableName.empty()) {
            markAsEmpty("TableName");
            return false;
        }
        if (element.sql.length() == 0) {
            markAsEmpty("SQL");
            return false;
        }
        if (!element.serialize(encoder)) {
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
    if (versionValue != 0x01000000 && versionValue != version) {
        markAsCorrupt("Version");
        return false;
    }
    deserialization.setDataVersion(versionValue);

    //Info
    if (!info.deserialize(deserialization)) {
        return false;
    }

    auto decompressed = deserializeData(deserialization);
    if (!decompressed.succeed()) {
        return false;
    }

    Deserialization decoder(decompressed.value());
    decoder.setDataVersion(deserialization.version());
    while (!decoder.ended()) {
        contentsList.emplace_back();
        Content &content = contentsList.back();
        if (!content.deserialize(decoder)) {
            return false;
        }
        StringView tableName = content.tableName;
        contentsMap[tableName] = &content;
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

void Material::decryptFail(const UnsafeStringView &element) const
{
    markAsCorrupt(element);
}

void Material::markAsCorrupt(const UnsafeStringView &element)
{
    Error error(Error::Code::Corrupt, Error::Level::Notice, "Material is corrupted");
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign("Element", element);
    Notifier::shared().notify(error);
    setThreadedError(std::move(error));
}

CipherDelegate *Material::getCipherDelegate() const
{
    return m_cipherDelegate;
}

#pragma mark - Info
Material::Info::Info()
: pageSize(0), reservedBytes(0), walSalt({ 0, 0 }), nBackFill(0), seqTableRootPage(UnknownPageNo)
{
    static_assert(size == 24, "");
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
    serialization.put4BytesUInt(nBackFill);
    serialization.put4BytesUInt(seqTableRootPage);
    return true;
}

#pragma mark - Deserialization
bool Material::Info::deserialize(Deserialization &deserialization)
{
    if (!deserialization.canAdvance(Info::size - sizeof(uint32_t))) {
        markAsCorrupt("Info");
        return false;
    }
    pageSize = deserialization.advance4BytesUInt();
    reservedBytes = deserialization.advance4BytesUInt();
    walSalt.first = deserialization.advance4BytesUInt();
    walSalt.second = deserialization.advance4BytesUInt();
    nBackFill = deserialization.advance4BytesUInt();
    if (deserialization.version() >= 0x01000001) {
        if (!deserialization.canAdvance(sizeof(uint32_t))) {
            markAsCorrupt("SeqTableRootPage");
            return false;
        }
        seqTableRootPage = deserialization.advance4BytesUInt();
        WCTAssert(seqTableRootPage != UnknownPageNo);
    }
    return true;
}

#pragma mark - Content
Material::Page::Page(uint32_t n, uint32_t h) : number(n), hash(h){};

Material::Page::~Page() = default;

Material::Content::Content()
: rootPage(UnknownPageNo), sequence(0), checked(false)
{
}

Material::Content::~Content() = default;

#pragma mark - Serialization
bool Material::Content::serialize(Serialization &serialization) const
{
    if (!serialization.putSizedString(tableName) || !serialization.putVarint(rootPage)
        || !serialization.putVarint(sequence) || !serialization.putSizedString(sql)) {
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
    VerifiedPages *pages = const_cast<VerifiedPages *>(&verifiedPagenos);
    std::sort(pages->begin(), pages->end(), [](const Page &a, const Page &b) {
        return a.number < b.number;
    });
    uint32_t prePageNo = 0;
    for (int i = 0; i < verifiedPagenos.size(); i++) {
        const auto &page = verifiedPagenos[i];
        if (page.number == 0) {
            WCTAssert(prePageNo == 0);
            // Deleted pages
            continue;
        }
        if (prePageNo == 0) {
            if (!serialization.putVarint(verifiedPagenos.size() - i)) {
                return false;
            }
            if (!serialization.putVarint(page.number)) {
                return false;
            }
        } else {
            WCTAssert(page.number > prePageNo);
            if (!serialization.putVarint(page.number - prePageNo)) {
                return false;
            }
        }
        prePageNo = page.number;
        if (!serialization.put4BytesUInt(page.hash)) {
            return false;
        }
    }

    // No leaf page
    if (prePageNo == 0 && !serialization.putVarint(0)) {
        return false;
    }

    return true;
}

#pragma mark - Deserialization
bool Material::Content::deserialize(Deserialization &deserialization)
{
    size_t lengthOfSizedString;
    StringView table;
    std::tie(lengthOfSizedString, table) = deserialization.advanceSizedString();
    if (lengthOfSizedString == 0 || table.empty()) {
        markAsCorrupt("TableName");
        return false;
    }
    tableName = table;

    if (deserialization.version() >= 0x01000001) {
        size_t lengthOfPageNo;
        uint64_t pageNo;
        std::tie(lengthOfPageNo, pageNo) = deserialization.advanceVarint();
        if (lengthOfPageNo == 0) {
            markAsCorrupt("rootPage");
            return false;
        }
        rootPage = (uint32_t) pageNo;
        WCTAssert(rootPage != UnknownPageNo);
    }

    size_t lengthOfVarint;
    uint64_t varint;
    std::tie(lengthOfVarint, varint) = deserialization.advanceVarint();
    if (lengthOfVarint == 0) {
        markAsCorrupt("Sequence");
        return false;
    }
    sequence = (int64_t) varint;

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
    uint64_t prePageNo = 0;
    verifiedPagenos.reserve(numberOfPages);
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
        if (deserialization.version() >= 0x01000001) {
            verifiedPagenos.emplace_back((uint32_t) (varint + prePageNo), checksum);
            prePageNo += varint;
        } else {
            verifiedPagenos.emplace_back((uint32_t) varint, checksum);
        }
    }
    return true;
}

} // namespace Repair

} //namespace WCDB
