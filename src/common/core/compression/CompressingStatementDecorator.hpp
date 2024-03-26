//
// Created by qiuwenchen on 2023/11/28.
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
#include "DecorativeHandleStatement.hpp"
#include <limits.h>
#include <list>
#include <unordered_map>

namespace WCDB {

class CompressingStatementDecorator final : public HandleStatementDecorator {
#pragma mark - Basic
public:
    CompressingStatementDecorator(Compression::Binder *binder);
    CompressingStatementDecorator(CompressingStatementDecorator &&other);
    ~CompressingStatementDecorator() override;

    CompressingStatementDecorator() = delete;
    CompressingStatementDecorator(const CompressingStatementDecorator &) = delete;
    CompressingStatementDecorator &operator=(const CompressingStatementDecorator &) = delete;

    void decorate(Decorative *handleStatement) override final;

protected:
    using Super = HandleStatementDecorator;

private:
    Compression::Binder *m_compressionBinder;

#pragma mark - Decorated functions
public:
    bool prepare(const Statement &statement) override final;
    void finalize() override final;
    bool step() override final;
    void reset() override final;
    void clearBindings() override final;

    void bindInteger(const Integer &value, int index) override final;
    void bindDouble(const Float &value, int index) override final;
    void bindText(const Text &value, int index) override final;
    void bindText16(const char16_t *value, size_t valueLength, int index) override final;
    void bindBLOB(const BLOB &value, int index) override final;
    void bindNull(int index) override final;
    void bindPointer(void *ptr, int index, const Text &type, void (*destructor)(void *)) override final;

#pragma mark - Process Statement
protected:
    bool processInsert(const StatementInsert &insert);
    bool processUpdate(const StatementUpdate &update);
    bool processSelect(const StatementSelect &select);
    bool processDelete(const StatementDelete &delete_);

    bool processCreateTable(const StatementCreateTable &createTable);
    bool processCreateView(const StatementCreateView &createView);
    bool processDropTable(const StatementDropTable &dropTable);
    bool processAlterTable(const StatementAlterTable &alterTable);

    bool initCompressionTableInfo(const Syntax::Schema &schema, const UnsafeStringView &table);
    bool adaptCompressingColumn(Statement &statement,
                                int *maxBindIndex = nullptr,
                                const CompressionTableInfo *curInfo = nullptr);
    typedef StringViewMap<const CompressionTableInfo *> TableInfos;
    bool parseTable(const std::list<Syntax::TableOrSubquery> &tables, TableInfos &tableInfos);
    bool checkBindParametersExist(std::list<Syntax::Expression> &exps);
    Optional<int>
    getBindParameter(std::list<Syntax::Expression> &exps, std::pair<int, int> &index);
    HandleStatement &addNewHandleStatement();

    void resetCompressionStatus();

    typedef struct BindInfo {
        std::pair<int, int> columnParaIndex = { -1, -1 };
        std::pair<int, int> matchColumnParaIndex = { -1, -1 };
        int columnBindIndex = 0;
        int typeBindIndex = 0;
        int matchColumnBindIndex = 0;
        Value bindedValue;
        const CompressionColumnInfo *columnInfo = nullptr;
    } BindInfo;

    static const int SelectedMatchValueBindIndex = INT_MAX;
    void bindValueInInfo(const BindInfo *bindInfo, const Integer &matchValue);

    using StatementType = Syntax::Identifier::Type;
    StatementType m_currentStatementType;
    bool m_processing;
    bool m_compressFail;
    const CompressionTableInfo *m_compressionTableInfo;
    std::list<const CompressionColumnInfo *> m_compressingUpdateColumns;
    std::list<BindInfo> m_bindInfoList;
    std::unordered_map<int, BindInfo *> m_bindInfoMap;
    std::list<HandleStatement> m_additionalStatements;

#pragma mark - Step Statement
protected:
    bool realStep();
    bool stepInsert();
    bool stepUpdate();
};

} // namespace WCDB
