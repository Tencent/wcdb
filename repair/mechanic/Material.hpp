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

#ifndef Material_hpp
#define Material_hpp

#include <WCDB/SharedThreadedErrorProne.hpp>
#include <list>
#include <map>
#include <stdlib.h>
#include <string>
#include <vector>

namespace WCDB {

class Data;

namespace Repair {

class Serialization;
class Deserialization;

class Material : protected SharedThreadedErrorProne {
#pragma mark - Initialiazation
public:
    Material();

#pragma mark - Header
public:
    static constexpr const uint32_t magic = 0x57434442;
    static constexpr const uint32_t version = 0x01000000; //1.0.0.0
    static constexpr const int headerSize =
        sizeof(magic) + sizeof(version); //magic + version

#pragma mark - Content
public:
    struct Info {
        uint32_t pageSize;
        uint32_t reservedBytes;

        std::pair<uint32_t, uint32_t> walSalt;
        uint32_t walFrame;
    };

    Info info;

    class Content {
    public:
        Content();
        std::string sql;
        int64_t sequence;
        std::vector<uint32_t> pagenos;
    };
    std::map<std::string, Content> contents;

#pragma mark - Serialization
public:
    bool deserialize(const Data &data);
    Data serialize() const;

    bool deserialize(const std::string &path);
    bool serialize(const std::string &path) const;

    static std::pair<bool, std::map<std::string, int64_t>>
    acquireMetas(const std::string &path);

protected:
    static bool serializeBLOB(Serialization &serialization, const Data &data);
    static std::pair<bool, Data>
    deserializeBLOB(Deserialization &deserialization);
    static std::pair<bool, Data> deserializeUnwrappedBLOB(
        Deserialization &deserialization, uint32_t checksum, uint32_t size);
    static bool isHeaderSanity(Deserialization &deserialization);
    static void markAsCorrupt();
};

} //namespace Repair

} //namespace WCDB

#endif /* Material_hpp */
