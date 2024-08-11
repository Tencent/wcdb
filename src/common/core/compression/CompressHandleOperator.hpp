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
    ~CompressHandleOperator() override;

#pragma mark - Stepper
    Optional<StringViewSet> getAllTables() override final;
    bool filterComplessingTables(std::set<const CompressionTableInfo*>& allTableInfos) override final;
    Optional<bool> compressRows(const CompressionTableInfo* info) override final;
    bool rollbackCompression(const CompressionTableInfo* info) override final;
    bool deleteCompressionRecord() override final;

private:
    std::set<StringView> parseColumns(const StringView& compressRecord);

    typedef struct CompressionPerformance {
        int compressedCount = 0;
        int uncompressedCount = 0;
        size_t compressedSize = 0;
        size_t originalSize = 0;
        int64_t compressTime = 0;
        size_t totalSize = 0;
    } CompressionPerformance;

    Optional<bool> doCompressRows(const OneColumnValue& rowids);
    bool compressRow(OneRowValue& row);

    bool prepareCompressionStatements();
    void resetCompressionStatements();
    void finalizeCompressionStatements();
    bool updateCompressionRecord();

    Optional<int64_t>
    batchRollbackCompression(const CompressionTableInfo* info,
                             const std::list<const CompressionColumnInfo*>& compressedColumns,
                             int64_t& maxRowId,
                             int64_t curRowId);
    bool execute(const Statement& statement);
    Optional<std::list<const CompressionColumnInfo*>>
    getCompressedColumns(const CompressionTableInfo* info);

    int m_compressedCount;
    const CompressionTableInfo* m_compressingTableInfo;
    size_t m_insertParameterCount;
    HandleStatement* m_selectRowidStatement;
    HandleStatement* m_selectRowStatement;
    HandleStatement* m_deleteRowStatement;
    HandleStatement* m_insertNewRowStatement;
    HandleStatement* m_updateRecordStatement;

    CompressionPerformance m_performance;
    void reportPerformance(const UnsafeStringView& table);

#pragma mark - Info Initializer
protected:
    InnerHandle* getCurrentHandle() const override final;
};

} //namespace WCDB
