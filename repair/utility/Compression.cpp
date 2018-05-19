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

#include <WCDB/Compression.hpp>
#include <WCDB/Data.hpp>
#include <zlib.h>

namespace WCDB {

namespace Repair {

Data compress(const Data &source)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    int step = getpagesize();
    int offset = 0;
    int ret = Z_OK;
    size_t total;
    Data destination;

    if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) {
        goto WCDB_Repair_Compress_Failed;
    }

    zs.next_in = reinterpret_cast<const Bytef *>(source.buffer());
    zs.avail_in = static_cast<uInt>(source.size());

    do {
        if (!destination.resize(step + offset)) {
            goto WCDB_Repair_Compress_Failed;
        }
        zs.next_out = reinterpret_cast<Bytef *>(destination.buffer() + offset);
        zs.avail_out = step;

        ret = deflate(&zs, Z_FINISH);
        offset += step;
        total = zs.total_out;
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        goto WCDB_Repair_Compress_Failed;
    }

    return destination.subdata(total);

WCDB_Repair_Compress_Failed:
    return Data::emptyData();
}

Data decompress(const Data &source)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    int step = getpagesize();
    int offset = 0;
    int ret = Z_OK;
    size_t total;
    Data destination;

    if (inflateInit(&zs) != Z_OK) {
        goto WCDB_Repair_Decompress_Failed;
    }

    zs.next_in = reinterpret_cast<const Bytef *>(source.buffer());
    zs.avail_in = static_cast<uInt>(source.size());

    do {
        if (!destination.resize(step + offset)) {
            goto WCDB_Repair_Decompress_Failed;
        }
        zs.next_out = reinterpret_cast<Bytef *>(destination.buffer() + offset);
        zs.avail_out = step;

        ret = inflate(&zs, Z_FINISH);
        offset += step;
        total = zs.total_out;
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        goto WCDB_Repair_Decompress_Failed;
    }

    return destination.subdata(total);

WCDB_Repair_Decompress_Failed:
    return Data::emptyData();
}

} //namespace Repair

} //namespace WCDB
