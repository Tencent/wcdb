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

#pragma once

#include "ColumnType.hpp"
#include "CompressionConst.hpp"
#include "ThreadLocal.hpp"
#include "ZSTDContext.hpp"
#include "ZSTDDict.hpp"
#include <memory>

namespace WCDB {

class ScalarFunctionAPI;
class InnerHandle;

class CompressionCenter : public SharedThreadedErrorProne {
public:
    CompressionCenter();
    ~CompressionCenter();
    CompressionCenter(const CompressionCenter&) = delete;
    CompressionCenter& operator=(const CompressionCenter&) = delete;

    static CompressionCenter& shared();

    using DictId = ZSTDDict::DictId;
    static constexpr const DictId MaxDictId = 1000;

    bool registerDict(DictId dictId, const UnsafeData& data);
    typedef std::function<Optional<UnsafeData>()> TrainDataEnumerator;
    Optional<Data> trainDict(DictId dictId, TrainDataEnumerator dataEnummerator);

    Optional<UnsafeData>
    compressContent(const UnsafeData& data, DictId dictId, InnerHandle* errorReportHandle);
    void decompressContent(const UnsafeData& data,
                           bool usingDict,
                           ColumnType originType,
                           ScalarFunctionAPI& resultAPI);
    Optional<UnsafeData>
    decompressContent(const UnsafeData& data, bool usingDict, InnerHandle* handle);

    bool testContentCanBeDecompressed(const UnsafeData& data,
                                      bool usingDict,
                                      InnerHandle* errorReportHandle);

private:
    ZSTDDict* getDict(DictId id) const;
    ZSTDDict** m_dicts;
    ThreadLocal<ZSTDContext> m_ctxes;
};

} // namespace WCDB
