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
#if defined(WCDB_ZSTD) && WCDB_ZSTD
#include <zstd/zdict.h>
#include <zstd/zstd.h>
#endif

namespace WCDB {

ZSTDDict::ZSTDDict() : m_dictId(0), m_cDict(nullptr), m_dDict(nullptr)
{
}

ZSTDDict::ZSTDDict(ZSTDDict&& dict)
: m_dictId(dict.m_dictId), m_cDict(dict.m_cDict), m_dDict(dict.m_dDict)
{
    dict.m_dictId = 0;
    dict.m_cDict = nullptr;
    dict.m_dDict = nullptr;
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
    m_cDict = (ZCDict*) ZSTD_createCDict(data.buffer(), data.size(), ZSTD_CLEVEL_DEFAULT);
    if (m_cDict == nullptr) {
        Error error(Error::Code::ZstdError, Error::Level::Error, "Create compress dict failed!");
        error.infos.insert_or_assign("DictSize", data.size());
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        return false;
    }
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

} //namespace WCDB
