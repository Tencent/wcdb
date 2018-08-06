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
#include <WCDB/Path.hpp>
#include <WCDB/Serialization.hpp>
#include <WCDB/Shm.hpp>
#include <WCDB/String.hpp>
#include <WCDB/Wal.hpp>

namespace WCDB {

namespace Repair {

Shm::Shm(Wal *wal)
: WalRelated(wal)
, m_fileHandle(Path::addExtention(getPagerPath(), "-shm"))
, m_maxFrame(std::numeric_limits<int>::max())
{
}

const std::string &Shm::getPath() const
{
    return m_fileHandle.path;
}

int Shm::getMaxFrame() const
{
    WCTInnerAssert(isInitialized());
    return m_maxFrame;
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

    if (fileSize < headerSize) {
        markAsCorrupted(String::formatted("File size: %lu is not enough for header.", fileSize));
        return false;
    }

    MappedData data = m_fileHandle.map(0, headerSize);
    if (data.size() != headerSize) {
        if (data.size() > 0) {
            markAsCorrupted(String::formatted("Acquired shm data with size: %d is less than the expected size: %d.",
                                              data.size(),
                                              headerSize));
        } else {
            assignWithSharedThreadedError();
        }
        return false;
    }

    Deserialization deserialization(data);
    WCTInnerAssert(deserialization.canAdvance(4));
    uint32_t version = deserialization.advance4BytesUInt();
#warning TODO check version
    deserialization.seek(16);
    WCTInnerAssert(deserialization.canAdvance(4));
    m_maxFrame = deserialization.advance4BytesUInt();
    return true;
}

} // namespace Repair

} // namespace WCDB
