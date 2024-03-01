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

#include "Page.hpp"
#include "Assertion.hpp"
#include "Cell.hpp"
#include "Pager.hpp"
#include "Serialization.hpp"
#include "StringView.hpp"

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Page::Page(int number_, Pager *pager)
: PagerRelated(pager), number(number_), m_type(Type::UnSet)
{
}

Page::Page(int number_, Pager *pager, const UnsafeData &data)
: PagerRelated(pager), number(number_), m_type(Type::UnSet), m_deserialization(data), m_data(data)
{
}

Page::~Page() = default;

Optional<Page::Type> Page::acquireType()
{
    int type = 0;
    if (m_deserialization.data().empty()) {
        UnsafeData data = m_pager->acquirePageData(number, getOffsetOfHeader(), 1);
        if (data.empty()) {
            return NullOpt;
        }
        Deserialization deserialization(data);
        WCTAssert(deserialization.canAdvance(1));
        type = deserialization.advance1ByteInt();
    } else {
        WCTAssert(m_deserialization.isEnough(1));
        m_deserialization.seek(getOffsetOfHeader());
        type = m_deserialization.get1ByteInt(0);
    }
    return convertToPageType(type);
}

const Data &Page::getData() const
{
    WCTAssert(isInitialized());
    return m_data;
}

Page::Type Page::getType() const
{
    WCTAssert(isInitialized());
    return m_type;
}

bool Page::isInteriorPage() const
{
    WCTAssert(m_type != Type::UnSet);
    return m_type == Type::InteriorTable || m_type == Type::InteriorIndex;
}

bool Page::isLeafPage() const
{
    WCTAssert(m_type != Type::UnSet);
    return m_type == Type::LeafTable || m_type == Type::LeafIndex;
}

bool Page::isTablePage() const
{
    WCTAssert(m_type != Type::UnSet);
    return m_type == Type::InteriorTable || m_type == Type::LeafTable;
}

bool Page::isIndexPage() const
{
    WCTAssert(m_type != Type::UnSet);
    return m_type == Type::InteriorIndex || m_type == Type::LeafIndex;
}

Page::Type Page::convertToPageType(int type)
{
    switch (type) {
    case (int) Type::InteriorIndex:
    case (int) Type::InteriorTable:
    case (int) Type::LeafIndex:
    case (int) Type::LeafTable:
        return (Type) type;
    default:
        return Type::Unknown;
    }
}

#pragma mark - Interior Table
int Page::getSubpageno(int index) const
{
    WCTAssert(isInitialized());
    WCTAssert(index < getNumberOfSubpages());
    WCTAssert(isInteriorPage());
    return m_subpagenos[index];
}

int Page::getNumberOfSubpages() const
{
    WCTAssert(isInitialized());
    WCTAssert(isInteriorPage());
    return (int) m_subpagenos.size();
}

int Page::getRightMostPage() const
{
    WCTAssert(isInitialized());
    WCTAssert(isInteriorPage());
    return m_subpagenos.back();
}

#pragma mark - Leaf Table
Cell Page::getCell(int index)
{
    WCTAssert(isInitialized());
    WCTAssert(index < getNumberOfCells());
    WCTAssert(m_type != Type::InteriorTable);
    return Cell(m_cellPointers[index], this, m_pager);
}

int Page::getNumberOfCells() const
{
    WCTAssert(isInitialized());
    WCTAssert(m_type != Type::InteriorTable);
    return (int) m_cellPointers.size();
}

int Page::getMaxLocal() const
{
    WCTAssert(m_type != Type::InteriorTable);
    if (m_type == Type::LeafTable) {
        return m_pager->getUsableSize() - 35;
    } else {
        return (m_pager->getUsableSize() - 12) * 64 / 255 - 23;
    }
}

int Page::getMinLocal() const
{
    WCTAssert(m_type != Type::InteriorTable);
    return (m_pager->getUsableSize() - 12) * 32 / 255 - 23;
}

#pragma mark - Common
int Page::getOffsetOfCellPointer() const
{
    WCTAssert(m_type != Type::Unknown);
    return isInteriorPage() ? 12 : 8;
}

bool Page::hasRightMostPageNo() const
{
    WCTAssert(m_type != Type::Unknown);
    return isInteriorPage();
}

int Page::getOffsetOfHeader() const
{
    return number == 1 ? 100 : 0;
}

#pragma mark - Initializeable
bool Page::doInitialize()
{
    if (m_data.empty()) {
        m_data = m_pager->acquirePageData(number);
        if (m_data.empty()) {
            return false;
        }
        m_deserialization.reset(m_data);
    }
    m_deserialization.seek(getOffsetOfHeader());
    WCTAssert(m_deserialization.canAdvance(1));
    int type = m_deserialization.advance1ByteInt();
    m_type = convertToPageType(type);
    if (m_type == Type::Unknown) {
        return true;
    }
    WCTAssert(m_deserialization.canAdvance(4));
    m_deserialization.advance(2);
    int numberOfCells = m_deserialization.advance2BytesInt();
    if (numberOfCells < 0) {
        markPagerAsCorrupted(
        number, StringView::formatted("Unexpected CellCount: %d.", numberOfCells));
        return false;
    }
    m_cellPointers.reserve(numberOfCells);
    for (int i = 0; i < numberOfCells; ++i) {
        int offset = getOffsetOfHeader() + getOffsetOfCellPointer() + i * 2;
        if (!m_deserialization.isEnough(offset + 2)) {
            markPagerAsCorrupted(number, "Unable to deserialize CellPointer.");
            return false;
        }
        int cellPointer = m_deserialization.get2BytesInt(offset);
        m_cellPointers.push_back(cellPointer);
    }
    if (m_type == Type::InteriorTable || m_type == Type::InteriorIndex) {
        int numberOfSubpage = (int) m_cellPointers.size() + hasRightMostPageNo();
        m_subpagenos.reserve(numberOfSubpage);
        for (int i = 0; i < numberOfSubpage; ++i) {
            int offset = i < m_cellPointers.size() || !hasRightMostPageNo() ?
                         m_cellPointers[i] :
                         8 + getOffsetOfHeader();
            if (!m_deserialization.isEnough(offset + 4)) {
                markPagerAsCorrupted(number, "Unable to deserialize Subpageno.");
                return false;
            }
            int pageno = m_deserialization.get4BytesInt(offset);
            if (pageno > m_pager->getNumberOfPages()) {
                markPagerAsCorrupted(
                number,
                StringView::formatted("Page number: %d exceeds the page count: %d.",
                                      pageno,
                                      m_pager->getNumberOfPages()));
                return false;
            }
            if (pageno <= 0) {
                markPagerAsCorrupted(
                number, StringView::formatted("Pageno: %d is less than or equal to 0.", pageno));
                return false;
            }
            m_subpagenos.push_back(pageno);
        }
    }
    return true;
}

} //namespace Repair

} //namespace WCDB
