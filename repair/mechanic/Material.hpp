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

#include <WCDB/Serialization.hpp>
#include <WCDB/String.hpp>
#include <list>
#include <map>
#include <stdlib.h>
#include <vector>

namespace WCDB {

class Data;
class Serialization;
class Deserialization;

namespace Repair {

class Material final : public Serializable, public Deserializable {
#pragma mark - Serializable
public:
    bool serialize(Serialization &serialization) const override final;
    using Serializable::serialize;

protected:
    static bool serializeData(Serialization &serialization, const Data &data);
    static void markAsEmpty(const String &element);

#pragma mark - Deserializable
public:
    bool deserialize(Deserialization &deserialization) override final;
    using Deserializable::deserialize;

protected:
    static std::pair<bool, Data> deserializeData(Deserialization &deserialization);
    static void markAsCorrupt(const String &element);

#pragma mark - Header
protected:
    static constexpr const uint32_t magic = 0x57434442;
    static constexpr const uint32_t version = 0x01000000; //1.0.0.0
    static constexpr const int headerSize = sizeof(magic) + sizeof(version); //magic + version

#pragma mark - Info
public:
    class Info final : public Serializable, public Deserializable {
    public:
        static constexpr const int size = sizeof(uint32_t) * 5;
        Info();
        uint32_t pageSize;
        uint32_t reservedBytes;
        std::pair<uint32_t, uint32_t> walSalt;
        uint32_t walFrame;
#pragma mark - Serializable
    public:
        bool serialize(Serialization &serialization) const override final;
#pragma mark - Deserializable
    public:
        bool deserialize(Deserialization &deserialization) override final;
    };

    Info info;

#pragma mark - Content
public:
    class Content final : public Serializable, public Deserializable {
    public:
        Content();
        String sql;
        std::list<String> associatedSQLs;
        int64_t sequence;
        std::map<uint32_t, uint32_t> verifiedPagenos;
#pragma mark - Serializable
    public:
        bool serialize(Serialization &serialization) const override final;
#pragma mark - Deserializable
    public:
        bool deserialize(Deserialization &deserialization) override final;
    };

    std::map<String, Content> contents;
};

} //namespace Repair

} //namespace WCDB

#endif /* Material_hpp */
