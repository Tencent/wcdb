//
// Created by 陈秋文 on 2023/11/26.
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

#include "ZSTDContext.hpp"
#include <memory>
#include <stdlib.h>
#if defined(WCDB_ZSTD) && WCDB_ZSTD
#include <zstd/zstd.h>
#endif

namespace WCDB {

ZSTDContext::ZSTDContext()
: m_buffer(nullptr), m_bufferSize(0), m_cctx(nullptr), m_dctx(nullptr)
{
}

ZSTDContext::~ZSTDContext()
{
#if defined(WCDB_ZSTD) && WCDB_ZSTD
    if (m_cctx != nullptr) {
        ZSTD_freeCCtx((ZSTD_CCtx*) m_cctx);
    }
    if (m_dctx != nullptr) {
        ZSTD_freeDCtx((ZSTD_DCtx*) m_dctx);
    }
#endif
    if (m_buffer != nullptr) {
        free(m_buffer);
    }
}

ZCCtx* ZSTDContext::getOrCreateCCtx()
{
    if (m_cctx == nullptr) {
#if defined(WCDB_ZSTD) && WCDB_ZSTD
        m_cctx = (ZCCtx*) ZSTD_createCCtx();
#endif
    }
    return m_cctx;
}

ZDCtx* ZSTDContext::getOrCreateDCtx()
{
    if (m_dctx == nullptr) {
#if defined(WCDB_ZSTD) && WCDB_ZSTD
        m_dctx = (ZDCtx*) ZSTD_createDCtx();
#endif
    }
    return m_dctx;
}

void* ZSTDContext::getOrCreateBuffer(size_t size)
{
    if (size < MaxBufferSize && m_bufferSize >= MaxBufferSize) {
        // Free accidentally large buffer
        free(m_buffer);
        m_buffer = nullptr;
    }
    if (m_bufferSize > size && m_buffer != nullptr) {
        return m_buffer;
    }
    if (m_buffer != nullptr) {
        free(m_buffer);
    }
    m_buffer = malloc(size);
    m_bufferSize = size;
    return m_buffer;
}

} //namespace WCDB
