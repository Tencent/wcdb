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
#include "InnerHandle.hpp"
#include "Notifier.hpp"
#include "WCDBError.hpp"

namespace WCDB {

CompressionEvent::~CompressionEvent() = default;

#pragma mark - Initialize
Compression::Compression(CompressionEvent* event)
: m_tableAcquired(false), m_compressed(false), m_event(event)
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

    bool targetTableExsits = false;
    std::set<StringView> columns;
    CompressionTableInfo info(userInfo);
    if (!initializer.getTargetTableInfo(info, targetTableExsits, columns)) {
        return false;
    }

    if (targetTableExsits && !initializer.tryAddCompressionColumn(info, columns)) {
        return false;
    }

    LockGuard lockGuard(m_lock);
    if (m_filted.find(targetTable) == m_filted.end()) {
        m_compressed = false;
        if (!targetTableExsits || columns.empty()) {
            // it's not created
            m_hints.emplace(targetTable);
            m_tableAcquired = false;
        } else {
            m_holder.push_back(info);
            const CompressionTableInfo* hold = &m_holder.back();
            m_filted.insert_or_assign(targetTable, hold);
            m_hints.erase(targetTable);
        }
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
    return info;
}

#pragma mark - InfoInitializer
Compression::InfoInitializer::~InfoInitializer() = default;

bool Compression::InfoInitializer::getTargetTableInfo(const CompressionTableInfo& info,
                                                      bool& exists,
                                                      std::set<StringView>& columns)
{
    const StringView& tableName = info.getTable();
    InnerHandle* handle = getCurrentHandle();
    WCTAssert(handle != nullptr);
    auto optionalExists = handle->tableExists(Schema::main(), tableName);
    if (!optionalExists.hasValue()) {
        return false;
    }
    exists = optionalExists.value();
    if (!exists) {
        return true;
    }
    auto optionalMetas = handle->getTableMeta(Schema::main(), tableName);
    if (!optionalMetas.succeed()) {
        return false;
    }
    auto& metas = optionalMetas.value();
    for (const auto& meta : metas) {
        columns.emplace(meta.name);
    }
    auto tableConfig = handle->getTableAttribute(Schema::main(), tableName);
    if (!tableConfig.succeed()) {
        return false;
    }
    if (tableConfig.value().withoutRowid) {
        StringView msg = StringView::formatted(
        "Does not support to compress data in the table without rowid: %s",
        tableName.data());
        handle->notifyError(Error::Code::Error, nullptr, msg);
        return false;
    }
    return true;
}

bool Compression::InfoInitializer::tryAddCompressionColumn(const CompressionTableInfo& info,
                                                           std::set<StringView>& curColumns)
{
    const auto& compressingColumns = info.getColumnInfos();
    const StringView& tableName = info.getTable();
    InnerHandle* handle = getCurrentHandle();
    WCTAssert(handle != nullptr);
    std::list<StringView> toAddColumns;
    for (const auto& column : compressingColumns) {
        StringView statusColumn
        = StringView::formatted("%s%s",
                                CompressionColumnTypePrefix.data(),
                                column.getColumn().syntax().name.data());
        if (curColumns.find(statusColumn) != curColumns.end()) {
            continue;
        }
        toAddColumns.push_back(statusColumn);
    }
    for (const auto& column : toAddColumns) {
        if (!handle->addColumn(
            Schema::main(),
            tableName,
            ColumnDef(column, ColumnType::Integer).constraint(ColumnConstraint().default_(nullptr)))) {
            return false;
        }
    }
    return true;
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

Optional<const CompressionTableInfo*>
Compression::Binder::tryGetCompressionInfo(const UnsafeStringView& table)
{
    return m_compression.getOrInitInfo(*this, table);
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
    WCTAssert(worked.succeed());
    WCTAssert(!worked.value());

    {
        SharedLockGuard lockGuard(m_lock);
        WCTAssert(m_tableAcquired);
        if (!m_compressings.empty()) {
            // wait until all tables unreferenced
            return false;
        }
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
    auto compressed = stepper.compressRows(info);
    if (compressed.succeed() && compressed.value()) {
        markAsCompressed(info);
    }
    return compressed;
}

#pragma mark - Event
bool Compression::isCompressed() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_compressed;
}

} // namespace WCDB
