//
// Created by sanhuazhang on 2018/07/23
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

#include "MappedData.hpp"
#include "Notifier.hpp"
#ifndef _WIN32
#include <sys/mman.h>
#else
#include <windows.h>
#endif

namespace WCDB {

MappedData::MappedData() : UnsafeData()
{
}

MappedData::MappedData(unsigned char* mapped, size_t size)
: UnsafeData(mapped, size, SharedBuffer(makeSharedBuffer(mapped, size, MappedData::unmapData)))
{
}

MappedData::MappedData(unsigned char* mapped, size_t size, SharedHighWater highWater)
: UnsafeData(mapped, size)
{
    m_sharedBuffer = makeSharedBuffer(mapped, size, [highWater](SharedData& data) {
        unmapData(data);
        if (highWater != nullptr) {
            highWater->decrease(data.size);
        }
    });
    if (highWater != nullptr) {
        highWater->increase(size);
    }
}

MappedData::~MappedData() = default;

void MappedData::unmapData(SharedData& data)
{
#ifndef _WIN32
    if (munmap(data.buffer, data.size) != 0) {
        Error error;
        error.level = Error::Level::Warning;
        error.setSystemCode(errno, Error::Code::IOError);
        error.infos.insert_or_assign("MunmapSize", data.size);
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
    }
#else
    if (!UnmapViewOfFile(data.buffer)) {
        Error error;
        error.level = Error::Level::Warning;
        error.setWinSystemCode(GetLastError(), Error::Code::IOError);
        error.infos.insert_or_assign("MunmapSize", data.size);
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
    }
#endif
}

MappedData::MappedData(const MappedData& other) : UnsafeData(other)
{
}

MappedData::MappedData(MappedData&& other) : UnsafeData(std::move(other))
{
}

MappedData::MappedData(const UnsafeData& data) : UnsafeData(data)
{
}

MappedData& MappedData::operator=(const MappedData& other)
{
    UnsafeData::operator=(other);
    return *this;
}

MappedData& MappedData::operator=(MappedData&& other)
{
    UnsafeData::operator=(std::move(other));
    return *this;
}

MappedData MappedData::subdata(size_t size) const
{
    return MappedData(UnsafeData::subdata(size));
}

MappedData MappedData::subdata(offset_t offset, size_t size) const
{
    return MappedData(UnsafeData::subdata(offset, size));
}

const MappedData& MappedData::null()
{
    static const MappedData* s_null = new MappedData;
    return *s_null;
}

} // namespace WCDB
