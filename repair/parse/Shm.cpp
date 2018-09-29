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

#include <WCDB/Assertion.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/Serialization.hpp>
#include <WCDB/Shm.hpp>
#include <WCDB/String.hpp>
#include <WCDB/Wal.hpp>

namespace WCDB {

namespace Repair {

Shm::Shm(Wal *wal)
: WalRelated(wal), m_fileHandle(Path::addExtention(getPagerPath(), "-shm"))
{
    static_assert(sizeof(Header) == 48, "");
    static_assert(sizeof(CheckpointInfo) == 40, "");
    memset(&m_checkpointInfo, 0, sizeof(m_checkpointInfo));
}

const std::string &Shm::getPath() const
{
    return m_fileHandle.path;
}

uint32_t Shm::getMaxFrame() const
{
    WCTInnerAssert(isInitialized());
    return m_header.maxFrame;
}

uint32_t Shm::getBackfill() const
{
    WCTInnerAssert(isInitialized());
    return m_checkpointInfo.backfill;
}

void Shm::markAsCorrupted(const std::string &message)
{
    Error error;
    error.setCode(Error::Code::Corrupt, "Repair");
    error.message = message;
    error.infos.set("Path", getPath());
    Notifier::shared()->notify(error);
    setError(std::move(error));
}

bool Shm::doInitialize()
{
    WCTInnerAssert(m_wal->isInitialized() || m_wal->isInitializing());

    bool succeed;
    size_t fileSize;
    std::tie(succeed, fileSize) = FileManager::getFileSize(getPath());
    if (fileSize == 0) {
        if (!succeed) {
            assignWithSharedThreadedError();
        }
        return succeed;
    }

    if (!m_fileHandle.open(FileHandle::Mode::ReadOnly)) {
        assignWithSharedThreadedError();
        return false;
    }
    FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(getPath());

    if (fileSize < sizeof(Header)) {
        markAsCorrupted(String::formatted("File size: %lu is not enough for header.", fileSize));
        return false;
    }

    constexpr const int size = sizeof(Header) * 2 + sizeof(CheckpointInfo);
    MappedData data = m_fileHandle.map(0, size);
    if (data.size() != size) {
        if (data.size() > 0) {
            markAsCorrupted(String::formatted("Acquired shm data with size: %d is less than the expected size: %d.",
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
        String::formatted("Shm version: %u is illegal.", m_header.version));
        return false;
    }
    memcpy(&m_checkpointInfo, data.buffer() + sizeof(Header) * 2, sizeof(CheckpointInfo));
    return true;
}

void Shm::hint() const
{
    if (!isInitialized()) {
        return;
    }
    Error error;
    error.level = Error::Level::Notice;
    error.setCode(Error::Code::Notice, "Repair");
    error.message = "Shm hint.";
    error.infos.set("Backfill", m_checkpointInfo.backfill);
    error.infos.set("MaxFrame", m_header.maxFrame);
    Notifier::shared()->notify(error);
}

Shm::Header::Header()
: version(3007000), maxFrame(std::numeric_limits<decltype(maxFrame)>::max())
{
}

} // namespace Repair

} // namespace WCDB
