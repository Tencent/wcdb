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

#pragma once
#include "SysTypes.h"

namespace WCDB {

typedef struct ZCCtx ZCCtx;
typedef struct ZDCtx ZDCtx;

class ZSTDContext {
public:
    ZSTDContext();
    ~ZSTDContext();
    ZCCtx* getOrCreateCCtx();
    ZDCtx* getOrCreateDCtx();
    void* getOrCreateBuffer(size_t size);

private:
    static constexpr const size_t MaxBufferSize = 1024 * 1024;
    void* m_buffer;
    size_t m_bufferSize;
    ZCCtx* m_cctx;
    ZDCtx* m_dctx;
};

} //namespace WCDB
