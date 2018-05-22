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
#include <WCDB/Materaial.hpp>
#include <WCDB/Serialization.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialization
Materaial::Materaial() : walSalt1(0), walSalt2(0), walFrame(0)
{
}

#pragma mark - Serialization
bool Materaial::initWithData(const Data &data)
{
    Deserialization deserialization(data);
    if (!deserialization.isEnough(24)) {
        return false;
    }
    //Header
    uint32_t magic = deserialization.advance4BytesUInt();
    if (magic != Materaial::magic) {
        return false;
    }
    uint32_t checksum = deserialization.advance4BytesUInt();

    walSalt1 = deserialization.advance4BytesUInt();
    walSalt2 = deserialization.advance4BytesUInt();
    walFrame = deserialization.advance4BytesUInt();

    Data decompressed = decompress(data.subdata(24, data.size() - 24));
    if (checksum == 0 && decompressed.empty()) {
        //empty content
        return true;
    }
    if (decompressed.empty() || checksum != hash(decompressed)) {
        return false;
    }
    deserialization.reset(decompressed);

    //Content
    Content content;
    std::pair<int, uint64_t> intermediate;
    while (!deserialization.ended()) {
        WCTInnerAssert(content.tableName.empty());
        WCTInnerAssert(content.sql.empty());
        WCTInnerAssert(content.associatedSQLs.empty());
        WCTInnerAssert(content.pagenos.empty());
        content.tableName = deserialization.advanceZeroTerminatedString();
        content.sql = deserialization.advanceZeroTerminatedString();
        intermediate = deserialization.advanceVarint();
        if (intermediate.first < 0) {
            return false;
        }
        for (int i = 0; i < intermediate.second; ++i) {
            std::string associatedSQL =
                deserialization.advanceZeroTerminatedString();
            content.associatedSQLs.push_back(std::move(associatedSQL));
        }
        intermediate = deserialization.advanceVarint();
        if (intermediate.first < 0) {
            return false;
        }
        int pageCount = (int) intermediate.second;
        content.pagenos.resize(pageCount);
        for (int i = 0; i < pageCount; ++i) {
            intermediate = deserialization.advanceVarint();
            if (intermediate.first < 0) {
                return false;
            }
            content.pagenos[i] = (uint32_t) intermediate.second;
        }
        contents.push_back(std::move(content));
    }
    return true;
}

Data Materaial::encodedData()
{
    Serialization serialization;
    uint32_t checksum = 0;

    if (!serialization.resizeToFit(24)) {
        goto WCDB_Repair_Materaial_EncodedData_End;
    }

    serialization.put4BytesUInt(magic);
    serialization.put4BytesUInt(checksum);
    serialization.put4BytesUInt(version);

    serialization.put4BytesUInt(walSalt1);
    serialization.put4BytesUInt(walSalt2);
    serialization.put4BytesUInt(walFrame);

    if (!contents.empty()) {
        for (const auto &content : contents) {
            if (!serialization.putZeroTerminatedString(
                    content.tableName) // with '\0'
                ||
                !serialization.putZeroTerminatedString(content.sql) // with '\0'
                || !serialization.putVarint(content.associatedSQLs.size())) {
                goto WCDB_Repair_Materaial_EncodedData_End;
            }
            for (const auto &sql : content.associatedSQLs) {
                if (serialization.putZeroTerminatedString(sql) // with '\0'
                    ) {
                    goto WCDB_Repair_Materaial_EncodedData_End;
                }
            }
            if (!serialization.putVarint(content.pagenos.size())) {
                goto WCDB_Repair_Materaial_EncodedData_End;
            }
            for (const auto &pageno : content.pagenos) {
                if (!serialization.putVarint(pageno)) {
                    goto WCDB_Repair_Materaial_EncodedData_End;
                }
            }
        }

        const Data &uncompressed = serialization.finalize();
        const Data uncompressedContent =
            uncompressed.subdata(24, uncompressed.size() - 24);
        checksum = hash(uncompressedContent);
        serialization.seek(4);
        serialization.put4BytesUInt(checksum);

        const Data compressedContent = compress(uncompressedContent);
        if (compressedContent.empty()) {
            goto WCDB_Repair_Materaial_EncodedData_End;
        }
        serialization.seek(24);
        if (!serialization.putBLOB(compressedContent)) {
            goto WCDB_Repair_Materaial_EncodedData_End;
        }
        serialization.seek(24 + compressedContent.size());
    }

WCDB_Repair_Materaial_EncodedData_End:
    return serialization.finalize();
}

} //namespace Repair

} //namespace WCDB
