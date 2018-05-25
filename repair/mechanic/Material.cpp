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
    memset(&meta, 0, sizeof(meta));
}

#pragma mark - Serialization
void Material::markAsCorrupt()
{
    Error error;
    error.setCode(Error::Code::Corrupt, "Repair");
    error.message = "Material is corrupted";
    setThreadedError(std::move(error));
}

bool Material::initWithData(const Data &data)
{
    Deserialization deserialization(data);
    if (!deserialization.isEnough(Material::HeaderSize)) {
        markAsCorrupt();
        return false;
    }
    //Header
    uint32_t magic = deserialization.advance4BytesUInt();
    if (magic != Material::magic) {
        markAsCorrupt();
        return false;
    }
    uint32_t checksum = deserialization.advance4BytesUInt();

    meta.pageSize = deserialization.advance4BytesUInt();
    meta.reservedBytes = deserialization.advance4BytesUInt();

    meta.walSalt1 = deserialization.advance4BytesUInt();
    meta.walSalt2 = deserialization.advance4BytesUInt();
    meta.walFrame = deserialization.advance4BytesUInt();

    Data decompressed = decompress(
        data.subdata(Material::HeaderSize, data.size() - Material::HeaderSize));
    if (decompressed.empty()) {
        return checksum == 0;
    }
    if (checksum != hash(decompressed)) {
        markAsCorrupt();
        return false;
    }
    deserialization.reset(decompressed);

    //Content
    Content content;
    std::pair<int, uint64_t> intermediate;
    while (!deserialization.ended()) {
        WCTInnerAssert(content.tableName.empty());
        WCTInnerAssert(content.sql.empty());
        WCTInnerAssert(content.pagenos.empty());
        content.tableName = deserialization.advanceZeroTerminatedString();
        content.sql = deserialization.advanceZeroTerminatedString();

        intermediate = deserialization.advanceVarint();
        if (intermediate.first <= 0) {
            markAsCorrupt();
            return false;
        }
        int pageCount = (int) intermediate.second;
        content.pagenos.resize(pageCount);
        for (int i = 0; i < pageCount; ++i) {
            intermediate = deserialization.advanceVarint();
            if (intermediate.first <= 0) {
                markAsCorrupt();
                return false;
            }
            content.pagenos[i] = (uint32_t) intermediate.second;
        }

        contents.push_back(std::move(content));
    }
    return true;
}

Data Material::encodedData() const
{
    Serialization serialization;
    uint32_t checksum = 0;

    if (!serialization.resizeToFit(Material::HeaderSize)) {
        goto WCDB_Repair_Material_EncodedData_Failed;
    }

    serialization.put4BytesUInt(magic);
    serialization.put4BytesUInt(checksum);
    serialization.put4BytesUInt(version);

    serialization.put4BytesUInt(meta.pageSize);
    serialization.put4BytesUInt(meta.reservedBytes);
    serialization.put4BytesUInt(meta.walSalt1);
    serialization.put4BytesUInt(meta.walSalt2);
    serialization.put4BytesUInt(meta.walFrame);

    if (!contents.empty()) {
        for (const auto &content : contents) {
            if (!serialization.putZeroTerminatedString(
                    content.tableName) // with '\0'
                ||
                !serialization.putZeroTerminatedString(content.sql) // with '\0'
                ) {
                goto WCDB_Repair_Material_EncodedData_Failed;
            }

            if (!serialization.putVarint(content.pagenos.size())) {
                goto WCDB_Repair_Material_EncodedData_Failed;
            }
            for (const auto &pageno : content.pagenos) {
                if (!serialization.putVarint(pageno)) {
                    goto WCDB_Repair_Material_EncodedData_Failed;
                }
            }
        }

        const Data &uncompressed = serialization.finalize();
        const Data uncompressedContent = uncompressed.subdata(
            Material::HeaderSize, uncompressed.size() - Material::HeaderSize);
        checksum = hash(uncompressedContent);
        serialization.seek(4);
        serialization.put4BytesUInt(checksum);

        const Data compressedContent = compress(uncompressedContent);
        if (compressedContent.empty()) {
            goto WCDB_Repair_Material_EncodedData_Failed;
        }
        serialization.seek(Material::HeaderSize);
        if (!serialization.putBLOB(compressedContent)) {
            goto WCDB_Repair_Material_EncodedData_Failed;
        }
        serialization.seek(Material::HeaderSize + compressedContent.size());
    }

    return serialization.finalize();

WCDB_Repair_Material_EncodedData_Failed:
    return Data::emptyData();
}

} //namespace Repair

} //namespace WCDB
