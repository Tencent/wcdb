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
#include <WCDB/FileHandle.hpp>
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
    Notifier::shared()->notify(error);
    setThreadedError(std::move(error));
}

bool Material::deserialize(const Data &data)
{
    Deserialization deserialization(data);

    //Header
    {
        if (!isHeaderSanity(deserialization)) {
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
        if (!serialization.resizeToFit(Material::headerSize)) {
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
    return deserializeUnwrappedBLOB(deserialization, checksum, size);
}

std::pair<bool, Data> Material::deserializeUnwrappedBLOB(
    Deserialization &deserialization, uint32_t checksum, uint32_t size)
{
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

bool Material::isHeaderSanity(Deserialization &deserialization)
{
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
    return true;
}

bool Material::deserialize(const std::string &path)
{
    FileHandle fileHandle(path);
    if (!fileHandle.open(FileHandle::Mode::ReadOnly)) {
        return false;
    }
    bool succeed = false;
    do {
        ssize_t size = fileHandle.size();
        if (size == -1) {
            break;
        }
        Data data(size);
        if (data.empty()) {
            break;
        }
        ssize_t read = fileHandle.read(data.buffer(), 0, size);
        if (read != size) {
            if (read >= 0) {
                Error error;
                error.setCode(Error::Code::IOError, "Repair");
                error.message = "Short read";
                error.infos.set("Path", path);
                Notifier::shared()->notify(error);
                setThreadedError(std::move(error));
            }
            break;
        }
        succeed = deserialize(data);
    } while (false);
    fileHandle.close();
    return succeed;
}

bool Material::serialize(const std::string &path) const
{
    Data data = serialize();
    if (data.empty()) {
        return false;
    }
    //TODO cipher
    FileHandle fileHandle(path);
    if (!fileHandle.open(FileHandle::Mode::ReadWrite)) {
        return false;
    }
    ssize_t wrote = fileHandle.write(data.buffer(), 0, data.size());
    fileHandle.close();
    if (wrote != data.size()) {
        if (wrote >= 0) {
            Error error;
            error.setCode(Error::Code::IOError, "Repair");
            error.message = "Short write";
            error.infos.set("Path", path);
            Notifier::shared()->notify(error);
            setThreadedError(std::move(error));
        }
        return false;
    }
    return true;
}

std::pair<bool, std::map<std::string, int64_t>>
Material::acquireMetas(const std::string &path)
{
    FileHandle fileHandle(path);
    if (!fileHandle.open(FileHandle::Mode::ReadOnly)) {
        return {false, {}};
    }
    uint32_t size;
    uint32_t checksum;
    constexpr const size_t fixedLength =
        headerSize + sizeof(size) + sizeof(checksum);
    //Header + Meta Header
    {
        Data header = Data(fixedLength);
        if (header.empty()) {
            return {false, {}};
        }
        ssize_t read = fileHandle.read(header.buffer(), 0, fixedLength);
        if (read != fixedLength) {
            if (read >= 0) {
                markAsCorrupt();
            }
            return {false, {}};
        }
        Deserialization deserialization(header);
        if (!isHeaderSanity(deserialization)) {
            return {false, {}};
        }
        size = deserialization.advance4BytesUInt();
        checksum = deserialization.advance4BytesUInt();
    }
    //Metas
    Data compressed(size);
    {
        if (compressed.empty()) {
            return {false, {}};
        }
        ssize_t read = fileHandle.read(compressed.buffer(), fixedLength, size);
        if (read != size) {
            if (read >= 0) {
                markAsCorrupt();
            }
            return {false, {}};
        }
    }

    Data decompressed;
    {
        Deserialization deserialization(compressed);
        bool result;
        std::tie(result, decompressed) =
            deserializeUnwrappedBLOB(deserialization, checksum, size);
        if (!result) {
            return {false, {}};
        }
    }

    std::map<std::string, int64_t> sequences;
    {
        Deserialization decoder(decompressed);
        size_t lengthOfSequence;
        int64_t sequence;
        std::string tableName;
        while (!decoder.ended()) {
            tableName = decoder.advanceZeroTerminatedString();
            if (tableName.empty()) {
                markAsCorrupt();
                return {false, {}};
            }
            std::tie(lengthOfSequence, sequence) = decoder.advanceVarint();
            if (lengthOfSequence == 0) {
                markAsCorrupt();
                return {false, {}};
            }
            sequences[std::move(tableName)] = sequence;
        }
    }
    return {true, sequences};
}

} // namespace WCDB

} //namespace WCDB
