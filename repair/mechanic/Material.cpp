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

#pragma mark - Initialization
Material::Material()
{
    memset(&info, 0, sizeof(info));
}

Material::Content::Content() : sequence(0)
{
}

#pragma mark - Serialization
void Material::markAsCorrupt()
{
    Error error;
    error.setCode(Error::Code::Corrupt, "Repair");
    error.message = "Material is corrupted";
    setThreadedError(std::move(error));
}

bool Material::deserialize(const Data &data)
{
    Deserialization deserialization(data);

    //Header
    {
        if (!deserialization.isEnough(Material::HeaderSize)) {
            markAsCorrupt();
            return false;
        }
        uint32_t magic = deserialization.advance4BytesUInt();
        if (magic != Material::magic) {
            markAsCorrupt();
            return false;
        }
        uint32_t version = deserialization.advance4BytesUInt();
        switch (version) {
            case 0x01000000:
                break;
            default:
                markAsCorrupt();
                return false;
        }
    }

    Deserialization metasDecoder;
    Deserialization contentsDecoder;

    //Metas
    {
        auto intermediate = deserializeBLOB(deserialization);
        if (!intermediate.first) {
            markAsCorrupt();
            return false;
        }
        metasDecoder.reset(intermediate.second);
    }

    //Info
    {
        if (!deserialization.isEnough(sizeof(info))) {
            markAsCorrupt();
            return false;
        }
        info.pageSize = deserialization.advance4BytesUInt();
        info.reservedBytes = deserialization.advance4BytesUInt();
        info.walSalt1 = deserialization.advance4BytesUInt();
        info.walSalt2 = deserialization.advance4BytesUInt();
        info.walFrame = deserialization.advance4BytesUInt();
    }

    //Contents
    {
        auto intermediate = deserializeBLOB(deserialization);
        if (!intermediate.first) {
            markAsCorrupt();
            return false;
        }
        contentsDecoder.reset(intermediate.second);
    }

    Content content;
    size_t lengthOfVarint;
    uint64_t varint;
    while (!metasDecoder.ended() || !contentsDecoder.ended()) {
        std::string tableName = metasDecoder.advanceZeroTerminatedString();
        if (tableName.empty()) {
            markAsCorrupt();
            return false;
        }

        std::tie(lengthOfVarint, varint) = metasDecoder.advanceVarint();
        if (lengthOfVarint == 0) {
            markAsCorrupt();
            return false;
        }
        content.sequence = (int64_t) varint;

        std::string sql = contentsDecoder.advanceZeroTerminatedString();
        if (sql.empty()) {
            markAsCorrupt();
            return false;
        }
        content.sql = std::move(sql);

        uint32_t pageCount;
        if (!contentsDecoder.isEnough(sizeof(pageCount))) {
            markAsCorrupt();
            return false;
        }
        pageCount = contentsDecoder.advance4BytesUInt();
        std::vector<uint32_t> pagenos;
        pagenos.reserve(pageCount);
        for (uint32_t i = 0; i < pageCount; ++i) {
            std::tie(lengthOfVarint, varint) = contentsDecoder.advanceVarint();
            if (lengthOfVarint == 0) {
                markAsCorrupt();
                return false;
            }
            pagenos.push_back((uint32_t) varint);
        }
        content.pagenos = std::move(pagenos);

        contents[std::move(tableName)] = std::move(content);
    }

    if (!metasDecoder.ended() && !contentsDecoder.ended()) {
        markAsCorrupt();
        return false;
    }
    return true;
}

Data Material::serialize() const
{
    Serialization serialization;

    //Header
    {
        if (!serialization.resizeToFit(Material::HeaderSize)) {
            return Data::emptyData();
        }
        serialization.put4BytesUInt(magic);
        serialization.put4BytesUInt(version);
    }

    //Metas
    {
        Data encoded;
        Serialization encoder;
        for (const auto &element : contents) {
            if (!encoder.putZeroTerminatedString(element.first) ||
                !encoder.putVarint(element.second.sequence)) {
                return Data::emptyData();
            }
        }
        if (!serializeBLOB(serialization, encoder.finalize())) {
            return Data::emptyData();
        }
    }

    //Info
    {
        if (!serialization.resizeToFit(sizeof(info))) {
            return Data::emptyData();
        }
        serialization.put4BytesUInt(info.pageSize);
        serialization.put4BytesUInt(info.reservedBytes);
        serialization.put4BytesUInt(info.walSalt1);
        serialization.put4BytesUInt(info.walSalt2);
        serialization.put4BytesUInt(info.walFrame);
    }

    //Contents
    {
        Data encoded;
        Serialization encoder;
        for (const auto &element : contents) {
            if (!encoder.putZeroTerminatedString(element.second.sql) ||
                !encoder.put4BytesUInt(
                    (uint32_t) element.second.pagenos.size())) {
                return Data::emptyData();
            }
            for (const auto pageno : element.second.pagenos) {
                if (!encoder.putVarint(pageno)) {
                    return Data::emptyData();
                }
            }
        }
        if (!serializeBLOB(serialization, encoder.finalize())) {
            return Data::emptyData();
        }
    }

    return serialization.finalize();
}

bool Material::serializeBLOB(Serialization &serialization,
                             const Data &data) const
{
    Data compressed;
    if (!data.empty()) {
        compressed = compress(data);
        if (compressed.empty()) {
            return false;
        }
    }
    uint32_t checksum = compressed.empty() ? 0 : hash(compressed);
    uint32_t size = (uint32_t) compressed.size();
    if (!serialization.resizeToFit(sizeof(checksum) + sizeof(size) + size)) {
        return false;
    }
    serialization.put4BytesUInt(checksum);
    serialization.put4BytesUInt(size);
    if (!compressed.empty()) {
        serialization.putBLOB(compressed);
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

} //namespace Repair

} //namespace WCDB
