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
#include <WCDB/Compression.hpp>
#include <WCDB/Data.hpp>
#include <WCDB/Material.hpp>
#include <WCDB/Serialization.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Material
Material::Material()
{
    memset(&info, 0, sizeof(info));
}

#pragma mark - Serialization
bool Material::serialize(Serialization &serialization) const
{
    //Header
    if (!serialization.resizeToFit(Material::headerSize)) {
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
            markAsEmpty();
            return false;
        }
        if (!element.second.serialize(encoder)) {
            return false;
        }
    }
    if (!serializeBLOB(serialization, encoder.finalize())) {
        return false;
    }

    return true;
}

bool Material::serializeBLOB(Serialization &serialization, const Data &data)
{
    Data compressed;
    if (!data.empty()) {
        compressed = compress(data);
        if (compressed.empty()) {
            return false;
        }
    }
    uint32_t size = (uint32_t) compressed.size();
    uint32_t checksum = compressed.empty() ? 0 : hash(compressed);
    if (!serialization.resizeToFit(sizeof(checksum) + sizeof(size) + size)) {
        return false;
    }
    serialization.put4BytesUInt(size);
    serialization.put4BytesUInt(checksum);
    if (!compressed.empty()) {
        serialization.putBLOB(compressed);
    }
    return true;
}

void Material::markAsEmpty()
{
    Error error;
    error.setCode(Error::Code::Empty, "Repair");
    error.message = "Element of material is empty.";
    Notifier::shared()->notify(error);
    setThreadedError(std::move(error));
}

#pragma mark - Deserialization
bool Material::deserialize(Deserialization &deserialization)
{
    //Header
    if (!deserialization.isEnough(Material::headerSize)) {
        markAsCorrupt();
        return false;
    }
    uint32_t magic = deserialization.advance4BytesUInt();
    uint32_t version = deserialization.advance4BytesUInt();
    if (magic != Material::magic || version != 0x01000000) {
        markAsCorrupt();
        return false;
    }

    //Info
    if (!info.deserialize(deserialization)) {
        markAsCorrupt();
        return false;
    }

    bool succeed;
    Data decompressed;
    std::tie(succeed, decompressed) = deserializeBLOB(deserialization);
    if (!succeed) {
        return false;
    }

    Deserialization decoder(decompressed);
    while (!decoder.ended()) {
        std::string tableName = decoder.advanceZeroTerminatedString();
        if (tableName.empty()) {
            markAsCorrupt();
            return false;
        }

        Content content;
        if (!content.deserialize(deserialization)) {
            return false;
        }

        contents[std::move(tableName)] = std::move(content);
    }

    return true;
}

std::pair<bool, Data>
Material::deserializeBLOB(Deserialization &deserialization)
{
    uint32_t checksum = 0;
    uint32_t size = 0;
    if (!deserialization.isEnough(sizeof(checksum) + sizeof(size))) {
        return {false, Data::emptyData()};
    }
    checksum = deserialization.advance4BytesUInt();
    size = deserialization.advance4BytesUInt();
    if (size == 0) {
        if (checksum != 0) {
            markAsCorrupt();
            return {false, Data::emptyData()};
        }
        return {true, Data::emptyData()};
    }
    const unsigned char *blob = deserialization.advanceBLOB(size);
    if (blob == nullptr) {
        markAsCorrupt();
        return {false, Data::emptyData()};
    }
    Data compressed = Data::immutableNoCopyData(blob, size);
    if (checksum != hash(compressed)) {
        markAsCorrupt();
        return {false, Data::emptyData()};
    }
    Data decompressed = decompress(compressed);
    if (decompressed.empty()) {
        return {false, Data::emptyData()};
    }
    return {true, decompressed};
}

void Material::markAsCorrupt()
{
    Error error;
    error.setCode(Error::Code::Corrupt, "Repair");
    error.message = "Material is corrupted";
    Notifier::shared()->notify(error);
    setThreadedError(std::move(error));
}

#pragma mark - Info
static_assert(Material::Info::size == 28, "");

Material::Info::Info()
    : moment(0), pageSize(0), walFrame(0), walSalt({0, 0}), reservedBytes(0)
{
}

#pragma mark - Serialization
bool Material::Info::serialize(Serialization &serialization) const
{
    if (!serialization.resizeToFit(Info::size)) {
        return false;
    }
    serialization.put8BytesInt(moment);
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
    if (!deserialization.isEnough(Info::size)) {
        return false;
    }
    moment = deserialization.advance8BytesInt();
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
    if (sql.empty() || pagenos.empty()) {
        markAsEmpty();
        return false;
    }
    if (!serialization.putVarint(sequence) ||
        !serialization.putZeroTerminatedString(sql) ||
        !serialization.put4BytesUInt((uint32_t) pagenos.size())) {
        return false;
    }
    for (const auto pageno : pagenos) {
        if (!serialization.putVarint(pageno)) {
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
        Material::markAsCorrupt();
        return false;
    }
    sequence = (int64_t) varint;

    sql = deserialization.advanceZeroTerminatedString();
    if (sql.empty()) {
        Material::markAsCorrupt();
        return false;
    }

    if (!deserialization.isEnough(sizeof(uint32_t))) {
        Material::markAsCorrupt();
        return false;
    }
    uint32_t pageCount = deserialization.advance4BytesUInt();
    pagenos.clear();
    pagenos.reserve(pageCount);
    for (uint32_t i = 0; i < pageCount; ++i) {
        std::tie(lengthOfVarint, varint) = deserialization.advanceVarint();
        if (lengthOfVarint == 0) {
            Material::markAsCorrupt();
            return false;
        }
        pagenos.push_back((uint32_t) varint);
    }
    return true;
}

} // namespace Repair

} //namespace WCDB
