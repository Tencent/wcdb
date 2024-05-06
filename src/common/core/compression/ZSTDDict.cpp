//
// Created by qiuwenchen on 2023/11/19.
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

#include "ZSTDDict.hpp"
#include "Assertion.hpp"
#include "Notifier.hpp"
#include "WCDBError.hpp"
#include <stdlib.h>
#if defined(WCDB_ZSTD) && WCDB_ZSTD
#define ZSTD_STATIC_LINKING_ONLY
#include <zstd/zdict.h>
#include <zstd/zstd.h>
#endif

namespace WCDB {

ZSTDDict::ZSTDDict()
: m_dictId(0), m_cDict(nullptr), m_dDict(nullptr), m_dictUseCount(0)
{
}

ZSTDDict::ZSTDDict(ZSTDDict&& dict)
: m_dictId(dict.m_dictId)
, m_cDict(dict.m_cDict)
, m_dDict(dict.m_dDict)
, m_memory(std::move(dict.m_memory))
, m_dictUseCount(dict.m_dictUseCount)
{
    dict.m_dictId = 0;
    dict.m_cDict = nullptr;
    dict.m_dDict = nullptr;
    dict.m_memory.clear();
    dict.m_dictUseCount = 0;
}

ZSTDDict& ZSTDDict::operator=(ZSTDDict&& other)
{
    clearDict();
    m_dictId = other.m_dictId;
    m_cDict = other.m_cDict;
    m_dDict = other.m_dDict;

    other.m_dictId = 0;
    other.m_cDict = nullptr;
    other.m_dDict = nullptr;

    return *this;
}

ZSTDDict::~ZSTDDict()
{
    clearDict();
}

#if defined(WCDB_ZSTD) && WCDB_ZSTD

void ZSTDDict::clearDict()
{
    ZSTD_freeCDict((ZSTD_CDict*) m_cDict);
    ZSTD_freeDDict((ZSTD_DDict*) m_dDict);
    WCTAssert(m_memory.size() == 0);
}

bool ZSTDDict::loadData(const UnsafeData& data)
{
    WCTAssert(m_dictId == 0 && m_cDict == nullptr && m_dDict == nullptr);
    if (data.empty()) {
        Error error(Error::Code::ZstdError, Error::Level::Error, "Empty dict!");
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        return false;
    }

    ZSTD_compressionParameters cparams
    = ZSTD_getCParams(ZSTD_CLEVEL_DEFAULT, ZSTD_CONTENTSIZE_UNKNOWN, data.size());
    m_cDict = (ZCDict*) ZSTD_createCDict_advanced(data.buffer(),
                                                  data.size(),
                                                  ZSTD_dlm_byCopy,
                                                  ZSTD_dct_auto,
                                                  cparams,
                                                  {
                                                  dictMemAlloc,
                                                  dictMemFree,
                                                  this,
                                                  });
    if (m_cDict == nullptr) {
        Error error(Error::Code::ZstdError, Error::Level::Error, "Create compress dict failed!");
        error.infos.insert_or_assign("DictSize", data.size());
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        return false;
    }
    initializeMemoryVerification();

    m_dDict = (ZDDcit*) ZSTD_createDDict(data.buffer(), data.size());
    if (m_dDict == nullptr) {
        Error error(Error::Code::ZstdError, Error::Level::Error, "Create decompress dict failed!");
        error.infos.insert_or_assign("DictSize", data.size());
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        return false;
    }
    m_dictId = ZSTD_getDictID_fromDDict((ZSTD_DDict*) m_dDict);
    if (m_dictId == 0) {
        Error error(Error::Code::ZstdError,
                    Error::Level::Error,
                    "The dictionary is not conformant to ZSTD specification");
        error.infos.insert_or_assign("DictSize", data.size());
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        return false;
    }
    return true;
}

#else

void ZSTDDict::clearDict()
{
}

bool ZSTDDict::loadData(const UnsafeData& data)
{
    Error error(Error::Code::ZstdError, Error::Level::Error, "You need to build WCDB with WCDB_ZSTD macro");
    error.infos.insert_or_assign("DictSize", data.size());
    Notifier::shared().notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
    return false;
}

#endif

ZSTDDict::DictId ZSTDDict::getDictId() const
{
    return m_dictId;
}

ZCDict* ZSTDDict::getCDict() const
{
    return m_cDict;
}

ZDDcit* ZSTDDict::getDDict() const
{
    return m_dDict;
}

#pragma mark - Memory verification
bool ZSTDDict::tryMemoryVerification() const
{
    if (++m_dictUseCount < kDictMaxUseCountBeforeCheck) {
        return true;
    }
    for (const auto& iter : m_memory) {
        UnsafeData dictMemory((unsigned char*) iter.first, iter.second.first);
        if (dictMemory.hash() != iter.second.second) {
            return false;
        }
    }
    m_dictUseCount = 0;
    return true;
}

void* ZSTDDict::dictMemAlloc(void* opaque, size_t size)
{
    ZSTDDict* dict = static_cast<ZSTDDict*>(opaque);
    WCTAssert(dict != nullptr);
    WCTAssert(dict->m_cDict == nullptr);
    void* buffer = malloc(size);
    if (buffer != nullptr) {
        dict->m_memory[buffer] = std::make_pair(size, 0);
    }
    return buffer;
}

void ZSTDDict::dictMemFree(void* opaque, void* address)
{
    ZSTDDict* dict = static_cast<ZSTDDict*>(opaque);
    WCTAssert(dict != nullptr);
    WCTAssert(dict->m_cDict != nullptr);
    WCTAssert(address != nullptr);
    if (address == nullptr) {
        return;
    }
    dict->m_memory.erase(address);
    free(address);
}

void ZSTDDict::initializeMemoryVerification()
{
    for (auto& iter : m_memory) {
        UnsafeData dictMemory((unsigned char*) iter.first, iter.second.first);
        iter.second.second = dictMemory.hash();
    }
}

} //namespace WCDB
