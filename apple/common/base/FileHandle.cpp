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

#include <WCDB/Assertion.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/FileHandle.hpp>
#include <WCDB/Notifier.hpp>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#ifndef __APPLE__
#include <WCDB/CrossPlatform.h>
#endif

namespace WCDB {

#pragma mark - Initialize
FileHandle::FileHandle(const UnsafeStringView &path_)
: path(path_), m_fd(-1), m_mode(Mode::None), m_errorIgnorable(false)
{
}

FileHandle::FileHandle(FileHandle &&other)
: path(std::move(other.path)), m_fd(other.m_fd), m_mode(other.m_mode)
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
    m_fd = std::move(other.m_fd);
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
        constexpr const int mask = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        static_assert(mask == 0644, "");
        m_fd = ::open(path.data(), O_CREAT | O_WRONLY | O_TRUNC, mask);
        break;
    }
    default:
        WCTAssert(mode == Mode::ReadOnly);
        m_fd = ::open(path.data(), O_RDONLY);
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
    if (m_fd != -1) {
        ::close(m_fd);
        m_fd = -1;
    }
}

ssize_t FileHandle::size()
{
    WCTAssert(isOpened());
    return (ssize_t) lseek(m_fd, 0, SEEK_END);
}

Data FileHandle::read(off_t offset, size_t size)
{
    WCTAssert(isOpened());
    Data data(size);
    if (data.empty()) {
        return Data::null();
    }
    ssize_t got;
    size_t prior = 0;
    unsigned char *buffer = data.buffer();
    do {
        got = pread(m_fd, buffer, size, offset);
        if (got == size) {
            break;
        }
        if (got < 0) {
            if (errno == EINTR) {
                got = 1;
                continue;
            }
            prior = 0;
            setThreadedError();
            break;
        } else if (got > 0) {
            size -= got;
            offset += got;
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

bool FileHandle::write(off_t offset, const UnsafeData &unsafeData)
{
    WCTAssert(isOpened());
    ssize_t wrote;
    ssize_t prior = 0;
    size_t size = unsafeData.size();
    const unsigned char *buffer = unsafeData.buffer();
    do {
        wrote = pwrite(m_fd, buffer, size, offset);
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
            offset += wrote;
            prior += wrote;
            buffer = wrote + buffer;
        }
    } while (wrote > 0);
    if (wrote + prior == size) {
        return true;
    }
    Error error;
    error.level = m_errorIgnorable ? Error::Level::Warning : Error::Level::Error;
    error.setSystemCode(EIO, Error::Code::IOError, "Short write.");
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
    Notifier::shared().notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
    return false;
}

#pragma mark - Memory map
MappedData FileHandle::map(off_t offset, size_t size)
{
    WCTRemedialAssert(m_mode == Mode::ReadOnly,
                      "Map is only supported in Readonly mode.",
                      return MappedData::null(););
    WCTAssert(size > 0);
    static int s_pagesize = getpagesize();
    int alignment = offset % s_pagesize;
    off_t roundedOffset = offset - alignment;
    size_t roundedSize = size + alignment;
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
    return MappedData(reinterpret_cast<unsigned char *>(mapped), roundedSize).subdata(alignment, size);
}

#pragma mark - Error
void FileHandle::markErrorAsIgnorable(bool flag)
{
    m_errorIgnorable = flag;
}

void FileHandle::setThreadedError()
{
    Error error;
    error.level = m_errorIgnorable ? Error::Level::Warning : Error::Level::Error;
    error.setSystemCode(errno, Error::Code::IOError);
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
    Notifier::shared().notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}

} //namespace WCDB
