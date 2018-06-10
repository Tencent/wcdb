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
Page::Page(int number_, Pager &pager) : PagerRelated(pager), number(number_)
{
}

std::pair<bool, Page::Type> Page::acquireType()
{
    int type = 0;
    if (!m_deserialization.getData().empty()) {
        Data data = m_pager.acquireData(getOffsetOfHeader(), 1);
        if (data.empty()) {
            return {false, Type::Unknown};
        }
        Deserialization deserialization(data);
        WCTInnerAssert(deserialization.isEnough(1));
        type = deserialization.advance1ByteInt();
    } else {
        m_deserialization.seek(0);
        type = m_deserialization.advance1ByteInt();
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
    WCTInnerAssert(isInited());
    return m_data;
}

Page::Type Page::getType() const
{
    WCTInnerAssert(isInited());
    return m_type;
}

#pragma mark - Interior Table
std::pair<bool, int> Page::getSubPageno(int index) const
{
    WCTInnerAssert(isInited());
    WCTInnerAssert(index < getSubPageCount());
    WCTInnerAssert(m_type == Type::InteriorTable);
    int offset = index < m_cellCount ? getCellPointer(index) : 8;
    if (!m_deserialization.isEnough(offset, 4)) {
        return {false, -1};
    }
    return {true, m_deserialization.get4BytesInt(offset)};
}

int Page::getSubPageCount() const
{
    WCTInnerAssert(isInited());
    WCTInnerAssert(m_type == Type::InteriorTable);
    return m_cellCount + hasRightMostPageNo();
}

#pragma mark - Leaf Table
Cell Page::getCell(int index)
{
    WCTInnerAssert(isInited());
    WCTInnerAssert(index < getCellCount());
    WCTInnerAssert(m_type == Type::LeafTable);
    return Cell(getCellPointer(index), *this, m_pager);
}

int Page::getCellCount() const
{
    WCTInnerAssert(isInited());
    WCTInnerAssert(m_type == Type::LeafTable);
    return m_cellCount;
}

int Page::getMaxLocal() const
{
    WCTInnerAssert(m_type == Type::LeafTable);
    return m_pager.getUsableSize() - 35;
}

int Page::getMinLocal() const
{
    WCTInnerAssert(m_type == Type::LeafTable);
    return (m_pager.getUsableSize() - 12) * 32 / 255 - 23;
}

#pragma mark - Common
int Page::getOffsetOfCellPointer() const
{
    return m_type == Type::InteriorTable ? 12 : 8;
}

int Page::getCellPointer(int cellIndex) const
{
    WCTInnerAssert(isInited());
    WCTInnerAssert(m_deserialization.isEnough(
        getOffsetOfHeader() + getOffsetOfCellPointer() + cellIndex * 2, 2));
    return m_deserialization.get2BytesInt(
        getOffsetOfHeader() + getOffsetOfCellPointer() + cellIndex * 2);
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
    m_data = m_pager.acquirePageData(number);
    if (m_data.empty()) {
        return false;
    }
    m_deserialization.reset(m_data);
    m_deserialization.seek(getOffsetOfHeader());
    WCTInnerAssert(m_deserialization.isEnough(1));
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
    m_deserialization.advance(2);
    WCTInnerAssert(m_deserialization.isEnough(2));
    m_cellCount = m_deserialization.advance2BytesInt();
    if (m_cellCount < 0 ||
        m_cellCount * 2 + getOffsetOfCellPointer() > m_pager.getPageSize()) {
        markAsCorrupted();
        return false;
    }
    return true;
}

} //namespace Repair

} //namespace WCDB
