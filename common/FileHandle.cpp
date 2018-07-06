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
#include <WCDB/FileHandle.hpp>
#include <WCDB/Notifier.hpp>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace WCDB {

FileHandle::FileHandle(const std::string &path_)
: path(path_), m_fd(-1), m_mode(Mode::None)
{
}

FileHandle::FileHandle(FileHandle &&other)
: path(std::move(other.path))
, m_fd(std::move(other.m_fd))
, m_mode(std::move(other.m_mode))
, m_mmap(std::move(other.m_mmap))
{
    other.m_fd = -1;
    other.m_mode = Mode::None;
}

FileHandle::~FileHandle()
{
    WCTRemedialAssert(!isOpened() || (m_mode & 0xff) != Mode::OverWrite,
                      "Close should be call manually to sync file.",
                      ;);
    close();
}

FileHandle &FileHandle::operator=(FileHandle &&other)
{
    WCTInnerAssert(path == other.path);
    m_fd = std::move(other.m_fd);
    other.m_fd = -1;
    return *this;
}

bool FileHandle::open(int mode)
{
    WCTInnerAssert(m_mmap.empty());
    WCTInnerAssert(mode != Mode::None);
    WCTRemedialAssert(!isOpened(), "File already is opened", markAsMisuse("Duplicate open.");
                      return true;);
    m_mode = mode;
    int openMode = m_mode & 0xff;
    bool mmap = m_mode & Mode::Mmap;
    switch (openMode) {
    case Mode::OverWrite:
        m_fd = ::open(path.c_str(),
                      O_CREAT | O_WRONLY | O_TRUNC,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); //0x0644
        if (mmap) {
            markAsMisuse("Invalid mmap mode.");
            close();
        }
        break;
    case Mode::ReadOnly:
        m_fd = ::open(path.c_str(), O_RDONLY);
        if (mmap) {
            if (!remap()) {
                close();
            }
        }
        break;
    default:
        markAsMisuse("Invalid open mode.");
        return false;
    }
    if (m_fd == -1) {
        m_mode = Mode::None;
        setThreadedError();
        return false;
    }
    m_mode = mode;
    return true;
}

#pragma mark - Mmap
bool FileHandle::unmap()
{
    if (!m_mmap.empty()) {
        if (munmap(m_mmap.buffer(), m_mmap.size()) < 0) {
            setThreadedError();
            return false;
        }
        m_mmap = Data::emptyData();
    }
    return true;
}

bool FileHandle::remap()
{
    WCTInnerAssert(isOpened());
    WCTRemedialAssert((m_mode & 0xff) == Mode::ReadOnly,
                      "Mmap only supports read-only mode.",
                      return false;);
    if (!unmap()) {
        return false;
    }
    WCTInnerAssert(m_mmap.empty());
    size_t fileSize = size();
    void *mapped = ::mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, m_fd, 0);
    if (mapped == MAP_FAILED) {
        setThreadedError();
        return false;
    }
    m_mmap = Data::noCopyData((unsigned char *) mapped, fileSize);
    return true;
}

bool FileHandle::isOpened() const
{
    return m_fd != -1;
}

void FileHandle::close()
{
    unmap();
    if (m_fd != -1) {
        ::close(m_fd);
        m_fd = -1;
    }
}

ssize_t FileHandle::size()
{
    WCTInnerAssert(isOpened());
    return (ssize_t) lseek(m_fd, 0, SEEK_END);
}

Data FileHandle::read(off_t offset, size_t size)
{
    WCTInnerAssert(isOpened());
    if (!m_mmap.empty()) {
        if (m_mmap.size() < size) {
            Error error;
            error.setCode(Error::Code::Exceed);
            error.message = "Size exceeds the mapped data.";
            error.infos.set("Path", path);
            Notifier::shared()->notify(error);
            SharedThreadedErrorProne::setThreadedError(std::move(error));
            return Data::emptyData();
        }
        return m_mmap.subdata(offset, size);
    }
    Data data(size);
    if (data.empty()) {
        return Data::emptyData();
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
    error.setSystemCode(EIO, Error::Code::IOError);
    error.message = "Short read.";
    error.infos.set("Path", path);
    Notifier::shared()->notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
    return data.subdata(got + prior);
}

bool FileHandle::write(off_t offset, const Data &data)
{
    WCTInnerAssert(isOpened());
    ssize_t wrote;
    ssize_t prior = 0;
    size_t size = data.size();
    const unsigned char *buffer = data.buffer();
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
    error.setSystemCode(EIO, Error::Code::IOError);
    error.message = "Short write.";
    error.infos.set("Path", path);
    Notifier::shared()->notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
    return false;
}

void FileHandle::setThreadedError()
{
    Error error;
    error.setSystemCode(errno, Error::Code::IOError);
    error.message = strerror(errno);
    error.infos.set("Path", path);
    Notifier::shared()->notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}

void FileHandle::markAsMisuse(const char *message)
{
    Error error;
    error.setCode(Error::Code::Misuse);
    error.message = message;
    error.infos.set("Path", path);
    Notifier::shared()->notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}

} //namespace WCDB
