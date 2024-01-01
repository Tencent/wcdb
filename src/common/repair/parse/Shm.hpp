//
// Created by sanhuazhang on 2018/08/06
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

#include "FileHandle.hpp"
#include "Initializeable.hpp"
#include "WalRelated.hpp"

namespace WCDB {

namespace Repair {

class Shm final : public WalRelated, public Initializeable {
public:
    Shm(Wal *wal);
    ~Shm() override;

    const StringView &getPath() const;

protected:
    bool doInitialize() override final;
    void markAsCorrupted(const UnsafeStringView &message);
    FileHandle m_fileHandle;

public:
    uint32_t getMaxFrame() const;
    uint32_t getBackfill() const;
    typedef std::pair<uint32_t, uint32_t> Salt;
    Salt getSalt() const;

protected:
    struct Header {
        Header();
        uint32_t version;
        uint32_t ___unused;
        uint32_t ___change;
        uint8_t ___isInit;
        uint8_t ___bigEndChecksum;
        uint16_t ___pageSize;
        uint32_t maxFrame;
        uint32_t ___page;
        uint32_t ___frameChecksum[2];
        uint32_t ___salt[2];
        uint32_t ___checksum[2];
    };
    typedef struct Header Header;

    struct CheckpointInfo {
        uint32_t backfill;
        uint32_t ___readMark[5];
        uint8_t ___lock[8];
        uint32_t ___backfillAttempted;
        uint32_t ___unused;
    };
    typedef struct CheckpointInfo CheckpointInfo;

    Header m_header;
    CheckpointInfo m_checkpointInfo;
};

} // namespace Repair

} // namespace WCDB
