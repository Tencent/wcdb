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

#include <WCDB/Assertion.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/Mechanic.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Page.hpp>
#include <WCDB/StringView.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Mechanic::Mechanic(const UnsafeStringView &path)
: Repairman(path), m_checksum(0)
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
    m_pager.setMaxWalFrame(m_material->info.numberOfWalFrames);
    m_pager.setPageSize(m_material->info.pageSize);
    m_pager.setReservedBytes(m_material->info.reservedBytes);

    if (!m_material->info.cipherSalt.empty()) {
        WCTAssert(m_cipherDelegate != nullptr);
        m_cipherDelegate->closeCipher();
        if (!m_cipherDelegate->openCipherInMemory()) {
            setCriticalError(m_cipherDelegate->getCipherError());
            return exit(false);
        }
        m_cipherDelegate->setCipherSalt(m_material->info.cipherSalt);
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

    if (m_pager.getNumberOfWalFrames() > 0) {
        if (m_pager.getWalSalt() != m_material->info.walSalt) {
            m_pager.disposeWal();
            Error error(Error::Code::Notice, Error::Level::Notice, "Dispose WAL of non-match salt.");
            error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
            error.infos.insert_or_assign(ErrorStringKeyAssociatePath, m_pager.getPath());
            error.infos.insert_or_assign("WalSalt1", m_pager.getWalSalt().first);
            error.infos.insert_or_assign("WalSalt2", m_pager.getWalSalt().second);
            error.infos.insert_or_assign("MaterialSalt1",
                                         m_material->info.walSalt.first);
            error.infos.insert_or_assign("MaterialSalt2",
                                         m_material->info.walSalt.second);
            Notifier::shared().notify(error);
        }
    }

    int numberOfPages = 0;
    for (const auto &element : m_material->contents) {
        numberOfPages += element.second.verifiedPagenos.size();
    }
    if (numberOfPages == 0) {
        return exit(true);
    }
    setPageWeight(Fraction(1, numberOfPages + m_pager.getDisposedWalPages()));

    if (markAsAssembling()) {
        for (const auto &contentElement : m_material->contents) {
            if (isErrorCritial()) {
                break;
            }
            if (!assembleTable(contentElement.first, contentElement.second.sql)
                || !assembleSequence(contentElement.first, contentElement.second.sequence)) {
                continue;
            }

            for (const auto &verifiedPagenosElement : contentElement.second.verifiedPagenos) {
                if (isErrorCritial()) {
                    break;
                }
                m_checksum = verifiedPagenosElement.second;
                if (!crawl(verifiedPagenosElement.first)) {
                    tryUpgradeCrawlerError();
                }
            }
            assembleAssociatedSQLs(contentElement.second.associatedSQLs);
        }
        markAsAssembled();
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
    increaseProgress(getPageWeight().value());
    if (isErrorCritial()) {
        return false;
    }
    if (page.getType() != Page::Type::LeafTable) {
        markAsCorrupted(page.number,
                        StringView::formatted(
                        "Unexpected page type: %d.", page.getType(), page.getType()));
        return false;
    }
    if (page.getData().hash() != m_checksum) {
        markAsCorrupted(page.number,
                        StringView::formatted(
                        "Mismatched hash: %u for %u.", page.getData().hash(), m_checksum));
        return false;
    }
    markPageAsCounted(page);
    return true;
}

} //namespace Repair

} //namespace WCDB
