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
#include <WCDB/Data.hpp>
#include <WCDB/Material.hpp>
#include <WCDB/Serialization.hpp>

namespace WCDB {

namespace Repair {

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
    Data encoded;
    Serialization encoder;
    for (const auto &element : contents) {
        if (element.first.empty()) {
            markAsEmpty("TableName");
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

void Material::markAsEmpty(const std::string &element)
{
    Error error;
    error.setCode(Error::Code::Empty, "Repair");
    error.message = "Element of material is empty.";
    error.infos.set("Element", element);
    Notifier::shared()->notify(error);
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
    uint32_t magic = deserialization.advance4BytesUInt();
    uint32_t version = deserialization.advance4BytesUInt();
    if (magic != Material::magic) {
        markAsCorrupt("Magic");
        return false;
    }
    if (version != 0x01000000) {
        markAsCorrupt("Version");
        return false;
    }

    //Info
    if (!info.deserialize(deserialization)) {
        return false;
    }

    bool succeed;
    Data decompressed;
    std::tie(succeed, decompressed) = deserializeData(deserialization);
    if (!succeed) {
        return false;
    }

    Deserialization decoder(decompressed);
    while (!decoder.ended()) {
        size_t lengthOfSizedString;
        std::string tableName;
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

std::pair<bool, Data> Material::deserializeData(Deserialization &deserialization)
{
    bool succeed = false;
    Data data;
    do {
        if (!deserialization.canAdvance(sizeof(uint32_t))) {
            markAsCorrupt("Checksum");
            break;
        }
        uint32_t checksum = deserialization.advance4BytesUInt();
        auto intermediate = deserialization.advanceSizedData();
        if (intermediate.first == 0) {
            markAsCorrupt("Content");
            break;
        }
        if (!intermediate.second.empty()) {
            data = intermediate.second;
            if (checksum != data.hash()) {
                markAsCorrupt("Checksum");
                break;
            }
        } else if (checksum != 0) {
            markAsCorrupt("Checksum");
            break;
        }
        succeed = true;
    } while (false);
    return { succeed, succeed ? data : Data::emptyData() };
}

void Material::markAsCorrupt(const std::string &element)
{
    Error error;
    error.setCode(Error::Code::Corrupt, "Repair");
    error.message = "Material is corrupted";
    error.infos.set("Element", element);
    Notifier::shared()->notify(error);
    setThreadedError(std::move(error));
}

#pragma mark - Info
Material::Info::Info()
: pageSize(0), walFrame(0), walSalt({ 0, 0 }), reservedBytes(0)
{
    static_assert(size == 20, "");
}

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
    serialization.put4BytesUInt(walFrame);
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
    walFrame = deserialization.advance4BytesUInt();
    return true;
}

#pragma mark - Content
Material::Content::Content() : sequence(0)
{
}

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
    int associatedSQLCount = (int) varint;
    associatedSQLs.clear();
    for (int i = 0; i < associatedSQLCount; ++i) {
        std::string sql;
        std::tie(lengthOfSizedString, sql) = deserialization.advanceSizedString();
        if (lengthOfSizedString == 0 || sql.empty()) {
            markAsCorrupt("SQLs");
            return false;
        }
        associatedSQLs.push_back(std::move(sql));
    }

    std::tie(lengthOfVarint, varint) = deserialization.advanceVarint();
    if (lengthOfVarint == 0) {
        markAsCorrupt("PageCount");
        return false;
    }
    int pageCount = (int) varint;
    if (pageCount == 0) {
        markAsCorrupt("PageCount");
        return false;
    }
    verifiedPagenos.clear();
    for (int i = 0; i < pageCount; ++i) {
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
