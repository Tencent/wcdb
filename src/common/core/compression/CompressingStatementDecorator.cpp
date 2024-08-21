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

#include "CompressingStatementDecorator.hpp"
#include "Assertion.hpp"
#include "CommonCore.hpp"
#include "CompressionCenter.hpp"
#include "CompressionConst.hpp"
#include "CompressionRecord.hpp"
#include "InnerHandle.hpp"
#include "StringView.hpp"
#include "WINQ.h"
#include <algorithm>

namespace WCDB {

#pragma mark - Basic
CompressingStatementDecorator::CompressingStatementDecorator(Compression::Binder* binder)
: HandleStatementDecorator()
, m_compressionBinder(binder)
, m_currentStatementType(StatementType::Invalid)
, m_processing(false)
, m_compressFail(false)
, m_compressionTableInfo(nullptr)
{
}

CompressingStatementDecorator::CompressingStatementDecorator(CompressingStatementDecorator&& other)
: Super(std::move(other))
, m_compressionBinder(other.m_compressionBinder)
, m_currentStatementType(other.m_currentStatementType)
, m_processing(other.m_processing)
, m_additionalStatements(std::move(other.m_additionalStatements))
{
    other.m_compressionBinder = nullptr;
    other.m_processing = false;
    other.m_currentStatementType = StatementType::Invalid;
    other.m_additionalStatements.clear();
}

CompressingStatementDecorator::~CompressingStatementDecorator() = default;

void CompressingStatementDecorator::decorate(Decorative* handleStatement)
{
    Super::decorate(handleStatement);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, prepare);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, finalize);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, step);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, reset);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, clearBindings);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, bindInteger);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, bindDouble);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, bindText);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, bindText16);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, bindBLOB);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, bindNull);
    WCDBSwizzleDecorativeFunction(handleStatement, CompressingStatementDecorator, bindPointer);
}

#pragma mark - Override
bool CompressingStatementDecorator::prepare(const Statement& statement)
{
    WCTRemedialAssert(!getHandleStatement()->isPrepared(),
                      "Last statement is not finalized.",
                      finalize(););
    WCTAssert(!m_processing);
    m_processing = true;
    resetCompressionStatus();
    bool ret = true;
    m_currentStatementType = statement.getType();
    switch (m_currentStatementType) {
    case StatementType::InsertSTMT:
        ret = processInsert(static_cast<const StatementInsert&>(statement));
        break;
    case StatementType::UpdateSTMT:
        ret = processUpdate(static_cast<const StatementUpdate&>(statement));
        break;
    case StatementType::SelectSTMT:
        ret = processSelect(static_cast<const StatementSelect&>(statement));
        break;
    case StatementType::DeleteSTMT:
        ret = processDelete(static_cast<const StatementDelete&>(statement));
        break;
    case StatementType::CreateTableSTMT:
        ret = processCreateTable(static_cast<const StatementCreateTable&>(statement));
        break;
    case StatementType::CreateViewSTMT:
        ret = processCreateView(static_cast<const StatementCreateView&>(statement));
        break;
    case StatementType::DropTableSTMT:
        ret = processDropTable(static_cast<const StatementDropTable&>(statement));
        break;
    case StatementType::AlterTableSTMT:
        ret = processAlterTable(static_cast<const StatementAlterTable&>(statement));
        break;
    default:
        ret = Super::prepare(statement);
        break;
    }
    m_processing = false;
    return ret;
}

void CompressingStatementDecorator::finalize()
{
    Super::finalize();
    for (auto& handleStatement : m_additionalStatements) {
        handleStatement.finalize();
    }
    m_additionalStatements.clear();
    resetCompressionStatus();
}

bool CompressingStatementDecorator::step()
{
    if (m_compressFail) {
        return false;
    }
    if (m_additionalStatements.size() > 0) {
        WCTAssert(dynamic_cast<InnerHandle*>(getHandle()) != nullptr);
        InnerHandle* handle = static_cast<InnerHandle*>(getHandle());
        return handle->runTransaction([this](InnerHandle*) { return realStep(); });
    }
    return realStep();
}

void CompressingStatementDecorator::reset()
{
    Super::reset();
    for (auto& handleStatement : m_additionalStatements) {
        handleStatement.reset();
    }
    m_compressFail = false;
}

void CompressingStatementDecorator::clearBindings()
{
    Super::clearBindings();
    for (auto& handleStatement : m_additionalStatements) {
        handleStatement.clearBindings();
    }
    m_compressFail = false;
}

void CompressingStatementDecorator::bindInteger(const Integer& value, int index)
{
    if (getHandleStatement()->getBindParameterCount() >= index) {
        Super::bindInteger(value, index);
    }
    BindInfo* info = m_bindInfoMap[index];
    if (info != nullptr) {
        WCTAssert(index != info->typeBindIndex);
        if (index == info->matchColumnBindIndex) {
            if (!info->bindedValue.isNull()) {
                bindValueInInfo(info, value);
            } else {
                info->bindedValue = value;
            }
        } else {
            Super::bindInteger((Integer) CompressedType::None, info->typeBindIndex);
        }
    } else if (m_currentStatementType == StatementType::UpdateSTMT
               && !m_additionalStatements.empty()
               && m_additionalStatements.front().getBindParameterCount() >= index) {
        m_additionalStatements.front().bindInteger(value, index);
    }
}

void CompressingStatementDecorator::bindDouble(const Float& value, int index)
{
    if (getHandleStatement()->getBindParameterCount() >= index) {
        Super::bindDouble(value, index);
    }
    BindInfo* info = m_bindInfoMap[index];
    if (info != nullptr) {
        if (index == info->matchColumnBindIndex) {
            getHandle()->notifyError(
            Error::Code::Misuse,
            "",
            StringView::formatted(
            "Bind float to compression match column: %s",
            info->columnInfo->getMatchColumn().syntax().name.data()));
            m_compressFail = true;
            return;
        }
        WCTAssert(index != info->typeBindIndex);
        Super::bindInteger((Integer) CompressedType::None, info->typeBindIndex);
    } else if (m_currentStatementType == StatementType::UpdateSTMT
               && !m_additionalStatements.empty()
               && m_additionalStatements.front().getBindParameterCount() >= index) {
        m_additionalStatements.front().bindDouble(value, index);
    }
}

void CompressingStatementDecorator::bindText(const Text& value, int index)
{
    BindInfo* info = m_bindInfoMap[index];
    if (info != nullptr) {
        WCTAssert(index == info->columnBindIndex);
        if (index == info->matchColumnBindIndex) {
            getHandle()->notifyError(
            Error::Code::Misuse,
            "",
            StringView::formatted(
            "Bind text to compression match column: %s",
            info->columnInfo->getMatchColumn().syntax().name.data()));
            m_compressFail = true;
            return;
        }
        UnsafeData data((unsigned char*) value.data(), value.size());
        if (info->matchColumnBindIndex > 0) {
            if (!info->bindedValue.isNull()) {
                Optional<UnsafeData> compressedValue;
                if (m_compressionBinder->canCompressNewData()) {
                    compressedValue = CompressionCenter::shared().compressContent(
                    data,
                    info->columnInfo->getMatchDictId(info->bindedValue.intValue()),
                    static_cast<InnerHandle*>(getHandle()));
                } else {
                    compressedValue = data;
                }
                if (compressedValue.failed()) {
                    m_compressFail = true;
                    return;
                }
                if (compressedValue.value().size() < value.size()) {
                    Super::bindBLOB(compressedValue.value(), info->columnBindIndex);
                    Super::bindInteger(WCDBMergeCompressionType(
                                       CompressedType::ZSTDDict, ColumnType::Text),
                                       info->typeBindIndex);
                } else {
                    Super::bindText(value, index);
                    if (m_compressionBinder->canCompressNewData()) {
                        Super::bindInteger(WCDBMergeCompressionType(
                                           CompressedType::None, ColumnType::Text),
                                           info->typeBindIndex);
                    } else {
                        Super::bindNull(info->typeBindIndex);
                    }
                }
            } else {
                info->bindedValue = value;
            }
        } else {
            bool usingDict
            = info->columnInfo->getCompressionType() == CompressionType::Dict;
            Optional<UnsafeData> compressedValue;
            if (m_compressionBinder->canCompressNewData()) {
                compressedValue = CompressionCenter::shared().compressContent(
                data,
                usingDict ? info->columnInfo->getDictId() : 0,
                static_cast<InnerHandle*>(getHandle()));
            } else {
                compressedValue = data;
            }
            if (compressedValue.failed()) {
                m_compressFail = true;
                return;
            }
            if (compressedValue.value().size() < value.size()) {
                Super::bindBLOB(compressedValue.value(), info->columnBindIndex);
                Super::bindInteger(
                WCDBMergeCompressionType(usingDict ? CompressedType::ZSTDDict :
                                                     CompressedType::ZSTDNormal,
                                         ColumnType::Text),
                info->typeBindIndex);
            } else {
                Super::bindText(value, index);
                if (m_compressionBinder->canCompressNewData()) {
                    Super::bindInteger(
                    WCDBMergeCompressionType(CompressedType::None, ColumnType::Text),
                    info->typeBindIndex);
                } else {
                    Super::bindNull(info->typeBindIndex);
                }
            }
        }
    } else {
        if (getHandleStatement()->getBindParameterCount() >= index) {
            Super::bindText(value, index);
        }
        if (m_currentStatementType == StatementType::UpdateSTMT
            && !m_additionalStatements.empty()
            && m_additionalStatements.front().getBindParameterCount() >= index) {
            m_additionalStatements.front().bindText(value, index);
        }
    }
}

void CompressingStatementDecorator::bindText16(const char16_t* value, size_t valueLength, int index)
{
    BindInfo* info = m_bindInfoMap[index];
    if (info == nullptr) {
        if (getHandleStatement()->getBindParameterCount() >= index) {
            Super::bindText16(value, valueLength, index);
        }
        if (m_currentStatementType == StatementType::UpdateSTMT
            && !m_additionalStatements.empty()
            && m_additionalStatements.front().getBindParameterCount() >= index) {
            m_additionalStatements.front().bindText16(value, valueLength, index);
        }
    } else {
        bindText(StringView::createFromUTF16(value, valueLength), index);
    }
}

void CompressingStatementDecorator::bindBLOB(const BLOB& value, int index)
{
    BindInfo* info = m_bindInfoMap[index];
    if (m_bindInfoMap[index] != nullptr) {
        WCTAssert(index == info->columnBindIndex);
        if (index == info->matchColumnBindIndex) {
            getHandle()->notifyError(
            Error::Code::Misuse,
            "",
            StringView::formatted(
            "Bind blob to compression match column: %s",
            info->columnInfo->getMatchColumn().syntax().name.data()));
            m_compressFail = true;
            return;
        }
        if (info->matchColumnBindIndex > 0) {
            if (!info->bindedValue.isNull()) {
                Optional<UnsafeData> compressedValue;
                if (m_compressionBinder->canCompressNewData()) {
                    compressedValue = CompressionCenter::shared().compressContent(
                    value,
                    info->columnInfo->getMatchDictId(info->bindedValue.intValue()),
                    static_cast<InnerHandle*>(getHandle()));
                } else {
                    compressedValue = value;
                }
                if (compressedValue.failed()) {
                    m_compressFail = true;
                    return;
                }
                if (compressedValue.value().size() < value.size()) {
                    Super::bindBLOB(compressedValue.value(), info->columnBindIndex);
                    Super::bindInteger(WCDBMergeCompressionType(
                                       CompressedType::ZSTDDict, ColumnType::BLOB),
                                       info->typeBindIndex);
                } else {
                    Super::bindBLOB(value, index);
                    if (m_compressionBinder->canCompressNewData()) {
                        Super::bindInteger(WCDBMergeCompressionType(
                                           CompressedType::None, ColumnType::BLOB),
                                           info->typeBindIndex);
                    } else {
                        Super::bindNull(info->typeBindIndex);
                    }
                }
            } else {
                info->bindedValue = value;
            }
        } else {
            bool usingDict
            = info->columnInfo->getCompressionType() == CompressionType::Dict;
            Optional<UnsafeData> compressedValue;
            if (m_compressionBinder->canCompressNewData()) {
                compressedValue = CompressionCenter::shared().compressContent(
                value,
                usingDict ? info->columnInfo->getDictId() : 0,
                static_cast<InnerHandle*>(getHandle()));
            } else {
                compressedValue = value;
            }
            if (compressedValue.failed()) {
                m_compressFail = true;
                return;
            }
            if (compressedValue.value().size() < value.size()) {
                Super::bindBLOB(compressedValue.value(), info->columnBindIndex);
                Super::bindInteger(
                WCDBMergeCompressionType(usingDict ? CompressedType::ZSTDDict :
                                                     CompressedType::ZSTDNormal,
                                         ColumnType::BLOB),
                info->typeBindIndex);
            } else {
                Super::bindBLOB(value, index);
                if (m_compressionBinder->canCompressNewData()) {
                    Super::bindInteger(
                    WCDBMergeCompressionType(CompressedType::None, ColumnType::BLOB),
                    info->typeBindIndex);
                } else {
                    Super::bindNull(info->typeBindIndex);
                }
            }
        }
    } else {
        if (getHandleStatement()->getBindParameterCount() >= index) {
            Super::bindBLOB(value, index);
        }
        if (m_currentStatementType == StatementType::UpdateSTMT
            && !m_additionalStatements.empty()
            && m_additionalStatements.front().getBindParameterCount() >= index) {
            m_additionalStatements.front().bindBLOB(value, index);
        }
    }
}

void CompressingStatementDecorator::bindNull(int index)
{
    if (getHandleStatement()->getBindParameterCount() >= index) {
        Super::bindNull(index);
    }
    BindInfo* info = m_bindInfoMap[index];
    if (info != nullptr) {
        WCTAssert(index == info->columnBindIndex);
        if (index == info->matchColumnBindIndex) {
            getHandle()->notifyError(
            Error::Code::Misuse,
            "",
            StringView::formatted(
            "Bind null to compression match column: %s",
            info->columnInfo->getMatchColumn().syntax().name.data()));
            m_compressFail = true;
            return;
        }
        Super::bindInteger((Integer) CompressedType::None, info->typeBindIndex);
    } else if (m_currentStatementType == StatementType::UpdateSTMT
               && !m_additionalStatements.empty()
               && m_additionalStatements.front().getBindParameterCount() >= index) {
        m_additionalStatements.front().bindNull();
    }
}

void CompressingStatementDecorator::bindPointer(void* ptr,
                                                int index,
                                                const Text& type,
                                                void (*destructor)(void*))
{
    if (getHandleStatement()->getBindParameterCount() >= index) {
        Super::bindPointer(ptr, index, type, destructor);
    }
    WCTAssert(m_bindInfoMap[index] == nullptr);
    if (m_currentStatementType == StatementType::UpdateSTMT
        && !m_additionalStatements.empty()
        && m_additionalStatements.front().getBindParameterCount() >= index) {
        m_additionalStatements.front().bindPointer(ptr, index, type, destructor);
    }
}

#pragma mark - Process Statement
bool CompressingStatementDecorator::processInsert(const StatementInsert& insert)
{
    const Syntax::InsertSTMT& insertSTMT = insert.syntax();
    if (insertSTMT.switcher == Syntax::InsertSTMT::Switch::Default) {
        return Super::prepare(insert);
    }

    // Get compression info
    if (!initCompressionTableInfo(insertSTMT.schema, insertSTMT.table)) {
        return false;
    }

    // Find compressing column
    if (m_compressionTableInfo != nullptr) {
        WCTRemedialAssert(!insertSTMT.isMultiWrite(),
                          "Insert statement that contains multiple values is not supported while using compression feature.",
                          return false;);
        for (const auto& compressingColumn : m_compressionTableInfo->getColumnInfos()) {
            m_bindInfoList.emplace_back();
            m_bindInfoList.back().columnInfo = &compressingColumn;
        }
        int index = 0;
        for (const auto& column : insertSTMT.columns) {
            for (auto& bindInfo : m_bindInfoList) {
                if (bindInfo.columnInfo->getColumn().syntax().name.equal(column.name)) {
                    bindInfo.columnParaIndex = { index, 0 };
                } else if (bindInfo.columnInfo->getMatchColumn().syntax().name.equal(
                           column.name)) {
                    bindInfo.matchColumnParaIndex = { index, 0 };
                }
                WCTAssert(
                !bindInfo.columnInfo->getTypeColumn().syntax().name.equal(column.name));
            }
            index++;
        }
        for (auto iter = m_bindInfoList.begin(); iter != m_bindInfoList.end();) {
            if (iter->columnParaIndex.first == -1) {
                iter = m_bindInfoList.erase(iter);
            } else {
                iter++;
            }
        }
    }

    bool needCompress
    = m_bindInfoList.size() > 0
      || (m_compressionTableInfo != nullptr && insertSTMT.columns.size() == 0);

    // Skip simple case
    if (!needCompress && insertSTMT.switcher == Syntax::InsertSTMT::Switch::Values
        && !insertSTMT.upsertClause.hasValue()
        && insertSTMT.commonTableExpressions.empty()) {
        return Super::prepare(insert);
    }

    StatementInsert newInsert = insert;
    int maxBindIndex = 0;
    if (!adaptCompressingColumn(newInsert, &maxBindIndex, m_compressionTableInfo)) {
        return false;
    }

    // Check whether data can be compressed directly in the current statement
    bool directCompress = true;
    Syntax::InsertSTMT& newInsertSTMT = newInsert.syntax();
    if (newInsertSTMT.switcher == Syntax::InsertSTMT::Switch::Select
        || newInsertSTMT.upsertClause.hasValue() || newInsertSTMT.columns.size() == 0
        || newInsertSTMT.expressionsValues.size() == 0) {
        directCompress = false;
    } else {
        WCTAssert(newInsertSTMT.switcher == Syntax::InsertSTMT::Switch::Values);
        directCompress
        = checkBindParametersExist(newInsertSTMT.expressionsValues.front());
    }

    // Add compression related columns/statements
    if (directCompress) {
        for (auto& bindInfo : m_bindInfoList) {
            m_bindInfoMap.emplace(bindInfo.columnBindIndex, &bindInfo);
            if (bindInfo.matchColumnBindIndex > 0) {
                WCTAssert(m_bindInfoMap.find(bindInfo.matchColumnBindIndex)
                          == m_bindInfoMap.end());
                m_bindInfoMap.emplace(bindInfo.matchColumnBindIndex, &bindInfo);
            }
            // INSERT INTO compressingTable(...columnA,..., columnB, ..., WCDB_CT_columnA, WCDB_CT_columnB, ...) VALUES(...)
            newInsertSTMT.columns.push_back(bindInfo.columnInfo->getTypeColumn());
            newInsertSTMT.expressionsValues.front().push_back(
            Expression(BindParameter(++maxBindIndex)));
            bindInfo.typeBindIndex = maxBindIndex;
            WCTAssert(m_bindInfoMap.find(maxBindIndex) == m_bindInfoMap.end());
            m_bindInfoMap.emplace(maxBindIndex, &bindInfo);
        }
        if (!Super::prepare(newInsert)) {
            return false;
        }
    } else {
        if (needCompress && newInsertSTMT.upsertClause.hasValue()) {
            const Syntax::UpsertClause& upsert = insertSTMT.upsertClause.value();
            Syntax::UpsertClause& newUpsert = newInsertSTMT.upsertClause.value();
            // Set null to compressed type
            for (const auto& columns : upsert.columnsList) {
                for (const auto& column : columns) {
                    for (const auto& columnInfo : m_compressionTableInfo->getColumnInfos()) {
                        if (column.name.equal(columnInfo.getColumn().syntax().name)) {
                            newUpsert.columnsList.push_back({ columnInfo.getTypeColumn() });
                            newUpsert.expressions.push_back(Expression(nullptr));
                            break;
                        }
                    }
                }
            }
        }
        if (!Super::prepare(newInsert)) {
            return false;
        }
        if (needCompress) {
            HandleStatement& selectUncompressRow = addNewHandleStatement();
            if (!selectUncompressRow.prepare(
                m_compressionTableInfo->getSelectUncompressRowStatement())) {
                return false;
            }
            HandleStatement& updateUncompressRow = addNewHandleStatement();
            if (!updateUncompressRow.prepare(
                m_compressionTableInfo->getUpdateCompressColumnStatement())) {
                return false;
            }
        }
    }
    return true;
}

bool CompressingStatementDecorator::processUpdate(const StatementUpdate& update)
{
    const Syntax::UpdateSTMT& updateSTMT = update.syntax();

    // Get compression info
    if (!initCompressionTableInfo(updateSTMT.table.schema, updateSTMT.table.table)) {
        return false;
    }

    // Find compressing column
    if (m_compressionTableInfo != nullptr) {
        for (const auto& compressingColumn : m_compressionTableInfo->getColumnInfos()) {
            m_bindInfoList.emplace_back();
            m_bindInfoList.back().columnInfo = &compressingColumn;
        }
        int i = 0;
        for (const auto& columns : updateSTMT.columnsList) {
            int j = 0;
            for (const auto& column : columns) {
                for (auto& bindInfo : m_bindInfoList) {
                    if (bindInfo.columnInfo->getColumn().syntax().name.equal(
                        column.name)) {
                        bindInfo.columnParaIndex = { i, j };
                    } else if (bindInfo.columnInfo->getMatchColumn()
                               .syntax()
                               .name.equal(column.name)) {
                        bindInfo.matchColumnParaIndex = { i, j };
                    }
                    WCTAssert(
                    !bindInfo.columnInfo->getTypeColumn().syntax().name.equal(
                    column.name));
                }
                j++;
            }
            i++;
        }
        for (auto iter = m_bindInfoList.begin(); iter != m_bindInfoList.end();) {
            if (iter->columnParaIndex.first == -1) {
                iter = m_bindInfoList.erase(iter);
            } else {
                m_compressingUpdateColumns.push_back(iter->columnInfo);
                iter++;
            }
        }
    }

    StatementUpdate newUpdate = update;
    int maxBindIndex = 0;
    if (!adaptCompressingColumn(newUpdate, &maxBindIndex, m_compressionTableInfo)) {
        return false;
    }

    // Check whether data can be compressed directly in the current statement
    Syntax::UpdateSTMT& newUpdateSTMT = newUpdate.syntax();
    bool directCompress = checkBindParametersExist(newUpdateSTMT.expressions);

    // Add compression related columns/statements
    if (directCompress) {
        for (auto& bindInfo : m_bindInfoList) {
            m_bindInfoMap.emplace(bindInfo.columnBindIndex, &bindInfo);
            if (bindInfo.matchColumnBindIndex > 0) {
                WCTAssert(m_bindInfoMap.find(bindInfo.matchColumnBindIndex)
                          == m_bindInfoMap.end());
                m_bindInfoMap.emplace(bindInfo.matchColumnBindIndex, &bindInfo);
            }
            newUpdateSTMT.columnsList.push_back({ bindInfo.columnInfo->getTypeColumn() });
            newUpdateSTMT.expressions.push_back(Expression(BindParameter(++maxBindIndex)));
            bindInfo.typeBindIndex = maxBindIndex;
            WCTAssert(m_bindInfoMap.find(maxBindIndex) == m_bindInfoMap.end());
            m_bindInfoMap.emplace(maxBindIndex, &bindInfo);
        }
        if (!Super::prepare(newUpdate)) {
            return false;
        }
    } else {
        if (m_bindInfoList.empty()) {
            if (!Super::prepare(newUpdate)) {
                return false;
            }
        } else {
            // SELECT rowid[, dictMatchTypeColumn] FROM compressingTable WHERE ... ORDER BY ... LIMIT ... OFFSET ...
            StatementSelect selectRowid
            = StatementSelect().select(Column::rowid()).from(newUpdateSTMT.table.table);
            selectRowid.syntax().select.getOrCreate().condition = newUpdateSTMT.condition;

            selectRowid.syntax().orderingTerms = newUpdateSTMT.orderingTerms;
            newUpdateSTMT.orderingTerms.clear();

            selectRowid.syntax().limitParameterType = newUpdateSTMT.limitParameterType;
            newUpdateSTMT.limitParameterType = Syntax::LimitParameterType::NotSet;
            selectRowid.syntax().limit = newUpdateSTMT.limit;
            newUpdateSTMT.limit = NullOpt;
            selectRowid.syntax().limitParameter = newUpdateSTMT.limitParameter;
            newUpdateSTMT.limitParameter = NullOpt;

            bool selectMatchType = true;
            for (auto& bindInfo : m_bindInfoList) {
                if (bindInfo.columnBindIndex == 0) {
                    selectMatchType = false;
                    break;
                }
                if (bindInfo.columnInfo->getCompressionType() != CompressionType::VariousDict) {
                    continue;
                }
                if (bindInfo.matchColumnParaIndex.first >= 0) {
                    selectMatchType = false;
                    break;
                }
            }

            if (selectMatchType) {
                for (auto& bindInfo : m_bindInfoList) {
                    m_bindInfoMap.emplace(bindInfo.columnBindIndex, &bindInfo);
                    if (bindInfo.matchColumnBindIndex > 0) {
                        WCTAssert(m_bindInfoMap.find(bindInfo.matchColumnBindIndex)
                                  == m_bindInfoMap.end());
                        m_bindInfoMap.emplace(bindInfo.matchColumnBindIndex, &bindInfo);
                    } else if (bindInfo.columnInfo->getCompressionType()
                               == CompressionType::VariousDict) {
                        selectRowid.syntax().select.getOrCreate().resultColumns.push_back(
                        ResultColumn(bindInfo.columnInfo->getMatchColumn()));
                        bindInfo.matchColumnBindIndex = SelectedMatchValueBindIndex;
                        m_bindInfoMap.emplace(bindInfo.matchColumnBindIndex, &bindInfo);
                    }
                    newUpdateSTMT.columnsList.push_back(
                    { bindInfo.columnInfo->getTypeColumn() });
                    newUpdateSTMT.expressions.push_back(
                    Expression(BindParameter(++maxBindIndex)));
                    bindInfo.typeBindIndex = maxBindIndex;
                    WCTAssert(m_bindInfoMap.find(maxBindIndex) == m_bindInfoMap.end());
                    m_bindInfoMap.emplace(maxBindIndex, &bindInfo);
                }
            } else {
                // Set null to compressed type
                for (auto& bindInfo : m_bindInfoList) {
                    newUpdateSTMT.columnsList.push_back(
                    { bindInfo.columnInfo->getTypeColumn() });
                    newUpdateSTMT.expressions.push_back(Expression(nullptr));
                }
            }

            //Rowid must be binded to the largest index
            newUpdate.where(Column::rowid() == BindParameter(++maxBindIndex));

            HandleStatement& selectRowidSTMT = addNewHandleStatement();
            if (!selectRowidSTMT.prepare(selectRowid)) {
                return false;
            }

            if (!Super::prepare(newUpdate)) {
                return false;
            }

            if (!selectMatchType) {
                HandleStatement& selectUncompressRow = addNewHandleStatement();
                if (!selectUncompressRow.prepare(m_compressionTableInfo->getSelectUncompressRowStatement(
                    &m_compressingUpdateColumns))) {
                    return false;
                }

                HandleStatement& updateUncompressRow = addNewHandleStatement();
                if (!updateUncompressRow.prepare(m_compressionTableInfo->getUpdateCompressColumnStatement(
                    &m_compressingUpdateColumns))) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool CompressingStatementDecorator::checkBindParametersExist(std::list<Syntax::Expression>& exps)
{
    bool ret = true;
    for (auto& bindInfo : m_bindInfoList) {
        //Check column value
        auto bindIndex = getBindParameter(exps, bindInfo.columnParaIndex);
        if (bindIndex.failed()) {
            ret = false;
            continue;
        }
        bindInfo.columnBindIndex = bindIndex.value();
        // Check match column value
        if (bindInfo.matchColumnParaIndex.first < 0) {
            if (bindInfo.columnInfo->getCompressionType() == CompressionType::VariousDict) {
                ret = false;
            }
            continue;
        }
        bindIndex = getBindParameter(exps, bindInfo.matchColumnParaIndex);
        if (bindIndex.failed()) {
            ret = false;
            continue;
        }
        bindInfo.matchColumnBindIndex = bindIndex.value();
    }
    return ret;
}

Optional<int>
CompressingStatementDecorator::getBindParameter(std::list<Syntax::Expression>& exps,
                                                std::pair<int, int>& index)
{
    if (exps.size() <= index.first) {
        return NullOpt;
    }
    auto iter = exps.begin();
    std::advance(iter, index.first);
    if (index.second > 0) {
        if (iter->switcher != Syntax::Expression::Switch::Expressions
            || iter->expressions.size() <= index.second) {
            return NullOpt;
        }
        iter = iter->expressions.begin();
        std::advance(iter, index.second);
    }
    if (iter->switcher != Syntax::Expression::Switch::BindParameter
        || iter->bindParameter().switcher != Syntax::BindParameter::Switch::QuestionSign) {
        return NullOpt;
    } else {
        return iter->bindParameter().n;
    }
}

HandleStatement& CompressingStatementDecorator::addNewHandleStatement()
{
    m_additionalStatements.emplace_back(getHandle());
    m_additionalStatements.back().enableAutoAddColumn();
    return m_additionalStatements.back();
}

bool CompressingStatementDecorator::processSelect(const StatementSelect& select)
{
    StatementSelect newSelect = select;
    if (!adaptCompressingColumn(newSelect)) {
        return false;
    }
    return Super::prepare(newSelect);
}

bool CompressingStatementDecorator::processDelete(const StatementDelete& delete_)
{
    const Syntax::DeleteSTMT& deleteSTMT = delete_.syntax();

    // Get compression info
    if (!initCompressionTableInfo(deleteSTMT.table.schema, deleteSTMT.table.table)) {
        return false;
    }
    StatementDelete newDelete = delete_;
    if (!adaptCompressingColumn(newDelete, nullptr, m_compressionTableInfo)) {
        return false;
    }
    return Super::prepare(newDelete);
}

bool CompressingStatementDecorator::processCreateTable(const StatementCreateTable& createTable)
{
    if (!createTable.syntax().schema.isMain()) {
        return Super::prepare(createTable);
    }
    auto compressingColumns = m_compressionBinder->tryGetCompressingColumnsForNewTable(
    createTable.syntax().table);
    if (compressingColumns.failed()) {
        return false;
    }
    if (compressingColumns.value().size() == 0) {
        return Super::prepare(createTable);
    }
    StatementCreateTable newStatement = createTable;
    for (auto columnInfo : compressingColumns.value()) {
        newStatement.define(ColumnDef(columnInfo.getTypeColumn(), ColumnType::Integer)
                            .constraint(ColumnConstraint().default_(nullptr)));
    }
    return Super::prepare(newStatement);
}

bool CompressingStatementDecorator::processCreateView(const StatementCreateView& createView)
{
    StatementCreateView newStatement = createView;
    if (!adaptCompressingColumn(newStatement)) {
        return false;
    }
    return Super::prepare(newStatement);
}

bool CompressingStatementDecorator::processDropTable(const StatementDropTable& dropTable)
{
    if (!Super::prepare(dropTable)) {
        return false;
    }
    if (!dropTable.syntax().schema.isMain()) {
        return true;
    }
    HandleStatement& newStatement = addNewHandleStatement();
    if (!newStatement.prepare(
        CompressionRecord::getDeleteRecordStatement(dropTable.syntax().table))) {
        if (getHandle()->getError().code() == Error::Code::Error
            && getHandle()->getError().getMessage().hasPrefix("no such table:")) {
            m_additionalStatements.pop_back();
        } else {
            return false;
        }
    }
    return true;
}

bool CompressingStatementDecorator::processAlterTable(const StatementAlterTable& alterTable)
{
    if (alterTable.syntax().switcher == Syntax::AlterTableSTMT::Switch::AddColumn) {
        return Super::prepare(alterTable);
    }
    // Get compression info
    if (!initCompressionTableInfo(alterTable.syntax().schema,
                                  alterTable.syntax().table)) {
        return false;
    }

    if (m_compressionTableInfo == nullptr) {
        return Super::prepare(alterTable);
    }
    if (alterTable.syntax().switcher == Syntax::AlterTableSTMT::Switch::RenameColumn) {
        getHandle()->notifyError(
        Error::Code::Misuse,
        alterTable.getDescription(),
        "Modifying the column name of a compressing table is not supported.");
        return false;
    }

    WCTAssert(alterTable.syntax().switcher == Syntax::AlterTableSTMT::Switch::RenameTable);

    if (!Super::prepare(alterTable)) {
        return false;
    }

    HandleStatement& newStatement = addNewHandleStatement();
    auto updateRecord = CompressionRecord::getUpdateRecordStatement(
    alterTable.syntax().table, alterTable.syntax().newTable);
    if (!newStatement.prepare(updateRecord)) {
        if (getHandle()->getError().getMessage().hasPrefix("no such table:")) {
            m_additionalStatements.clear();
            return true;
        }
        return false;
    }
    return true;
}

bool CompressingStatementDecorator::initCompressionTableInfo(const Syntax::Schema& schema,
                                                             const UnsafeStringView& table)
{
    if (schema.isMain()) {
        auto optionalTableInfo = m_compressionBinder->tryGetCompressionInfo(table);
        if (optionalTableInfo.failed()) {
            return false;
        }
        m_compressionTableInfo = optionalTableInfo.value();
    }
    return true;
}

/*
 Adapt compresssing columns in expression to decompress function.
 Eg:
    SELECT compresssingColumnA From compressingTable
 will be changed to
    SELECT wcdb_decompress(compresssingColumnA, WCDB_CT_compresssingColumnA) From compressingTable
 */
bool CompressingStatementDecorator::adaptCompressingColumn(Statement& statement,
                                                           int* maxBindIndex,
                                                           const CompressionTableInfo* curInfo)
{
    std::list<TableInfos> tableInfoStack;
    if (curInfo != nullptr) {
        tableInfoStack.emplace_back();
        tableInfoStack.back().insert_or_assign(curInfo->getTable(), curInfo);
    }
    bool succeed = true;
    std::unordered_map<Syntax::Expression*, const CompressionColumnInfo*> compressingColumns;
    statement.iterate([&](Syntax::Identifier& identifier, bool isBegin, bool& stop) {
        if (identifier.getType() == StatementType::SelectSTMT) {
            if (!isBegin) {
                tableInfoStack.pop_back();
                return;
            }
            tableInfoStack.emplace_back();
            TableInfos& tableInfos = tableInfoStack.back();
            Syntax::SelectSTMT& selectSTMT = (Syntax::SelectSTMT&) identifier;
            if (selectSTMT.select.hasValue()
                && selectSTMT.select.value().switcher != Syntax::SelectCore::Switch::Values
                && !parseTable(selectSTMT.select.value().tableOrSubqueries, tableInfos)) {
                succeed = false;
                stop = true;
                return;
            }
            for (const auto& selectCore : selectSTMT.cores) {
                if (selectCore.switcher != Syntax::SelectCore::Switch::Values
                    && !parseTable(selectCore.tableOrSubqueries, tableInfos)) {
                    succeed = false;
                    stop = true;
                    return;
                }
            }
        } else if (identifier.getType() == StatementType::SelectCore) {
            if (!isBegin) {
                tableInfoStack.pop_back();
                return;
            }
            Syntax::SelectCore& syntax = (Syntax::SelectCore&) identifier;
            tableInfoStack.emplace_back();
            if (syntax.switcher == Syntax::SelectCore::Switch::Values) {
                return;
            }
            TableInfos& tableInfos = tableInfoStack.back();
            if (!parseTable(syntax.tableOrSubqueries, tableInfos)) {
                succeed = false;
                stop = true;
                return;
            }
        } else if (identifier.getType() == StatementType::CommonTableExpression) {
            if (isBegin) {
                if (tableInfoStack.size() == 0) {
                    tableInfoStack.emplace_back();
                    Syntax::CommonTableExpression& cte
                    = (Syntax::CommonTableExpression&) identifier;
                    tableInfoStack.back().insert_or_assign(cte.table, nullptr);
                }
                tableInfoStack.emplace_back();
            } else {
                tableInfoStack.pop_back();
            }
        } else {
            if (!isBegin && identifier.getType() == Syntax::Identifier::Type::ResultColumn) {
                Syntax::ResultColumn& resultColumn = (Syntax::ResultColumn&) identifier;
                auto iter = compressingColumns.find(&resultColumn.expression.value());
                if (iter != compressingColumns.end()) {
                    resultColumn.alias = iter->second->getColumn().syntax().name;
                }
                return;
            }
            if (!isBegin || identifier.getType() != Syntax::Identifier::Type::Expression) {
                return;
            }
            Syntax::Expression& expression = (Syntax::Expression&) identifier;
            if (expression.switcher == Syntax::Expression::Switch::BindParameter) {
                if (expression.bindParameter().switcher
                    != Syntax::BindParameter::Switch::QuestionSign) {
                    getHandle()->notifyError(
                    Error::Code::Misuse,
                    statement.getDescription(),
                    "The compressing table only supports numeric type bind parameters.");
                    succeed = false;
                    stop = true;
                    return;
                }
                if (maxBindIndex == nullptr) {
                    return;
                }
                *maxBindIndex = std::max(expression.bindParameter().n, *maxBindIndex);
            }
            if (expression.switcher != Syntax::Expression::Switch::Column) {
                return;
            }
            Syntax::Column& column = expression.column();
            if (!column.schema.isMain()) {
                return;
            }
            if (tableInfoStack.size() == 0) {
                return;
            }
            const CompressionTableInfo* tableInfo = nullptr;
            if (!column.table.empty()) {
                auto stackIter = tableInfoStack.end();
                for (int i = 0; i < tableInfoStack.size(); i++) {
                    stackIter--;
                    auto iter = stackIter->find(column.table);
                    if (iter != stackIter->end()) {
                        tableInfo = iter->second;
                        break;
                    }
                }
            } else if (!tableInfoStack.back().empty()) {
                tableInfo = tableInfoStack.back().begin()->second;
            }
            if (tableInfo == nullptr) {
                return;
            }
            for (const auto& compressingColumn : tableInfo->getColumnInfos()) {
                if (compressingColumn.getColumn().syntax().name.equal(column.name)) {
                    compressingColumns[&expression] = &compressingColumn;
                    return;
                }
            }
        }
    });

    for (auto iter : compressingColumns) {
        Syntax::Expression& expression = *(iter.first);
        const CompressionColumnInfo& compressingColumn = *(iter.second);
        StringView table = expression.column().table;
        expression = Expression();
        expression.switcher = Syntax::Expression::Switch::Function;
        expression.function() = DecompressFunctionName;
        expression.expressions.push_back(Expression(compressingColumn.getColumn()));
        expression.expressions.back().column().table = table;
        expression.expressions.push_back(Expression(compressingColumn.getTypeColumn()));
        expression.expressions.back().column().table = table;
        expression.useWildcard = false;
    }

    return succeed;
}

bool CompressingStatementDecorator::parseTable(const std::list<Syntax::TableOrSubquery>& tables,
                                               StringViewMap<const CompressionTableInfo*>& tableInfos)
{
    for (const auto& table : tables) {
        if (table.switcher == Syntax::TableOrSubquery::Switch::Table) {
            if (!table.schema.isMain()) {
                return true;
            }
            auto tableInfo
            = m_compressionBinder->tryGetCompressionInfo(table.tableOrFunction);
            if (!tableInfo.hasValue()) {
                return false;
            }
            tableInfos.insert_or_assign(table.tableOrFunction, tableInfo.value());
            if (!table.alias.empty()) {
                tableInfos.insert_or_assign(table.alias, tableInfo.value());
            }
            return true;
        } else if (table.switcher == Syntax::TableOrSubquery::Switch::TableOrSubqueries) {
            if (!parseTable(table.tableOrSubqueries, tableInfos)) {
                return false;
            }
        } else if (!table.alias.empty()) {
            tableInfos.insert_or_assign(table.alias, nullptr);
        }
    }
    return true;
}

void CompressingStatementDecorator::resetCompressionStatus()
{
    m_compressFail = false;
    m_compressionTableInfo = nullptr;
    m_compressingUpdateColumns.clear();
    m_bindInfoMap.clear();
    m_bindInfoList.clear();
}

void CompressingStatementDecorator::bindValueInInfo(const BindInfo* info, const Integer& matchValue)
{
    UnsafeData data;
    UnsafeStringView text;
    ColumnType valueType = info->bindedValue.getType();
    if (valueType == Value::Type::Text) {
        text = info->bindedValue.textValue();
        data = UnsafeData((unsigned char*) text.data(), text.length());
    } else {
        data = info->bindedValue.blobValue();
    }
    Optional<UnsafeData> compressedValue;
    if (m_compressionBinder->canCompressNewData()) {
        compressedValue = CompressionCenter::shared().compressContent(
        data,
        info->columnInfo->getMatchDictId(matchValue),
        static_cast<InnerHandle*>(getHandle()));
    } else {
        compressedValue = data;
    }
    if (compressedValue.failed()) {
        m_compressFail = true;
        return;
    }
    if (compressedValue.value().size() < data.size()) {
        Super::bindBLOB(compressedValue.value(), info->columnBindIndex);
        Super::bindInteger(WCDBMergeCompressionType(CompressedType::ZSTDDict, valueType),
                           info->typeBindIndex);
    } else {
        if (valueType == Value::Type::Text) {
            Super::bindText(text, info->columnBindIndex);
        } else {
            Super::bindBLOB(data, info->columnBindIndex);
        }
        if (m_compressionBinder->canCompressNewData()) {
            Super::bindInteger(WCDBMergeCompressionType(CompressedType::None, valueType),
                               info->typeBindIndex);
        } else {
            Super::bindNull(info->typeBindIndex);
        }
    }
}

#pragma mark - Step Statement

bool CompressingStatementDecorator::realStep()
{
    switch (m_currentStatementType) {
    case StatementType::InsertSTMT:
        return stepInsert();
    case StatementType::UpdateSTMT:
        return stepUpdate();
    default: {
        if (!Super::step()) {
            return false;
        }
        for (auto& handleStatement : m_additionalStatements) {
            if (!handleStatement.step()) {
                return false;
            }
        }
        return true;
    }
    }
}

bool CompressingStatementDecorator::stepInsert()
{
    if (!Super::step()) {
        return false;
    }
    if (m_additionalStatements.size() == 0) {
        return true;
    }
    AbstractHandle* handle = getHandle();
    if (handle->getChanges() == 0) {
        return true;
    }
    WCTAssert(m_additionalStatements.size() == 2);
    auto iter = m_additionalStatements.begin();
    HandleStatement* select = &(*iter);
    iter++;
    HandleStatement* update = &(*iter);
    return m_compressionTableInfo->stepSelectAndUpdateUncompressRowStatement(
    select, update, handle->getLastInsertedRowID());
}

bool CompressingStatementDecorator::stepUpdate()
{
    if (m_additionalStatements.empty()) {
        return Super::step();
    }
    WCTAssert(m_compressionTableInfo != nullptr);
    WCTAssert(m_compressingUpdateColumns.size() > 0);
    AbstractHandle* handle = getHandle();
    auto iter = m_additionalStatements.begin();
    HandleStatement* selectRowid = &(*iter);
    iter++;

    HandleStatement* selectValue = nullptr;
    HandleStatement* updateValue = nullptr;
    bool needSelectValue = m_additionalStatements.size() > 1;
    if (needSelectValue) {
        WCTAssert(m_additionalStatements.size() == 3) selectValue = &(*iter);
        iter++;
        updateValue = &(*iter);
    }

    int rowidIndex = getHandleStatement()->getBindParameterCount();
    if (!selectRowid->step()) {
        return false;
    }
    while (!selectRowid->done()) {
        int64_t rowid = selectRowid->getInteger();
        Super::reset();
        Super::bindInteger(rowid, rowidIndex);
        if (!needSelectValue) {
            int index = 0;
            for (const auto& bindInfo : m_bindInfoList) {
                if (bindInfo.matchColumnBindIndex != SelectedMatchValueBindIndex) {
                    continue;
                }
                index++;
                if (bindInfo.bindedValue.isNull()) {
                    continue;
                }
                bindValueInInfo(&bindInfo, selectRowid->getValue(index));
            }
        }
        if (!Super::step()) {
            return false;
        }
        if (needSelectValue && handle->getChanges() > 0
            && !m_compressionTableInfo->stepSelectAndUpdateUncompressRowStatement(
            selectValue, updateValue, rowid, &m_compressingUpdateColumns)) {
            return false;
        }
        if (!selectRowid->step()) {
            return false;
        }
    }
    return true;
}

} //namespace WCDB
