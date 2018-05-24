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
#include <WCDB/Error.hpp>
#include <WCDB/ThreadedErrors.hpp>
#include <zlib.h>

namespace WCDB {

namespace Repair {

static void setThreadedError(int zCode, const char *msg)
{
    Error error;
    Error::Code code;
    WCTInnerAssert(zCode != Z_OK && zCode != Z_STREAM_END);
    switch (zCode) {
        case Z_MEM_ERROR:
            code = Error::Code::NoMemory;
            break;
        case Z_DATA_ERROR:
            code = Error::Code::Corrupt;
            break;
        case Z_STREAM_ERROR:
            code = Error::Code::IOError;
            break;
        default:
            code = Error::Code::Error;
            break;
    }
    error.setCode(code, "Zlib");
    if (msg != nullptr) {
        error.message = msg;
    }
    error.infos.set("ExtCode", zCode);
    ThreadedErrors::shared()->setThreadedError(std::move(error));
}

Data compress(const Data &source)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    int step = getpagesize();
    int offset = 0;
    size_t total = 0;
    Data destination;

    int ret = deflateInit(&zs, Z_BEST_COMPRESSION);
    if (ret != Z_OK) {
        setThreadedError(ret, zs.msg);
        goto WCDB_Repair_Compress_End;
    }

    zs.next_in = reinterpret_cast<const Bytef *>(source.buffer());
    zs.avail_in = static_cast<uInt>(source.size());

    do {
        if (!destination.resize(step + offset)) {
            total = 0;
            goto WCDB_Repair_Compress_End;
        }
        zs.next_out = reinterpret_cast<Bytef *>(destination.buffer() + offset);
        zs.avail_out = step;

        ret = deflate(&zs, Z_FINISH);
        offset += step;
        total = zs.total_out;
    } while (ret == Z_OK);

    if (ret != Z_STREAM_END) {
        total = 0;
        setThreadedError(ret, zs.msg);
        goto WCDB_Repair_Compress_End;
    }

WCDB_Repair_Compress_End:
    deflateEnd(&zs);
    return total > 0 ? destination.subdata(total) : Data::emptyData();
    ;
}

Data decompress(const Data &source)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    int step = getpagesize();
    int offset = 0;
    size_t total = 0;
    Data destination;

    int ret = inflateInit(&zs);
    if (ret != Z_OK) {
        setThreadedError(ret, zs.msg);
        goto WCDB_Repair_Decompress_End;
    }

    zs.next_in = reinterpret_cast<const Bytef *>(source.buffer());
    zs.avail_in = static_cast<uInt>(source.size());

    do {
        if (!destination.resize(step + offset)) {
            total = 0;
            goto WCDB_Repair_Decompress_End;
        }
        zs.next_out = reinterpret_cast<Bytef *>(destination.buffer() + offset);
        zs.avail_out = step;

        ret = inflate(&zs, Z_FINISH);
        offset += step;
        total = zs.total_out;
    } while (ret == Z_OK);

    if (ret != Z_STREAM_END) {
        total = 0;
        setThreadedError(ret, zs.msg);
        goto WCDB_Repair_Decompress_End;
    }

WCDB_Repair_Decompress_End:
    inflateEnd(&zs);
    return total > 0 ? destination.subdata(total) : Data::emptyData();
}

uint32_t hash(const Data &source)
{
    return (uint32_t) crc32(0, source.buffer(), (uint32_t) source.size());
}

} //namespace Repair

} //namespace WCDB
