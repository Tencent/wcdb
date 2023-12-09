//
// Created by qiuwenchen on 2023/11/26.
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

#include "Compression.hpp"
#include "HandleOperator.hpp"
#include <array>
#include <set>

namespace WCDB {

class CompressHandleOperator final : public HandleOperator, public Compression::Stepper {
public:
    CompressHandleOperator(InnerHandle* handle);
    ~CompressHandleOperator() override final;

#pragma mark - Stepper
    Optional<StringViewSet> getAllTables() override final;
    bool filterComplessingTables(std::set<const CompressionTableInfo*>& allTableInfos) override final;
    Optional<bool> compressRows(const CompressionTableInfo* info) override final;
    bool compressRow(int64_t rowid);

private:
    bool prepareCompressionStatements();
    void resetCompressionStatements();
    void finalizeCompressionStatements();
    bool updateCompressionRecord();
    bool m_compressionRecordTableCreated;
    int m_compressedCount;
    const CompressionTableInfo* m_compressingTableInfo;
    HandleStatement* m_selectRowidStatement;
    HandleStatement* m_selectValueStatement;
    HandleStatement* m_updateValueStatement;
    HandleStatement* m_updateRecordStatement;

#pragma mark - Info Initializer
protected:
    InnerHandle* getCurrentHandle() const override final;
};

} //namespace WCDB
