//
// Created by 陈秋文 on 2023/9/2.
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
#include "Page.hpp"
#include "StringView.hpp"
#include <stdlib.h>
#include <unordered_map>
#include <vector>

namespace WCDB {

class Data;
class Serialization;
class Deserialization;

namespace Repair {

class IncrementalMaterial final : public EncryptedSerializable,
                                  public DecryptedDeserializable,
                                  public CipherDelegateHolder {
#pragma mark - Serializable
public:
    bool serialize(Serialization &serialization) const override final;
    using Serializable::serialize;

    ~IncrementalMaterial() override;

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
    static constexpr const uint32_t magic = 0x57434441;
    static constexpr const uint32_t version = 0x01000000; //1.0.0.0
    static constexpr const int headerSize = sizeof(magic) + sizeof(version); //magic + version

#pragma mark - Info
    class Info final : public Serializable, public Deserializable {
    public:
        static constexpr const int saltSize = sizeof(uint32_t) * 4;
        Info();
        ~Info() override;
        std::pair<uint32_t, uint32_t> lastWalSalt;
        std::pair<uint32_t, uint32_t> currentWalSalt;
        uint32_t lastNBackFill;
        uint32_t currentNBackFill;
        uint32_t lastSchemaCookie;
        bool lastCheckPointFinish;
        uint32_t lastBackupTime;
        int32_t incrementalBackupTimes;
#pragma mark - Serializable
    public:
        bool serialize(Serialization &serialization) const override final;
#pragma mark - Deserializable
    public:
        bool deserialize(Deserialization &deserialization) override final;
    };

public:
    Info info;

#pragma mark - Page
public:
    class Page final : public Serializable, public Deserializable {
    public:
        Page();
        ~Page() override;

        typedef Repair::Page::Type Type;

        uint32_t number;
        Type type;
        uint32_t hash;

#pragma mark - Serializable
    public:
        bool serialize(Serialization &serialization) const override final;
#pragma mark - Deserializable
    public:
        bool deserialize(Deserialization &deserialization) override final;
    };

    typedef std::unordered_map<uint32_t, Page> Pages;

    Pages pages;
};

} //namespace Repair

typedef std::shared_ptr<Repair::IncrementalMaterial> SharedIncrementalMaterial;

} //namespace WCDB
