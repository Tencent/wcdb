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
    Data decompressed = decompress(data);
    if (decompressed.empty()) {
        return false;
    }
    Deserialization deserialization(decompressed);
    if (!deserialization.isEnough(24)) {
        return false;
    }
    //Header
    uint32_t magic = deserialization.advance4BytesUInt();
    if (magic != Materaial::magic) {
        return false;
    }
    uint32_t checksum = deserialization.advance4BytesUInt();
    if (checksum != hash(decompressed.subdata(8, decompressed.size()))) {
        return false;
    }

    walSalt1 = deserialization.advance4BytesUInt();
    walSalt2 = deserialization.advance4BytesUInt();
    walFrame = deserialization.advance4BytesUInt();

    //Content
    Content content;
    std::pair<int, uint64_t> intermediate;
    while (!deserialization.ended()) {
        WCTInnerAssert(content.tableName.empty());
        WCTInnerAssert(content.sql.empty());
        WCTInnerAssert(content.associatedSQLs.empty());
        WCTInnerAssert(content.pagenos.empty());
        content.tableName = deserialization.advanceZeroTerminatedString();
        if (content.tableName.empty()) {
            return false;
        }
        content.sql = deserialization.advanceZeroTerminatedString();
        if (content.sql.empty()) {
            return false;
        }
        intermediate = deserialization.advanceVarint();
        if (intermediate.first < 0) {
            return false;
        }
        for (int i = 0; i < intermediate.second; ++i) {
            std::string associatedSQL =
                deserialization.advanceZeroTerminatedString();
            if (associatedSQL.empty()) {
                return false;
            }
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
    return false;
}

Data Materaial::encodedData()
{
    uint32_t checksum = 0;
    Data compressed;

    Serialization serialization;
    if (!serialization.resizeToFit(24)) {
        goto WCDB_Repair_Materaial_EncodedData_End;
    }

    serialization.put4BytesUInt(magic);
    serialization.put4BytesUInt(checksum);
    serialization.put4BytesUInt(version);
    serialization.put4BytesUInt(walSalt1);
    serialization.put4BytesUInt(walSalt2);
    serialization.put4BytesUInt(walFrame);

    for (const auto &content : contents) {
        if (!serialization.putZeroTerminatedString(
                content.tableName)                                 // with '\0'
            || !serialization.putZeroTerminatedString(content.sql) // with '\0'
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

    {
        const Data &finalData = serialization.finalize();
        WCTInnerAssert(!serialization.finalize().empty());

        checksum = hash(finalData.subdata(8, finalData.size() - 8));
        serialization.seek(4);
        serialization.put4BytesUInt(checksum);
    }

    compressed = compress(serialization.finalize());

WCDB_Repair_Materaial_EncodedData_End:
    return compressed;
}

} //namespace Repair

} //namespace WCDB
