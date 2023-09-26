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

#include "IncrementalMaterial.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "Data.hpp"
#include "FileHandle.hpp"
#include "FileManager.hpp"
#include "Notifier.hpp"
#include "SQLite.h"
#include "Serialization.hpp"
#include "WCDBError.hpp"
#include <cstring>

namespace WCDB {

namespace Repair {

IncrementalMaterial::~IncrementalMaterial() = default;

#pragma mark - Serialization
bool IncrementalMaterial::serialize(Serialization &serialization) const
{
    //Header
    if (!serialization.expand(IncrementalMaterial::headerSize)) {
        return false;
    }
    serialization.put4BytesUInt(magic);
    serialization.put4BytesUInt(version);

    //Info
    if (!info.serialize(serialization)) {
        return false;
    }

    //Contents
    Serialization encoder;
    for (const auto &element : pages) {
        if (element.second.number == 0) {
            markAsEmpty("Page");
            return false;
        }
        if (!element.second.serialize(encoder)) {
            return false;
        }
    }
    return serializeData(serialization, encoder.finalize());
}

bool IncrementalMaterial::serializeData(Serialization &serialization, const Data &data)
{
    uint32_t checksum = data.empty() ? 0 : data.hash();
    return serialization.put4BytesUInt(checksum) && serialization.putSizedData(data);
}

void IncrementalMaterial::markAsEmpty(const UnsafeStringView &element)
{
    Error error(Error::Code::Empty, Error::Level::Error, "Element of IncrementalMaterial is empty.");
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign("Element", element);
    Notifier::shared().notify(error);
    setThreadedError(std::move(error));
}

#pragma mark - Deserialization
bool IncrementalMaterial::deserialize(Deserialization &deserialization)
{
    //Header
    if (!deserialization.canAdvance(IncrementalMaterial::headerSize)) {
        markAsCorrupt("Header");
        return false;
    }
    uint32_t magicValue = deserialization.advance4BytesUInt();
    uint32_t versionValue = deserialization.advance4BytesUInt();
    if (magicValue != IncrementalMaterial::magic) {
        markAsCorrupt("Magic");
        return false;
    }
    if (versionValue != 0x01000000) {
        markAsCorrupt("Version");
        return false;
    }
    deserialization.setDataVersion(versionValue);

    //Info
    if (!info.deserialize(deserialization)) {
        return false;
    }

    auto decompressed = deserializeData(deserialization);
    if (!decompressed.succeed()) {
        return false;
    }

    Deserialization decoder(decompressed.value());
    decoder.setDataVersion(deserialization.version());
    while (!decoder.ended()) {
        Page page;
        if (!page.deserialize(decoder)) {
            return false;
        }
        pages[page.number] = std::move(page);
    }
    return true;
}

Optional<Data> IncrementalMaterial::deserializeData(Deserialization &deserialization)
{
    if (!deserialization.canAdvance(sizeof(uint32_t))) {
        markAsCorrupt("Checksum");
        return NullOpt;
    }
    uint32_t checksum = deserialization.advance4BytesUInt();
    auto intermediate = deserialization.advanceSizedData();
    if (intermediate.first == 0) {
        markAsCorrupt("Content");
        return NullOpt;
    }
    Data data;
    if (!intermediate.second.empty()) {
        data = intermediate.second;
        if (checksum != data.hash()) {
            markAsCorrupt("Checksum");
            return NullOpt;
        }
    } else if (checksum != 0) {
        markAsCorrupt("Checksum");
        return NullOpt;
    }
    return data;
}

void IncrementalMaterial::decryptFail(const UnsafeStringView &element) const
{
    markAsCorrupt(element);
}

void IncrementalMaterial::markAsCorrupt(const UnsafeStringView &element)
{
    Error error(Error::Code::Corrupt, Error::Level::Notice, "IncrementalMaterial is corrupted");
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign("Element", element);
    Notifier::shared().notify(error);
    setThreadedError(std::move(error));
}

CipherDelegate *IncrementalMaterial::getCipherDelegate() const
{
    return m_cipherDelegate;
}

#pragma mark - Info
IncrementalMaterial::Info::Info()
: lastWalSalt({ 0, 0 })
, currentWalSalt({ 0, 0 })
, currentNBackFill(0)
, lastSchemaCookie(0)
, lastCheckPointFinish(false)
, lastBackupTime(0)
, incrementalBackupTimes(0)
{
    static_assert(saltSize == 16, "");
}

IncrementalMaterial::Info::~Info() = default;

#pragma mark - Serialization
bool IncrementalMaterial::Info::serialize(Serialization &serialization) const
{
    if (!serialization.expand(Info::saltSize)) {
        return false;
    }
    serialization.put4BytesUInt(lastWalSalt.first);
    serialization.put4BytesUInt(lastWalSalt.second);
    serialization.put4BytesUInt(currentWalSalt.first);
    serialization.put4BytesUInt(currentWalSalt.second);
    serialization.putVarint(lastNBackFill);
    serialization.putVarint(currentNBackFill);
    serialization.put4BytesUInt(lastSchemaCookie);
    serialization.putVarint(lastCheckPointFinish);
    serialization.put4BytesUInt(lastBackupTime);
    serialization.putVarint(incrementalBackupTimes);
    return true;
}

#pragma mark - Deserialization
bool IncrementalMaterial::Info::deserialize(Deserialization &deserialization)
{
    if (!deserialization.canAdvance(Info::saltSize)) {
        markAsCorrupt("Salt");
        return false;
    }
    lastWalSalt.first = deserialization.advance4BytesUInt();
    lastWalSalt.second = deserialization.advance4BytesUInt();
    currentWalSalt.first = deserialization.advance4BytesUInt();
    currentWalSalt.second = deserialization.advance4BytesUInt();

    auto varLastNBackfill = deserialization.advanceVarint();
    if (varLastNBackfill.first == 0) {
        markAsCorrupt("LastNBackfill");
        return false;
    }
    lastNBackFill = (uint32_t) varLastNBackfill.second;

    auto varCurrentNBackfill = deserialization.advanceVarint();
    if (varCurrentNBackfill.first == 0) {
        markAsCorrupt("MxFrame");
        return false;
    }
    currentNBackFill = (uint32_t) varCurrentNBackfill.second;

    if (!deserialization.canAdvance(4)) {
        markAsCorrupt("lastSchemaCookie");
    }
    lastSchemaCookie = deserialization.advance4BytesUInt();

    auto varLastCheckpointFinish = deserialization.advanceVarint();
    if (varLastCheckpointFinish.first == 0) {
        markAsCorrupt("LastCheckpointFinish");
        return false;
    }
    lastCheckPointFinish = (bool) varLastCheckpointFinish.second;

    if (!deserialization.canAdvance(4)) {
        markAsCorrupt("LastBackupTime");
    }
    lastBackupTime = deserialization.advance4BytesUInt();

    auto varBackupTimes = deserialization.advanceVarint();
    if (varBackupTimes.first == 0) {
        markAsCorrupt("BackupTimes");
        return false;
    }
    incrementalBackupTimes = (int32_t) varBackupTimes.second;

    return true;
}

#pragma mark - Page
IncrementalMaterial::Page::Page() : number(0), type(Type::Unknown), hash(0)
{
}

IncrementalMaterial::Page::~Page() = default;

#pragma mark - Serialization
bool IncrementalMaterial::Page::serialize(Serialization &serialization) const
{
    if (!serialization.putVarint(number)) {
        return false;
    }
    if (hash != 0) {
        return serialization.putVarint(2 * type + 1) && serialization.put4BytesUInt(hash);
    } else {
        return serialization.putVarint(2 * type);
    }
}

#pragma mark - Deserialization
bool IncrementalMaterial::Page::deserialize(Deserialization &deserialization)
{
    auto varPageNo = deserialization.advanceVarint();
    if (varPageNo.first == 0) {
        markAsCorrupt("PageNo");
        return false;
    }
    number = (uint32_t) varPageNo.second;

    auto varType = deserialization.advanceVarint();
    if (varType.first == 0) {
        markAsCorrupt("PageType");
        return false;
    }
    type = (Type) (varType.second / 2);

    if (varType.second % 2 == 1) {
        if (!deserialization.canAdvance(4)) {
            markAsCorrupt("PageHash");
            return false;
        }
        hash = deserialization.advance4BytesInt();
    }
    return true;
}

} // namespace Repair

} //namespace WCDB
