//
// Created by qiuwenchen on 2023/12/23.
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

#include "VacuumHandleOperator.hpp"
#include "CoreConst.h"
#include "WINQ.h"

namespace WCDB {

const char *VacuumHandleOperator::kOriginSchema = "origin";

VacuumHandleOperator::VacuumHandleOperator(InnerHandle *handle)
: HandleOperator(handle), Repair::VacuumDelegate(), m_tableWeight(0)
{
}

VacuumHandleOperator::~VacuumHandleOperator()
{
}

#pragma mark - Vacuum
bool VacuumHandleOperator::executeVacuum()
{
    if (!configDatabase()) {
        return false;
    }
    if (!initTables()) {
        return false;
    }
    InnerHandle *handle = getHandle();
    WCTAssert(handle->isOpened());
    auto seqIter = m_tables.find(Syntax::sequenceTable);
    if (seqIter != m_tables.end() && !copyWithouRowidTable(seqIter->second)) {
        return false;
    }
    bool needCheckShadowTable = false;
    for (const auto &table : m_tables) {
        if (table.first.equal(Syntax::sequenceTable)) {
            continue;
        }
        if (needCheckShadowTable) {
            auto exist = handle->tableExists(table.first);
            if (exist.failed()) {
                return false;
            }
            if (exist.value()) {
                continue;
            }
            needCheckShadowTable = false;
        }
        auto attribute = handle->getTableAttribute(kOriginSchema, table.first);
        if (attribute.failed()) {
            return false;
        }
        if (attribute.value().withoutRowid) {
            if (!copyWithouRowidTable(table.second)) {
                return false;
            }
        } else {
            if (attribute.value().isVirtual) {
                needCheckShadowTable = true;
            }
            if (!copyNormalTable(table.second)) {
                return false;
            }
        }
    }
    for (const auto &sql : m_associatedSQLs) {
        if (!handle->execute(sql)) {
            return false;
        }
    }
    handle->close();
    return finishProgress();
}

const Error &VacuumHandleOperator::getVacuumError()
{
    return getHandle()->getError();
}

bool VacuumHandleOperator::configDatabase()
{
    WCTAssert(!m_vacuumPath.empty());
    WCTAssert(!m_originalPath.empty());
    InnerHandle *handle = getHandle();
    WCTAssert(!handle->isOpened());
    handle->setPath(m_vacuumPath);
    if (!handle->open()) {
        return false;
    }
    if (!handle->execute(StatementPragma().pragma(Pragma::journalMode()).to("OFF"))) {
        return false;
    }
    if (!handle->execute(StatementPragma().pragma(Pragma::mmapSize()).to(2147418112))) {
        return false;
    }
    if (!handle->execute(StatementPragma().pragma(Pragma::writableSchema()).to(true))) {
        return false;
    }
    auto attach = StatementAttach().attach(m_originalPath).as(kOriginSchema);
    if (handle->hasCipher()) {
        Data cipher = handle->getRawCipherKey();
        attach.key(UnsafeStringView((const char *) cipher.buffer(), cipher.size()));
    }
    if (!handle->execute(attach)) {
        return false;
    }
    return true;
}

bool VacuumHandleOperator::initTables()
{
    InnerHandle *handle = getHandle();
    WCTAssert(handle->isOpened());
    if (!handle->prepare(StatementSelect()
                         .select(Column::all())
                         .from(TableOrSubquery(Syntax::masterTable).schema(kOriginSchema)))) {
        return false;
    }
    if (!handle->step()) {
        handle->finalize();
        return false;
    }
    while (!handle->done()) {
        auto row = handle->getOneRow();
        WCTAssert(row.size() == 5);
        const StringView &type = row[0].textValue();
        if (type.equal("table")) {
            const StringView &name = row[1].textValue();
            if (!name.hasPrefix(Syntax::builtinTablePrefix)
                || name.equal(Syntax::sequenceTable)) {
                TableInfo info;
                info.name = name;
                info.sql = row[4].textValue();
                WCTAssert(m_tables.find(name) == m_tables.end());
                m_tables.insert_or_assign(name, info);
            }
        } else if (type.equal("index")) {
            const StringView &name = row[1].textValue();
            if (!name.hasPrefix(Syntax::builtinTablePrefix)) {
                const StringView &tblName = row[2].textValue();
                auto iter = m_tables.find(tblName);
                WCTAssert(iter != m_tables.end());
                if (iter != m_tables.end()) {
                    iter->second.indexSQLs.push_back(row[4].textValue());
                }
            }
        } else {
            m_associatedSQLs.push_back(row[4].textValue());
        }
        if (!handle->step()) {
            handle->finalize();
            return false;
        }
    }
    handle->finalize();
    m_tableWeight = m_tables.size() > 0 ? 1.0 / m_tables.size() : 0;
    return true;
}

bool VacuumHandleOperator::createTable(const TableInfo &info)
{
    InnerHandle *handle = getHandle();
    WCTAssert(handle->isOpened());
    if (!handle->execute(info.sql)) {
        return false;
    }
    for (const auto &index : info.indexSQLs) {
        if (!handle->execute(index)) {
            return false;
        }
    }
    return true;
}

bool VacuumHandleOperator::copyWithouRowidTable(const TableInfo &info)
{
    if (!createTable(info)) {
        return false;
    }
    StatementInsert insert = StatementInsert().insertIntoTable(info.name).values(
    StatementSelect().select(Column::all()).from(TableOrSubquery(info.name).schema(kOriginSchema)));
    if (!getHandle()->execute(insert)) {
        return false;
    }
    return increaseProgress(m_tableWeight);
}

bool VacuumHandleOperator::copyNormalTable(const TableInfo &info)
{
    if (!createTable(info)) {
        return false;
    }
    InnerHandle *handle = getHandle();
    WCTAssert(handle->isOpened());
    auto selectMaxRowid = StatementSelect()
                          .select(Column::rowid().max())
                          .from(TableOrSubquery(info.name).schema(kOriginSchema));
    if (!handle->prepare(selectMaxRowid)) {
        return false;
    }
    if (!handle->step()) {
        handle->finalize();
        return false;
    }
    if (handle->done()) {
        handle->finalize();
        return increaseProgress(m_tableWeight);
    }
    int64_t maxRowid = handle->getInteger();
    handle->finalize();

    auto selectMinRowid = StatementSelect()
                          .select(Column::rowid().min())
                          .from(TableOrSubquery(info.name).schema(kOriginSchema));
    if (!handle->prepare(selectMinRowid)) {
        return false;
    }
    if (!handle->step()) {
        handle->finalize();
        return false;
    }
    if (handle->done()) {
        handle->finalize();
        return increaseProgress(m_tableWeight);
    }
    int64_t minRowid = handle->getInteger();
    handle->finalize();

    auto optionalMetas = handle->getTableMeta(Schema(), info.name);
    if (!optionalMetas.succeed()) {
        return false;
    }
    auto &metas = optionalMetas.value();
    Columns columns = { Column::rowid() };
    for (const auto &meta : metas) {
        columns.push_back(Column(meta.name));
    }

    auto insert = StatementInsert().insertIntoTable(info.name).columns(columns).values(
    StatementSelect()
    .select(columns)
    .from(TableOrSubquery(info.name).schema(kOriginSchema))
    .where(Column::rowid() >= BindParameter())
    .order(Column::rowid().asOrder(Order::ASC))
    .limit(VacuumBatchCount));
    if (!handle->prepare(insert)) {
        return false;
    }
    int64_t curMinRowid = minRowid;
    do {
        handle->reset();
        handle->bindInteger(curMinRowid);
        if (!handle->step()) {
            handle->finalize();
            return false;
        }
        double incre = (double) (handle->getLastInsertedRowID() - curMinRowid + 1)
                       / (maxRowid - minRowid + 1) * m_tableWeight;
        WCTAssert(incre >= 0);
        if (!increaseProgress(incre)) {
            handle->finalize();
            return false;
        }
        curMinRowid = handle->getLastInsertedRowID() + 1;
    } while (handle->getChanges() > 0);
    handle->finalize();
    return true;
}

} // namespace WCDB
