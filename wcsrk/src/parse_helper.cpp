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

#include "parse_helper.hpp"

int32_t ParseHelper::ParseInt32(const unsigned char* data, const size_t& size)
{
    return (int32_t)ParseInt64(data, size);
}

int64_t ParseHelper::ParseInt64(const unsigned char* data, const size_t& size)
{
    int64_t out = 0;
    for (int i = 0; i < size; i++) {
        // All int values in SQLite is big-endian
        int shift = ((int)size-1-i)*8;
        out |= (data[i]<<shift);
    }
    return out;
}

double ParseHelper::ParseDouble(const unsigned char* data)
{
    unsigned char out[8];
    for (int i = 0; i < 8; i++) {
        // All float values in SQLite is big-endian with 8 lengths. For further informantion, see [Record Format] chapter at https://www.sqlite.org/fileformat2.html
        out[i] = data[8-1-i];
    }
    return *(double*)out;
}

std::tuple<int64_t, int, bool> ParseHelper::ParseVarint(const unsigned char* data)
{
    static constexpr const int varintMaxSize = 9;
    int64_t out = 0;
    int i = 0;
    while ((data[i]&0x80)&&i<varintMaxSize) {
        out |= (data[i]&0x7f);
        out = (out<<7);
        i++;
        if (i>=varintMaxSize) {
            return {false, false, false};
        }
    }
    out |= data[i];
    return {out, i+1, true};
}

int ParseHelper::ParseSerialTypeSize(int serialType)
{
    if (serialType<0) {
        return 0;
    }
    static int sqliterk_btree_serialtype_fixlengths[12] = {
        0, 1, 2, 3, 4, 6, 8, 8, 0, 0, 0, 0
    };
    if (serialType<12) {
        return sqliterk_btree_serialtype_fixlengths[serialType];
    }
    return (serialType-12-serialType%2)/2;
}
