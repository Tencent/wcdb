//
// Created by sanhuazhang on 2018/06/27
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

#include "FileHandle.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "FileManager.hpp"
#include "Notifier.hpp"
#include <errno.h>
#include <fcntl.h>
#ifndef _WIN32
#include <sys/mman.h>
#else
#define NOMINMAX
#include <windows.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace WCDB {

#pragma mark - Initialize
FileHandle::FileHandle(const UnsafeStringView &path)
: path(path)
, m_fd(-1)
#ifdef _WIN32
, m_mapHandle(INVALID_HANDLE_VALUE)
#endif
, m_mode(Mode::None)
, m_errorIgnorable(false)
, m_fileSize(-1)
{
}

FileHandle::FileHandle(FileHandle &&other)
: path(std::move(other.path))
, m_fd(other.m_fd)
#ifdef _WIN32
, m_mapHandle(other.m_mapHandle)
#endif
, m_mode(other.m_mode)
, m_errorIgnorable(false)
, m_fileSize(other.m_fileSize)
{
    other.m_fd = -1;
    other.m_mode = Mode::None;
}

FileHandle::~FileHandle()
{
    WCTAssert(!isOpened() || m_mode != Mode::OverWrite);
    close();
}

FileHandle &FileHandle::operator=(FileHandle &&other)
{
    WCTAssert(path == other.path);
    m_fd = other.m_fd;
    other.m_fd = -1;
    other.m_mode = Mode::None;
    return *this;
}

#pragma mark - Basic
bool FileHandle::open(Mode mode)
{
    WCTAssert(!isOpened());
    switch (mode) {
    case Mode::OverWrite: {
        m_fd = wcdb_open(
        GetPathString(path), O_BINARY | O_CREAT | O_WRONLY | O_TRUNC, FileFullAccess);
        break;
    }
    default:
        WCTAssert(mode == Mode::ReadOnly);
        m_fd = wcdb_open(GetPathString(path), O_RDONLY | O_BINARY);
        break;
    }
    if (m_fd == -1) {
        setThreadedError();
        return false;
    }
    m_mode = mode;
    return true;
}

bool FileHandle::isOpened() const
{
    return m_fd != -1;
}

void FileHandle::close()
{
#ifdef _WIN32
    if (m_mapHandle != INVALID_HANDLE_VALUE) {
        bool ret = CloseHandle(m_mapHandle);
        WCTAssert(ret);
        WCDB_UNUSED(ret);
        m_mapHandle = INVALID_HANDLE_VALUE;
    }
#endif
    if (m_fd != -1) {
        int ret = ::close(m_fd);
        WCTAssert(ret == 0);
        WCDB_UNUSED(ret);
        m_fd = -1;
    }
}

ssize_t FileHandle::size()
{
    if (m_mode == Mode::ReadOnly && m_fileSize >= 0) {
        return m_fileSize;
    }
    auto fileSize = FileManager::getFileSize(path);
    if (fileSize.succeed()) {
        m_fileSize = fileSize.value();
    }
    return m_fileSize;
}

Data FileHandle::read(size_t size)
{
    if (size == 0) {
        return Data::null();
    }
    WCTAssert(isOpened());
    Data data(size);
    if (data.empty()) {
        return Data::null();
    }
    offset_t offset = (offset_t) wcdb_lseek(m_fd, 0, SEEK_SET);
    if (offset != 0) {
        setThreadedError();
        return Data::null();
    }
    ssize_t got;
    size_t prior = 0;
    size_t readSize = size;
    unsigned char *buffer = data.buffer();
    do {
        got = ::read(m_fd, buffer, readSize);
        if (got == readSize) {
            break;
        }
#ifndef _WIN32
        if (got <= 0) {
            auto err = errno;
            if (err == EDEVERR || err == EILSEQ || err == EINVAL) {
                setThreadedError("This file may be permanently damaged");
                prior = 0;
                break;
            }
        }
#endif
        if (got < 0) {
            if (errno == EINTR) {
                got = 1;
                continue;
            }
            prior = 0;
            setThreadedError();
            break;
        } else if (got > 0) {
            readSize -= got;
            prior += got;
            buffer = got + buffer;
        }
    } while (got > 0);
    if (got + prior == size) {
        return data;
    }
    Error error;
    error.level = m_errorIgnorable ? Error::Level::Warning : Error::Level::Error;
    error.setSystemCode(EIO, Error::Code::IOError, "Short read.");
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
    Notifier::shared().notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
    return data.subdata(got + prior);
}

bool FileHandle::write(const UnsafeData &unsafeData)
{
    WCTAssert(isOpened());
    ssize_t wrote;
    ssize_t prior = 0;
    size_t size = unsafeData.size();
    const unsigned char *buffer = unsafeData.buffer();
    offset_t offset = (offset_t) wcdb_lseek(m_fd, 0, SEEK_SET);
    if (offset != 0) {
        setThreadedError();
        return false;
    }
    do {
        wrote = ::write(m_fd, buffer, size);
        if (wrote == size) {
            break;
        }
        if (wrote < 0) {
            if (errno == EINTR) {
                wrote = 1;
                continue;
            }
            setThreadedError();
            break;
        } else if (wrote > 0) {
            size -= wrote;
            prior += wrote;
            buffer = wrote + buffer;
        }
    } while (wrote > 0);
    if (wrote + prior == unsafeData.size()) {
        m_fileSize = unsafeData.size();
        return true;
    }
    m_fileSize = -1;
    Error error;
    error.level = m_errorIgnorable ? Error::Level::Warning : Error::Level::Error;
    error.setSystemCode(EIO, Error::Code::IOError, "Short write.");
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
    Notifier::shared().notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
    return false;
}

#pragma mark - Memory map
MappedData FileHandle::map(offset_t offset, size_t length, SharedHighWater highWater)
{
    WCTRemedialAssert(m_mode == Mode::ReadOnly,
                      "Map is only supported in Readonly mode.",
                      return MappedData::null(););
    WCTAssert(length > 0);
    size_t fileSize = size();
    size_t pageSize = memoryPageSize();
    offset_t offsetAlignment = offset % pageSize;
    offset_t roundedOffset = offset - offsetAlignment;
    WCTAssert(roundedOffset < fileSize);

    size_t roundedSize = length + offsetAlignment;
    size_t sizeAlignment = roundedSize % pageSize;
    if (sizeAlignment != 0) {
        roundedSize += pageSize - sizeAlignment;
    }
    if (roundedSize + roundedOffset > fileSize) {
        roundedSize = fileSize - roundedOffset;
    }

#ifndef _WIN32

    // Avoid to mmap permanently corrupted files
    if (offset == 0) {
        auto testData = read(4);
        if (testData.size() <= 0) {
            return MappedData::null();
        }
    }

    void *mapped = mmap(
    nullptr, roundedSize, PROT_READ, MAP_SHARED | MAP_NOEXTEND | MAP_NORESERVE, m_fd, roundedOffset);
    if (mapped == MAP_FAILED) {
        Error error;
        error.level = m_errorIgnorable ? Error::Level::Warning : Error::Level::Error;
        error.setSystemCode(errno, Error::Code::IOError);
        error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
        error.infos.insert_or_assign("MmapSize", roundedSize);
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        return MappedData::null();
    }
    return MappedData(reinterpret_cast<unsigned char *>(mapped), roundedSize, highWater)
    .subdata(offsetAlignment, std::min(length, roundedSize));
#else
    if (m_mapHandle == INVALID_HANDLE_VALUE) {
        HANDLE handle = (HANDLE) _get_osfhandle(m_fd);
        if (handle == INVALID_HANDLE_VALUE) {
            Error error;
            error.level = m_errorIgnorable ? Error::Level::Warning : Error::Level::Error;
            error.setSystemCode(errno, Error::Code::IOError);
            error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
            error.infos.insert_or_assign("MmapSize", roundedSize);
            Notifier::shared().notify(error);
            SharedThreadedErrorProne::setThreadedError(std::move(error));
            return MappedData::null();
        }
        m_mapHandle = CreateFileMappingA(handle, NULL, PAGE_READONLY, 0, 0, NULL);
        if (m_mapHandle == INVALID_HANDLE_VALUE) {
            Error error;
            error.level = m_errorIgnorable ? Error::Level::Warning : Error::Level::Error;
            error.setWinSystemCode(GetLastError(), Error::Code::IOError);
            error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
            error.infos.insert_or_assign("MmapSize", roundedSize);
            Notifier::shared().notify(error);
            SharedThreadedErrorProne::setThreadedError(std::move(error));
            return MappedData::null();
        }
    }
    WCTAssert(m_mapHandle != INVALID_HANDLE_VALUE);
    uint32_t offsetLow = roundedOffset & 0xffffffff;
    uint32_t offsetHigh = roundedOffset >> 32;
    void *mapped
    = MapViewOfFile(m_mapHandle, FILE_MAP_READ, offsetHigh, offsetLow, roundedSize);
    if (mapped == NULL) {
        Error error;
        error.level = m_errorIgnorable ? Error::Level::Warning : Error::Level::Error;
        error.setWinSystemCode(GetLastError(), Error::Code::IOError);
        error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
        error.infos.insert_or_assign("MmapSize", roundedSize);
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        return MappedData::null();
    }
    return MappedData(reinterpret_cast<unsigned char *>(mapped), roundedSize, highWater)
    .subdata(offsetAlignment, std::min(length, roundedSize));
#endif
}

UnsafeData FileHandle::mapOrReadAllData()
{
    do {
        ssize_t fileSize = size();
        if (fileSize < 0) {
            break;
        }
        FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(path);
        markErrorAsIgnorable(true);
        UnsafeData data = map(0, fileSize);
        markErrorAsIgnorable(false);
        if (data.size() == fileSize) {
            return data;
        }
        data = read(fileSize);
        if (data.size() == fileSize) {
            return data;
        }
    } while (false);
    return UnsafeData::null();
}

const size_t &FileHandle::memoryPageSize()
{
#ifndef _WIN32
    static size_t s_memoryPageSize = getpagesize();
#else
    static size_t s_memoryPageSize = 0;
    if (s_memoryPageSize == 0) {
        SYSTEM_INFO system_info;
        GetSystemInfo(&system_info);
        s_memoryPageSize
        = std::max(system_info.dwPageSize, system_info.dwAllocationGranularity);
    }
#endif
    return s_memoryPageSize;
}

#pragma mark - Error
void FileHandle::markErrorAsIgnorable(bool flag)
{
    m_errorIgnorable = flag;
}

void FileHandle::setThreadedError(const UnsafeStringView &message)
{
    Error error;
    error.level = m_errorIgnorable ? Error::Level::Warning : Error::Level::Error;
    error.setSystemCode(errno, Error::Code::IOError, message);
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
    Notifier::shared().notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}

} //namespace WCDB
