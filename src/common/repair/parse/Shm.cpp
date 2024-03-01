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

#include "Shm.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "FileManager.hpp"
#include "Notifier.hpp"
#include "Path.hpp"
#include "Serialization.hpp"
#include "StringView.hpp"
#include "Wal.hpp"
#include <cstring>

namespace WCDB {

namespace Repair {

Shm::Shm(Wal *wal)
: WalRelated(wal), m_fileHandle(Path::addExtention(getPagerPath(), "-shm"))
{
    static_assert(sizeof(Header) == 48, "");
    static_assert(sizeof(CheckpointInfo) == 40, "");
    memset(&m_checkpointInfo, 0, sizeof(m_checkpointInfo));
}

Shm::~Shm() = default;

const StringView &Shm::getPath() const
{
    return m_fileHandle.path;
}

uint32_t Shm::getMaxFrame() const
{
    WCTAssert(isInitialized());
    return m_header.maxFrame;
}

uint32_t Shm::getBackfill() const
{
    WCTAssert(isInitialized());
    return m_checkpointInfo.backfill;
}

Shm::Salt Shm::getSalt() const
{
    Salt salt;
    const unsigned char *p = (const unsigned char *) &(m_header.___salt[0]);
    salt.first = (((uint32_t) p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
    p = (const unsigned char *) &(m_header.___salt[1]);
    salt.second = (((uint32_t) p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
    return salt;
}

void Shm::markAsCorrupted(const UnsafeStringView &message)
{
    Error error(Error::Code::Corrupt, Error::Level::Notice, message);
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, getPagerPath());
    Notifier::shared().notify(error);
    setError(std::move(error));
}

bool Shm::doInitialize()
{
    WCTAssert(m_wal->isInitialized() || m_wal->isInitializing());

    auto optionalFileSize = FileManager::getFileSize(getPath());
    if (!optionalFileSize.succeed()) {
        assignWithSharedThreadedError();
        return false;
    }
    size_t fileSize = optionalFileSize.value();
    if (fileSize == 0) {
        return true;
    }

    if (!m_fileHandle.open(FileHandle::Mode::ReadOnly)) {
        assignWithSharedThreadedError();
        return false;
    }
    FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(getPath());

    if (fileSize < sizeof(Header)) {
        markAsCorrupted(StringView::formatted(
        "File size: %lu is not enough for header.", fileSize));
        return false;
    }

    constexpr const int size = sizeof(Header) * 2 + sizeof(CheckpointInfo);
    MappedData data = m_fileHandle.map(0, size);
    if (data.size() != size) {
        if (data.size() > 0) {
            markAsCorrupted(StringView::formatted("Acquired shm data with size: %d is less than the expected size: %d.",
                                                  data.size(),
                                                  size));
        } else {
            assignWithSharedThreadedError();
        }
        return false;
    }

    memcpy(&m_header, data.buffer(), sizeof(Header));
    if (m_header.version != 3007000) {
        markAsCorrupted(
        StringView::formatted("Shm version: %u is illegal.", m_header.version));
        return false;
    }
    memcpy(&m_checkpointInfo, data.buffer() + sizeof(Header) * 2, sizeof(CheckpointInfo));
    return true;
}

Shm::Header::Header()
: version(3007000), maxFrame(std::numeric_limits<decltype(maxFrame)>::max())
{
}

} // namespace Repair

} // namespace WCDB
