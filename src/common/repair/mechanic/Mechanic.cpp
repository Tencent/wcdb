//
// Created by sanhuazhang on 2018/05/19
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

#include "Mechanic.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "Notifier.hpp"
#include "Page.hpp"
#include "StringView.hpp"

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Mechanic::Mechanic(const UnsafeStringView &path)
: Repairman(path), m_checksum(0), m_withoutRowId(false)
{
}

Mechanic::~Mechanic() = default;

#pragma mark - Material
void Mechanic::setMaterial(const Material *material)
{
    m_material = material;
}

#pragma mark - Mechanic
bool Mechanic::work()
{
    WCTAssert(m_material != nullptr);

    auto isEmpty = isEmptyDatabase();
    if (isEmpty.succeed()) {
        if (isEmpty.value()) {
            return exit(true);
        }
    } else {
        return exit(false);
    }

    m_pager.setWalImportance(false);
    m_pager.setNBackFill(m_material->info.nBackFill);
    m_pager.setWalSalt(m_material->info.walSalt);
    m_pager.setPageSize(m_material->info.pageSize);
    m_pager.setReservedBytes(m_material->info.reservedBytes);

    WCTAssert(m_cipherDelegate != nullptr);
    if (m_cipherDelegate->isCipherDB()) {
        m_pager.setCipherContext(m_cipherDelegate->getCipherContext());
    }

    if (!m_pager.initialize()) {
        //Actually, the initialization of pager always succeed if material is not corrupted.
        if (m_pager.getError().isCorruption()) {
            tryUpgradeCrawlerError();
            return exit(true);
        } else {
            setCriticalError(m_pager.getError());
            return exit(false);
        }
    }

    m_pager.disposeWal();

    int numberOfPages = 0;
    for (const auto &element : m_material->contentsMap) {
        numberOfPages += element.second->verifiedPagenos.size();
    }
    // If there are only without-rowid tables in the db, numberOfPages will be 0
    setPageWeight(Fraction(
    1, (numberOfPages == 0 ? 1 : numberOfPages) + m_pager.getDisposedWalPages()));

    if (markAsAssembling()) {
        for (const auto &contentElement : m_material->contentsMap) {
            if (isErrorCritial()) {
                break;
            }
            if (!assembleTable(contentElement.first, contentElement.second->sql)
                || !assembleSequence(contentElement.first,
                                     contentElement.second->sequence)) {
                continue;
            }

            if (!m_assembleDelegate->isAssemblingTableWithoutRowid()) {
                m_withoutRowId = false;
                for (const auto &verifiedPagenosElement : contentElement.second->verifiedPagenos) {
                    m_checksum = verifiedPagenosElement.hash;
                    if (!crawl(verifiedPagenosElement.number)) {
                        tryUpgradeCrawlerError();
                        if (isErrorCritial()) {
                            break;
                        }
                    }
                }
            } else if (contentElement.second->rootPage > 1) {
                m_withoutRowId = true;
                if (!crawl(contentElement.second->rootPage)) {
                    tryUpgradeCrawlerError();
                }
            }
            assembleAssociatedSQLs(contentElement.second->associatedSQLs);
        }
        markAsAssembled();
    }
    if (numberOfPages == 0 && !isErrorCritial() && getScore().value() == 0) {
        increaseScore(1);
        markSegmentedScoreCounted();
    }
    return exit();
}

#pragma mark - Crawlable
void Mechanic::onCellCrawled(const Cell &cell)
{
    if (isErrorCritial()) {
        return;
    }
    assembleCell(cell);
}

bool Mechanic::willCrawlPage(const Page &page, int)
{
    if (page.getType() == Page::Type::LeafTable) {
        increaseProgress(getPageWeight().value());
    }
    if (isErrorCritial()) {
        return false;
    }
    if (page.getType() == Page::Type::LeafTable && !m_withoutRowId) {
        if (page.getData().hash() != m_checksum) {
            markAsCorrupted(page.number,
                            StringView::formatted(
                            "Mismatched hash: %u for %u.", page.getData().hash(), m_checksum));
            return false;
        }
        markPageAsCounted(page);
    } else if (page.isIndexPage() && m_withoutRowId) {
        markPageAsCounted(page);
    } else {
        markAsCorrupted(page.number,
                        StringView::formatted(
                        "Unexpected page type: %d.", page.getType(), page.getType()));
        return false;
    }
    return true;
}

} //namespace Repair

} //namespace WCDB
