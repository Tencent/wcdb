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

#include "Compression.hpp"
#include "Assertion.hpp"
#include "CompressionConst.hpp"
#include "CompressionRecord.hpp"
#include "InnerHandle.hpp"
#include "Notifier.hpp"
#include "WCDBError.hpp"

namespace WCDB {

CompressionEvent::~CompressionEvent() = default;

#pragma mark - Initialize
Compression::Compression(CompressionEvent* event)
: m_dataVersion(0)
, m_hasCreatedRecord(false)
, m_canCompressNewData(true)
, m_tableAcquired(false)
, m_compressed(false)
, m_event(event)
{
}

void Compression::setTableFilter(const TableFilter& tableFilter)
{
    LockGuard lockGuard(m_lock);
    m_tableFilter = tableFilter;
    purge();
}

bool Compression::shouldCompress() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_tableFilter != nullptr;
}

void Compression::purge()
{
    LockGuard lockGuard(m_lock);
    m_tableAcquired = false;
    m_compressed = false;
    m_compressings.clear();
    m_holder.clear();
    m_hints.clear();
    m_filted.clear();
    // Invalidate all thread local data.
    m_dataVersion++;
}

void Compression::tryResetLocalStatus()
{
    if (m_localDataVersion.getOrCreate() == m_dataVersion) {
        return;
    }
    m_localFilted.getOrCreate().clear();
    m_commitingTables.getOrCreate().clear();
    m_localDataVersion.getOrCreate() = m_dataVersion;
}

bool Compression::initInfo(InfoInitializer& initializer, const UnsafeStringView& table)
{
    // do not migrate sqlite builtin table
    if (table.hasPrefix(Syntax::builtinTablePrefix)
        || table.hasPrefix(Syntax::builtinWCDBTablePrefix)) {
        markAsNoNeedToCompress(table);
        return true;
    }
    StringView targetTable = StringView(table);
    TableFilter filter;
    {
        SharedLockGuard lockGuard(m_lock);
        filter = m_tableFilter;
    }
    CompressionTableUserInfo userInfo(targetTable);
    if (filter != nullptr) {
        filter(userInfo);
    }
    if (!userInfo.shouldCompress()) {
        markAsNoNeedToCompress(table);
        return true;
    }

    auto tableExist = initializer.tableExist(userInfo.getTable());
    if (tableExist.failed()) {
        return false;
    }

    if (!tryCreateRecordTable(initializer)) {
        return false;
    }

    LockGuard lockGuard(m_lock);
    auto iter = m_filted.find(targetTable);
    if (iter == m_filted.end()) {
        m_compressed = false;
        if (!tableExist.value()) {
            // it's not created
            m_hints.emplace(targetTable);
            m_tableAcquired = false;
        } else {
            m_holder.emplace_back(userInfo);
            const CompressionTableInfo* hold = &m_holder.back();
            m_filted.insert_or_assign(targetTable, hold);
            m_hints.erase(targetTable);
        }
    } else {
        iter->second->setNeedCheckColumns(true);
    }
    return true;
}

void Compression::markAsNoNeedToCompress(const UnsafeStringView& table)
{
    LockGuard lockGuard(m_lock);
    if (m_filted.find(table) == m_filted.end()) {
        m_filted.insert_or_assign(table, nullptr);
    }
    m_hints.erase(table);
}

bool Compression::hintThatTableWillBeCreated(InfoInitializer& initializer,
                                             const UnsafeStringView& table)
{
    return initInfo(initializer, table);
}

void Compression::markAsCompressed(const CompressionTableInfo* info)
{
    {
        LockGuard lockGuard(m_lock);
        WCTAssert(info != nullptr);
        WCTAssert(m_filted.find(info->getTable()) != m_filted.end());
        WCTAssert(m_hints.find(info->getTable()) == m_hints.end());
        WCTAssert(m_compressings.find(info) != m_compressings.end());
        m_compressings.erase(info);
    }
    if (m_event != nullptr) {
        m_event->didCompress(info);
    }
}

Optional<const CompressionTableInfo*> Compression::getInfo(const UnsafeStringView& table)
{
    tryResetLocalStatus();
    auto localFilted = m_localFilted.getOrCreate();
    auto localIter = localFilted.find(table);
    if (localIter != localFilted.end()) {
        return localIter->second;
    }
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_filted.find(table);
    if (iter != m_filted.end()) {
        localFilted.insert_or_assign(iter->first, iter->second);
        return iter->second;
    }
    return NullOpt;
}

Optional<const CompressionTableInfo*>
Compression::getOrInitInfo(InfoInitializer& initializer, const UnsafeStringView& table)
{
    auto info = getInfo(table);
    if (!info.succeed() && initInfo(initializer, table)) {
        info = getInfo(table);
    }
    if (info.hasValue() && info.value() != nullptr
        && !checkCompressingColumn(initializer, info.value())) {
        return NullOpt;
    }
    return info;
}

bool Compression::tryFixCompressingColumn(InfoInitializer& initializer,
                                          const UnsafeStringView& table)
{
    auto info = getInfo(table);
    if (!info.succeed() || info.value() == nullptr) {
        return false;
    }
    auto compressingInfo = info.value();
    if (compressingInfo->needCheckColumns()) {
        return false;
    }
    compressingInfo->setNeedCheckColumns(true);
    return checkCompressingColumn(initializer, compressingInfo);
}

bool Compression::checkCompressingColumn(InfoInitializer& initializer,
                                         const CompressionTableInfo* info)
{
    if (!info->needCheckColumns()) {
        return true;
    }
    InnerHandle* handle = initializer.getCurrentHandle();
    auto addColumn = initializer.checkCompressingColumns(*info);
    if (addColumn.failed()) {
        return false;
    }
    info->setNeedCheckColumns(false);
    if (addColumn.value() && handle->isInTransaction()) {
        tryResetLocalStatus();
        m_commitingTables.getOrCreate().insert(info);
    }
    return true;
}

void Compression::notifyTransactionCommitted(bool committed)
{
    tryResetLocalStatus();
    if (!committed) {
        for (auto tableInfo : m_commitingTables.getOrCreate()) {
            tableInfo->setNeedCheckColumns(true);
        }
        if (m_localHasCreatedRecord.getOrCreate()) {
            m_hasCreatedRecord = false;
        }
    }
    m_localHasCreatedRecord.getOrCreate() = false;
    m_commitingTables.getOrCreate().clear();
}

bool Compression::tryCreateRecordTable(InfoInitializer& initializer)
{
    if (m_hasCreatedRecord) {
        return true;
    }
    auto exist = initializer.tableExist(CompressionRecord::tableName);
    if (exist.failed()) {
        return false;
    }
    if (exist.value()) {
        m_hasCreatedRecord = false;
        return true;
    }
    InnerHandle* handle = initializer.getCurrentHandle();
    WCTAssert(handle != nullptr);
    HandleStatement createTable(handle);
    if (!createTable.prepare(CompressionRecord::getCreateTableStatement())) {
        return false;
    }
    bool created = createTable.step();
    createTable.finalize();
    if (!created) {
        return false;
    }
    m_hasCreatedRecord = false;
    if (handle->isInTransaction()) {
        m_localHasCreatedRecord.getOrCreate() = true;
    }
    return true;
}

#pragma mark - InfoInitializer
Compression::InfoInitializer::~InfoInitializer() = default;

Optional<bool> Compression::InfoInitializer::tableExist(const UnsafeStringView& table)
{
    InnerHandle* handle = getCurrentHandle();
    WCTAssert(handle != nullptr);
    return handle->tableExists(Schema::main(), table);
}

Optional<bool>
Compression::InfoInitializer::checkCompressingColumns(const CompressionTableInfo& info)
{
    auto& compressingColumns = info.getColumnInfos();
    const StringView& tableName = info.getTable();
    InnerHandle* handle = getCurrentHandle();
    WCTAssert(handle != nullptr);
    auto optionalMetas = handle->getTableMeta(Schema::main(), tableName);
    if (!optionalMetas.succeed()) {
        return NullOpt;
    }
    auto& metas = optionalMetas.value();
    std::list<StringView> curColumns;
    for (const auto& meta : metas) {
        curColumns.push_back(meta.name);
    }
    auto tableConfig = handle->getTableAttribute(Schema::main(), tableName);
    if (!tableConfig.succeed()) {
        return NullOpt;
    }
    if (tableConfig.value().withoutRowid) {
        StringView msg = StringView::formatted(
        "Does not support to compress data in the table without rowid: %s",
        tableName.data());
        handle->notifyError(Error::Code::Misuse, nullptr, msg);
        return NullOpt;
    }
    uint16_t newColumnIndex = (uint16_t) curColumns.size();
    for (auto& compressingColumn : compressingColumns) {
        uint16_t columnIndex = 0;
        bool findTypeColumn = false;
        for (const auto& column : curColumns) {
            if (column.equal(compressingColumn.getColumn().syntax().name)) {
                compressingColumn.setColumnIndex(columnIndex);
            } else if (column.equal(compressingColumn.getTypeColumn().syntax().name)) {
                compressingColumn.setTypeColumnIndex(columnIndex);
                findTypeColumn = true;
            } else if (column.equal(compressingColumn.getMatchColumn().syntax().name)) {
                compressingColumn.setMatchColumnIndex(columnIndex);
            }
            columnIndex++;
        }
        if (!findTypeColumn) {
            if (!handle->addColumn(
                Schema::main(),
                tableName,
                ColumnDef(compressingColumn.getTypeColumn(), ColumnType::Integer)
                .constraint(ColumnConstraint().default_(nullptr)))) {
                return NullOpt;
            }
            compressingColumn.setTypeColumnIndex(newColumnIndex);
            newColumnIndex++;
        }
    }
    if (newColumnIndex != curColumns.size()) {
        return true;
    }
    return false;
}

Compression::Binder::Binder(Compression& compression)
: m_compression(compression)
{
}

Compression::Binder::~Binder() = default;

bool Compression::Binder::hintThatTableWillBeCreated(const UnsafeStringView& table)
{
    return m_compression.hintThatTableWillBeCreated(*this, table);
}

void Compression::Binder::notifyTransactionCommitted(bool committed)
{
    m_compression.notifyTransactionCommitted(committed);
}

Optional<const CompressionTableInfo*>
Compression::Binder::tryGetCompressionInfo(const UnsafeStringView& table)
{
    return m_compression.getOrInitInfo(*this, table);
}

bool Compression::Binder::tryFixCompressingColumn(const UnsafeStringView& table)
{
    return m_compression.tryFixCompressingColumn(*this, table);
}

bool Compression::Binder::canCompressNewData() const
{
    return m_compression.canCompressNewData();
}

bool Compression::canCompressNewData() const
{
    return m_canCompressNewData;
}

void Compression::setCanCompressNewData(bool canCompress)
{
    m_canCompressNewData = canCompress;
}

#pragma mark - Step
Compression::Stepper::~Stepper() = default;

Optional<bool> Compression::step(Compression::Stepper& stepper)
{
    auto worked = tryCompressRows(stepper);
    if (!worked.succeed()) {
        return NullOpt;
    }
    if (worked.value()) {
        return false;
    }
    // acquire table is a readonly operation.
    // retry if and only if it works.
    worked = tryAcquireTables(stepper);
    if (!worked.succeed()) {
        return NullOpt;
    }
    if (worked.value()) {
        return step(stepper);
    }
    LockGuard lockGuard(m_lock);
    WCTAssert(m_tableAcquired);
    WCTAssert(m_compressings.empty());
    WCTAssert(m_hints.empty());
    m_compressed = true;
    if (m_event != nullptr) {
        m_event->didCompress(nullptr);
    }
    return true;
}

Optional<bool> Compression::tryAcquireTables(Compression::Stepper& stepper)
{
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_tableAcquired) {
            return false;
        }
    }
    auto optionalTables = stepper.getAllTables();
    if (!optionalTables.succeed()) {
        return NullOpt;
    }
    StringViewSet& tables = optionalTables.value();
    tables.insert(m_hints.begin(), m_hints.end());
    for (const auto& table : tables) {
        WCTAssert(!table.hasPrefix(Syntax::builtinTablePrefix)
                  && !table.hasPrefix(Syntax::builtinWCDBTablePrefix));
        if (!initInfo(stepper, table)) {
            return NullOpt;
        }
    }
    std::set<const CompressionTableInfo*> needCompressInfos;
    {
        SharedLockGuard lockGuard(m_lock);
        for (auto iter = tables.begin(); iter != tables.end();) {
            auto filtedIter = m_filted.find(*iter);
            if (filtedIter == m_filted.end() || filtedIter->second == nullptr) {
                iter = tables.erase(iter);
            } else {
                needCompressInfos.insert(filtedIter->second);
                iter++;
            }
        }
    }
    if (!stepper.filterComplessingTables(needCompressInfos)) {
        return NullOpt;
    }
    LockGuard lockGuard(m_lock);
    m_tableAcquired = true;
    m_compressings = needCompressInfos;
    return true;
}

Optional<bool> Compression::tryCompressRows(Compression::Stepper& stepper)
{
    const CompressionTableInfo* info = nullptr;
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_compressings.empty()) {
            return false;
        }
        info = *m_compressings.begin();
    }
    WCTAssert(info != nullptr);
    InnerHandle* handle = stepper.getCurrentHandle();
    auto exists = handle->tableExists(info->getTable());
    if (!exists.succeed()) {
        return NullOpt;
    }
    if (!exists.value()) {
        handle->execute(CompressionRecord::getDeleteRecordStatement(info->getTable()));
        markAsCompressed(info);
        return true;
    }
    if (!checkCompressingColumn(stepper, info)) {
        return NullOpt;
    }

    auto compressed = stepper.compressRows(info);
    if (compressed.failed()) {
        return NullOpt;
    }
    if (compressed.succeed() && compressed.value()) {
        markAsCompressed(info);
    }
    return true;
}

#pragma mark - Event
bool Compression::isCompressed() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_compressed;
}

} // namespace WCDB
