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
#include <WCDB/Cell.hpp>
#include <WCDB/Data.hpp>
#include <WCDB/Page.hpp>
#include <WCDB/Pager.hpp>
#include <WCDB/Serialization.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Page::Page(int number_, Pager *pager) : PagerRelated(pager), number(number_)
{
}

std::pair<bool, Page::Type> Page::acquireType()
{
    int type = 0;
    if (m_deserialization.getData().empty()) {
        Data data = m_pager->acquirePageData(number, getOffsetOfHeader(), 1);
        if (data.empty()) {
            return {false, Type::Unknown};
        }
        Deserialization deserialization(data);
        WCTInnerAssert(deserialization.canAdvance(1));
        type = deserialization.advance1ByteInt();
    } else {
        WCTInnerAssert(m_deserialization.isEnough(1));
        m_deserialization.seek(getOffsetOfHeader());
        type = m_deserialization.get1ByteInt(0);
    }
    switch (type) {
        case (int) Type::InteriorIndex:
        case (int) Type::InteriorTable:
        case (int) Type::LeafIndex:
        case (int) Type::LeafTable:
            return {true, (Type) type};
        default:
            return {true, Type::Unknown};
    }
}

const Data &Page::getData() const
{
    WCTInnerAssert(isInitialized());
    return m_data;
}

Page::Type Page::getType() const
{
    WCTInnerAssert(isInitialized());
    return m_type;
}

#pragma mark - Interior Table
std::pair<bool, int> Page::getSubPageno(int index) const
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(index < getSubPageCount());
    WCTInnerAssert(m_type == Type::InteriorTable);
    int offset = index < m_cellPointers.size() ? m_cellPointers[index] : 8;
    if (!m_deserialization.isEnough(offset + 4)) {
        return {false, -1};
    }
    return {true, m_deserialization.get4BytesInt(offset)};
}

int Page::getSubPageCount() const
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(m_type == Type::InteriorTable);
    return (int) m_cellPointers.size() + hasRightMostPageNo();
}

#pragma mark - Leaf Table
Cell Page::getCell(int index)
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(index < getCellCount());
    WCTInnerAssert(m_type == Type::LeafTable);
    return Cell(m_cellPointers[index], this, m_pager);
}

int Page::getCellCount() const
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(m_type == Type::LeafTable);
    return (int) m_cellPointers.size();
}

int Page::getMaxLocal() const
{
    WCTInnerAssert(m_type == Type::LeafTable);
    return m_pager->getUsableSize() - 35;
}

int Page::getMinLocal() const
{
    WCTInnerAssert(m_type == Type::LeafTable);
    return (m_pager->getUsableSize() - 12) * 32 / 255 - 23;
}

#pragma mark - Common
int Page::getOffsetOfCellPointer() const
{
    return m_type == Type::InteriorTable ? 12 : 8;
}

bool Page::hasRightMostPageNo() const
{
    return m_type == Type::InteriorTable;
}

int Page::getOffsetOfHeader() const
{
    return number == 1 ? 100 : 0;
}

#pragma mark - Initializeable
bool Page::doInitialize()
{
    m_data = m_pager->acquirePageData(number);
    if (m_data.empty()) {
        return false;
    }
    m_deserialization.reset(m_data);
    m_deserialization.seek(getOffsetOfHeader());
    WCTInnerAssert(m_deserialization.canAdvance(1));
    int type = m_deserialization.advance1ByteInt();
    switch (type) {
        case (int) Type::InteriorIndex:
        case (int) Type::InteriorTable:
        case (int) Type::LeafIndex:
        case (int) Type::LeafTable:
            m_type = (Type) type;
            break;
        default:
            m_type = Type::Unknown;
            break;
    }
    WCTInnerAssert(m_deserialization.canAdvance(4));
    m_deserialization.advance(2);
    int cellCount = m_deserialization.advance2BytesInt();
    if (cellCount < 0 ||
        cellCount * 2 + getOffsetOfCellPointer() > m_pager->getPageSize()) {
        markPagerAsCorrupted(number, "CellCount");
        return false;
    }
    for (int i = 0; i < cellCount; ++i) {
        int offset = getOffsetOfHeader() + getOffsetOfCellPointer() + i * 2;
        if (!m_deserialization.isEnough(offset + 2)) {
            markPagerAsCorrupted(number, "CellPointer");
            return false;
        }
        int cellPointer = m_deserialization.get2BytesInt(offset);
        if (cellPointer > m_pager->getPageSize()) {
            markPagerAsCorrupted(number, "CellPointer");
            return false;
        }
        m_cellPointers.push_back(cellPointer);
    }
    return true;
}

} //namespace Repair

} //namespace WCDB
