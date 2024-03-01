//
// Created by sanhuazhang on 2018/05/25
//

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

#pragma once

#include "EncryptedSerialization.hpp"
#include "StringView.hpp"
#include "WCDBOptional.hpp"
#include <list>
#include <map>
#include <stdlib.h>
#include <vector>

namespace WCDB {

class Data;
class Serialization;
class Deserialization;

namespace Repair {

class Material final : public EncryptedSerializable,
                       public DecryptedDeserializable,
                       public CipherDelegateHolder {
#pragma mark - Serializable
public:
    bool serialize(Serialization &serialization) const override final;
    using Serializable::serialize;

    ~Material() override;

protected:
    static bool serializeData(Serialization &serialization, const Data &data);
    static void markAsEmpty(const UnsafeStringView &element);

#pragma mark - Deserializable
public:
    bool deserialize(Deserialization &deserialization) override final;
    using Deserializable::deserialize;

protected:
    static Optional<Data> deserializeData(Deserialization &deserialization);
    static void markAsCorrupt(const UnsafeStringView &element);
    void decryptFail(const UnsafeStringView &element) const override final;
    CipherDelegate *getCipherDelegate() const override;

#pragma mark - Header
protected:
    static constexpr const uint32_t magic = 0x57434442;
    static constexpr const uint32_t version = 0x01000001; //1.0.0.1
    static constexpr const uint8_t saltBytes = 16;
    static constexpr const int headerSize = sizeof(magic) + sizeof(version); //magic + version

#pragma mark - Info
public:
    static constexpr const uint32_t UnknownPageNo = UINT32_MAX;

    class Info final : public Serializable, public Deserializable {
    public:
        static constexpr const int size = sizeof(uint32_t) * 6;
        Info();
        ~Info() override;

        uint32_t pageSize;
        uint32_t reservedBytes;
        std::pair<uint32_t, uint32_t> walSalt;
        uint32_t nBackFill;
        uint32_t seqTableRootPage;
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
    typedef struct Page {
    public:
        uint32_t number;
        uint32_t hash;
        Page(uint32_t number, uint32_t hash);
        ~Page();
    } Page;
    typedef std::vector<Page> VerifiedPages;
    class Content final : public Serializable, public Deserializable {
    public:
        Content();
        ~Content() override;

        StringView tableName;
        StringView sql;
        std::list<StringView> associatedSQLs;
        uint32_t rootPage;
        int64_t sequence;
        VerifiedPages verifiedPagenos;
        bool checked; //It will not be saved to file
#pragma mark - Serializable
    public:
        bool serialize(Serialization &serialization) const override final;
#pragma mark - Deserializable
    public:
        bool deserialize(Deserialization &deserialization) override final;
    };

    std::list<Content> contentsList;
    StringViewMap<Content *> contentsMap;
};

} //namespace Repair

} //namespace WCDB
