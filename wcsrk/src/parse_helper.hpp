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

#ifndef parse_helper_hpp
#define parse_helper_hpp

#include <tuple>
#include <stdint.h>

class ParseHelper
{
public:
    static int32_t ParseInt32(const unsigned char* data, const size_t& size = sizeof(int32_t));
    static int64_t ParseInt64(const unsigned char* data, const size_t& size = sizeof(int64_t));
    static double ParseDouble(const unsigned char* data);
    static std::tuple<int64_t, int, bool> ParseVarint(const unsigned char* data);
    static int ParseSerialTypeSize(int serialType);
};

#endif /* parse_helper_hpp */
