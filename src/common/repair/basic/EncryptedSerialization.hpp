//
// Created by 陈秋文 on 2023/9/3.
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

#include "Cipher.hpp"
#include "Serialization.hpp"

namespace WCDB {

namespace Repair {

constexpr const uint8_t saltBytes = 16;

#pragma mark - Serializable
class EncryptedSerializable : public Serializable {
public:
    virtual ~EncryptedSerializable() override = 0;
    Data encryptedSerialize() const;
    bool encryptedSerialize(const UnsafeStringView &path) const;

protected:
    virtual CipherDelegate *getCipherDelegate() const = 0;
};

#pragma mark - Deserializable
class DecryptedDeserializable : public Deserializable {
public:
    virtual ~DecryptedDeserializable() override = 0;
    bool decryptedDeserialize(Data &rawData, bool reloadSalt);
    bool decryptedDeserialize(const UnsafeStringView &path, bool reloadSalt);
    virtual void decryptFail(const UnsafeStringView &element) const = 0;

protected:
    virtual CipherDelegate *getCipherDelegate() const = 0;
};

} //namespace Repair

} //namespace WCDB
