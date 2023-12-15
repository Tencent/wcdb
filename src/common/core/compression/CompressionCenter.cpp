//
// Created by qiuwenchen on 2023/11/21.
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

#include "CompressionCenter.hpp"
#include "Assertion.hpp"
#include "InnerHandle.hpp"
#include "Notifier.hpp"
#include "ScalarFunctionModule.hpp"
#include "WCDBError.hpp"
#include <string.h>
#if defined(WCDB_ZSTD) && WCDB_ZSTD
#include <zstd/zdict.h>
#include <zstd/zstd.h>
#endif

namespace WCDB {

CompressionCenter::CompressionCenter()
{
    m_dicts = (ZSTDDict**) calloc(MaxDictId, sizeof(ZSTDDict*));
    WCTAssert(m_dicts != nullptr);
}

CompressionCenter::~CompressionCenter()
{
    free(m_dicts);
}

CompressionCenter& CompressionCenter::shared()
{
    static CompressionCenter* g_dictCenter = new CompressionCenter;
    return *g_dictCenter;
}

ZSTDDict* CompressionCenter::getDict(DictId id) const
{
    if (id >= MaxDictId || id == 0) {
        return nullptr;
    }
    return m_dicts[id];
}

bool CompressionCenter::registerDict(DictId dictId, const UnsafeData& data)
{
    ZSTDDict* dict = new ZSTDDict;
    if (!dict->loadData(data)) {
        delete dict;
        return false;
    }
    if (dictId != dict->getDictId()) {
        Error error(Error::Code::ZstdError, Error::Level::Error, "DictId mismatch!");
        error.infos.insert_or_assign("GivenDictId", dictId);
        error.infos.insert_or_assign("ActualDictId", dict->getDictId());
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        delete dict;
        return false;
    }
    if (dict->getDictId() >= MaxDictId || dict->getDictId() == 0) {
        Error error(Error::Code::ZstdError, Error::Level::Error, "DictId must be an integer between 1 and 999!");
        error.infos.insert_or_assign("CurrendDictId", dict->getDictId());
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        delete dict;
        return false;
    }
    if (m_dicts[dict->getDictId()] != nullptr) {
        Error error(Error::Code::ZstdError, Error::Level::Error, "Duplicate dictid!");
        error.infos.insert_or_assign("DictId", dict->getDictId());
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        delete dict;
        return false;
    }
    m_dicts[dict->getDictId()] = dict;
    return true;
}

#if defined(WCDB_ZSTD) && WCDB_ZSTD

Optional<Data> CompressionCenter::trainDict(DictId dictId, TrainDataEnumerator dataEnummerator)
{
    if (dictId == 0 || dictId >= MaxDictId) {
        Error error(Error::Code::ZstdError, Error::Level::Error, "DictId must be an integer between 1 and 999");
        error.infos.insert_or_assign("CurrendDictId", dictId);
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        return NullOpt;
    }
    size_t totalSize = 0;
    std::vector<size_t> dataSizes;

    size_t dictSize = 100 * 1024;
    Data dict(dictSize);
    if (dict.empty()) {
        return NullOpt;
    }
    Data allData(1024 * 1024);
    if (allData.empty()) {
        return NullOpt;
    }

    Optional<UnsafeData> data = dataEnummerator();
    while (data.hasValue()) {
        size_t dataSize = data.value().size();
        if (totalSize + dataSize > allData.size()) {
            size_t newSize = std::max(2 * allData.size(), allData.size() + dataSize);
            if (!allData.resize(newSize)) {
                return NullOpt;
            }
        }

        memcpy(allData.buffer() + totalSize, data.value().buffer(), dataSize);
        totalSize += dataSize;
        dataSizes.push_back(dataSize);
        data = dataEnummerator();
    }

    dictSize = ZDICT_trainFromBuffer(dict.buffer(),
                                     dictSize,
                                     allData.buffer(),
                                     dataSizes.data(),
                                     (unsigned int) dataSizes.size());
    if (ZSTD_isError(dictSize)) {
        Error error(Error::Code::ZstdError, Error::Level::Error, "Train dict failed");
        error.infos.insert_or_assign("ZSTDErrorCode", dictSize);
        error.infos.insert_or_assign("ZSTDErrorName", ZSTD_getErrorName(dictSize));
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        return NullOpt;
    }

    dictSize = ZDICT_finalizeDictionary(dict.buffer(),
                                        dict.size(),
                                        dict.buffer(),
                                        dict.size(),
                                        allData.buffer(),
                                        dataSizes.data(),
                                        (unsigned int) dataSizes.size(),
                                        { ZSTD_CLEVEL_DEFAULT, 0, dictId });
    if (ZSTD_isError(dictSize)) {
        Error error(Error::Code::ZstdError, Error::Level::Error, "Finalize dict failed");
        error.infos.insert_or_assign("ZSTDErrorCode", dictSize);
        error.infos.insert_or_assign("ZSTDErrorName", ZSTD_getErrorName(dictSize));
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        return NullOpt;
    }
    return dict;
}

Optional<UnsafeData>
CompressionCenter::compressContent(const UnsafeData& data, DictId dictId, InnerHandle* errorReportHandle)
{
    if (data.size() == 0) {
        return data;
    }

    int64_t boundSize = ZSTD_compressBound(data.size());
    if (ZSTD_isError(boundSize)) {
        errorReportHandle->notifyError(
        Error::Code::ZstdError,
        nullptr,
        StringView::formatted("Compress bound fail: %s", ZSTD_getErrorName(boundSize)));
        return NullOpt;
    }
    ZSTDContext& ctx = m_ctxes.getOrCreate();
    void* buffer = ctx.getOrCreateBuffer(boundSize);
    if (buffer == nullptr) {
        errorReportHandle->notifyError(
        Error::Code::NoMemory, nullptr, "Compress fail due to no memory");
        return NullOpt;
    }
    int64_t compressSize = 0;
    if (dictId > 0) {
        ZSTDDict* dict = getDict(dictId);
        if (dict == nullptr) {
            errorReportHandle->notifyError(
            Error::Code::ZstdError,
            nullptr,
            StringView::formatted("Can not find compress dict with id: %d", dictId));
            return NullOpt;
        }
        compressSize = ZSTD_compress_usingCDict((ZSTD_CCtx*) ctx.getOrCreateCCtx(),
                                                buffer,
                                                boundSize,
                                                data.buffer(),
                                                data.size(),
                                                (ZSTD_CDict*) dict->getCDict());
    } else {
        compressSize = ZSTD_compress2((ZSTD_CCtx*) ctx.getOrCreateCCtx(),
                                      buffer,
                                      boundSize,
                                      data.buffer(),
                                      data.size());
    }
    if (ZSTD_isError(compressSize)) {
        errorReportHandle->notifyError(
        Error::Code::ZstdError,
        nullptr,
        StringView::formatted("Compress fail: %s", ZSTD_getErrorName(boundSize)));
        return NullOpt;
    }
    if (compressSize >= data.size()) {
        return data;
    }
    return UnsafeData((unsigned char*) buffer, compressSize);
}

void CompressionCenter::decompressContent(const UnsafeData& data,
                                          bool usingDict,
                                          ColumnType originType,
                                          ScalarFunctionAPI& resultAPI)
{
    int64_t frameSize = ZSTD_getFrameContentSize(data.buffer(), data.size());
    if (ZSTD_isError(frameSize)) {
        resultAPI.setErrorResult(Error::Code::ZstdError,
                                 StringView::formatted("Get compress content frame size fail: %s",
                                                       ZSTD_getErrorName(frameSize)));
        return;
    }
    ZSTDContext& ctx = m_ctxes.getOrCreate();
    void* buffer = ctx.getOrCreateBuffer(frameSize);
    if (buffer == nullptr) {
        resultAPI.setErrorResult(Error::Code::NoMemory, "Decompress fail due to no memory");
        return;
    }
    int64_t decompressSize = 0;
    if (usingDict) {
        DictId dictId = ZSTD_getDictID_fromFrame(data.buffer(), data.size());
        if (dictId == 0) {
            resultAPI.setErrorResult(Error::Code::ZstdError, "Can not decode dictid");
            return;
        }
        ZSTDDict* dict = getDict(dictId);
        if (dict == nullptr) {
            resultAPI.setErrorResult(
            Error::Code::ZstdError,
            StringView::formatted("Can not find decompress dict with id: %d", dictId));
            return;
        }
        decompressSize = ZSTD_decompress_usingDDict((ZSTD_DCtx*) ctx.getOrCreateDCtx(),
                                                    buffer,
                                                    frameSize,
                                                    data.buffer(),
                                                    data.size(),
                                                    (ZSTD_DDict*) dict->getDDict());
    } else {
        decompressSize = ZSTD_decompressDCtx((ZSTD_DCtx*) ctx.getOrCreateDCtx(),
                                             buffer,
                                             frameSize,
                                             data.buffer(),
                                             data.size());
    }

    if (ZSTD_isError(decompressSize)) {
        resultAPI.setErrorResult(
        Error::Code::ZstdError,
        StringView::formatted("Decompress fail: %s", ZSTD_getErrorName(decompressSize)));
        return;
    }
    if (originType == ColumnType::Text) {
        resultAPI.setTextResult(UnsafeStringView((char*) buffer, decompressSize));
    } else {
        resultAPI.setBlobResult(UnsafeData((unsigned char*) buffer, decompressSize));
    }
}

bool CompressionCenter::testContentCanBeDecompressed(const UnsafeData& data,
                                                     bool usingDict,
                                                     InnerHandle* errorReportHandle)
{
    int64_t frameSize = ZSTD_getFrameContentSize(data.buffer(), data.size());
    if (ZSTD_isError(frameSize)) {
        errorReportHandle->notifyError(
        Error::Code::ZstdError,
        StringView::formatted("Get compress content frame size fail: %s",
                              ZSTD_getErrorName(frameSize)));
        return false;
    }
    ZSTDContext& ctx = m_ctxes.getOrCreate();
    void* buffer = ctx.getOrCreateBuffer(frameSize);
    if (buffer == nullptr) {
        errorReportHandle->notifyError(
        Error::Code::NoMemory, "", "Decompress fail due to no memory");
        return false;
    }
    int64_t decompressSize = 0;
    if (usingDict) {
        DictId dictId = ZSTD_getDictID_fromFrame(data.buffer(), data.size());
        if (dictId == 0) {
            errorReportHandle->notifyError(Error::Code::ZstdError, "", "Can not decode dictid");
            return false;
        }
        ZSTDDict* dict = getDict(dictId);
        if (dict == nullptr) {
            errorReportHandle->notifyError(
            Error::Code::ZstdError,
            "",
            StringView::formatted("Can not find decompress dict with id: %d", dictId));
            return false;
        }
        decompressSize = ZSTD_decompress_usingDDict((ZSTD_DCtx*) ctx.getOrCreateDCtx(),
                                                    buffer,
                                                    frameSize,
                                                    data.buffer(),
                                                    data.size(),
                                                    (ZSTD_DDict*) dict->getDDict());
    } else {
        decompressSize = ZSTD_decompressDCtx((ZSTD_DCtx*) ctx.getOrCreateDCtx(),
                                             buffer,
                                             frameSize,
                                             data.buffer(),
                                             data.size());
    }

    if (ZSTD_isError(decompressSize)) {
        errorReportHandle->notifyError(
        Error::Code::ZstdError,
        "",
        StringView::formatted("Decompress fail: %s", ZSTD_getErrorName(decompressSize)));
        return false;
    }
    return true;
}

#else

Optional<Data> CompressionCenter::trainDict(DictId, TrainDataEnumerator)
{
    Error error(Error::Code::ZstdError, Error::Level::Error, "You need to build WCDB with WCDB_ZSTD macro");
    Notifier::shared().notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
    return NullOpt;
}

Optional<UnsafeData>
CompressionCenter::compressContent(const UnsafeData&, DictId, InnerHandle* errorReportHandle)
{
    errorReportHandle->notifyError(
    Error::Code::ZstdError, nullptr, "You need to build WCDB with WCDB_ZSTD macro");
    return NullOpt;
}

void CompressionCenter::decompressContent(const UnsafeData&, bool, ColumnType, ScalarFunctionAPI& resultAPI)
{
    resultAPI.setErrorResult(Error::Code::ZstdError,
                             "You need to build WCDB with WCDB_ZSTD macro");
}

bool CompressionCenter::testContentCanBeDecompressed(const UnsafeData&, bool, InnerHandle* errorReportHandle)
{
    errorReportHandle->notifyError(
    Error::Code::ZstdError, "", "You need to build WCDB with WCDB_ZSTD macro");
    return false;
}

#endif

} // namespace WCDB
