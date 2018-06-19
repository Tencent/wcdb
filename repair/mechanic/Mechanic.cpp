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

#include <WCDB/Assertion.hpp>
#include <WCDB/Mechanic.hpp>
#include <WCDB/Page.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Mechanic::Mechanic(const std::string &path) : Repairman(path)
{
}

#pragma mark - Material
void Mechanic::setMaterial(const Material &material)
{
    m_material = material;
}

void Mechanic::setMaterial(Material &&material)
{
    m_material = std::move(material);
}

#pragma mark - Mechanic
void Mechanic::work()
{
    m_pager.setMaxWalFrame(m_material.info.walFrame);
    m_pager.setPageSize(m_material.info.pageSize);
    m_pager.setReservedBytes(m_material.info.reservedBytes);

    if (!m_pager.initialize()) {
        //Actually, the initialization of pager always succeed if material is not corrupted.
        setCriticalError(m_pager.getError());
        markAsFailed();
        return;
    }

    if (!m_pager.isWalDisposed()) {
        if (m_pager.getWalSalt() != m_material.info.walSalt) {
            m_pager.disposeWal();
        }
    }

    if (m_pager.isWalDisposed()) {
        Error error;
        error.level = Error::Level::Notice;
        error.setCode(Error::Code::Mismatch, "Repair");
        error.message = "Skip WAL of non-match salt.";
        error.infos.set("Path", m_pager.getPath());
        Notifier::shared()->notify(error);
    }

    int pageCount = 0;
    for (const auto &element : m_material.contents) {
        pageCount += element.second.pagenos.size();
    }
    setPageWeight(pageCount > 0 ? (double) 1.0 / pageCount : 0);

    markAsAssembling();
    for (const auto &element : m_material.contents) {
        if (getCriticalLevel() >= CriticalLevel::Fatal) {
            break;
        }
        if (!assembleTable(element.first, element.second.sql,
                           element.second.sequence)) {
            continue;
        }
        for (const auto &pageno : element.second.pagenos) {
            if (!crawl(pageno)) {
                tryUpgradeCrawlerError();
            }
        }
    }
    markAsAssembled();
}

#pragma mark - Crawlable
void Mechanic::onCellCrawled(const Cell &cell)
{
    if (getCriticalLevel() >= CriticalLevel::Fatal) {
        return;
    }
    assembleCell(cell);
}

bool Mechanic::willCrawlPage(const Page &page, int)
{
    increaseProgress(getPageWeight());
    if (getCriticalLevel() >= CriticalLevel::Fatal) {
        return false;
    }
    if (page.getType() == Page::Type::LeafTable) {
        markCellCount(page.getCellCount());
        return true;
    }
    markAsCorrupted();
    return false;
}

} //namespace Repair

} //namespace WCDB
