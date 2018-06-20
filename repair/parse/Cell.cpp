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
#include <WCDB/Page.hpp>
#include <WCDB/Pager.hpp>
#include <WCDB/Serialization.hpp>
#include <set>

namespace WCDB {

namespace Repair {

Cell::Cell(int pointer, Page *page, Pager *pager)
    : PagerRelated(pager), m_pointer(pointer), m_rowid(0), m_page(page)
{
    WCTInnerAssert(m_page != nullptr);
}

const Page &Cell::getPage() const
{
    return *m_page;
}

int Cell::getLengthOfSerialType(int serialType)
{
    WCTInnerAssert(isSerialTypeSanity(serialType));
    static int s_lengthsOfSerialType[10] = {
        0, //Null
        1, //8-bit integer
        2, //16-bit integer
        3, //24-bit integer
        4, //32-bit integer
        6, //48-bit integer
        8, //64-bit integer
        8, //IEEE 754-2008 64-bit floating point number
        0, //0
        0, //1
    };
    if (serialType <= 10) {
        return s_lengthsOfSerialType[serialType];
    }
    return (serialType - 12 - serialType % 2) / 2;
}

int Cell::isSerialTypeSanity(int serialType)
{
    return serialType > 0 && serialType != 10 && serialType != 11;
}

int64_t Cell::getRowID() const
{
    WCTInnerAssert(isInitialized());
    return m_rowid;
}

int Cell::getCount() const
{
    WCTInnerAssert(isInitialized());
    return (int) m_columns.size();
}

Cell::Type Cell::getValueType(int index) const
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(index < m_columns.size());
    int serialType = m_columns[index].first;
    if (serialType == 0) {
        return Type::Null;
    } else if (serialType == 7) {
        //IEEE 754-2008 64-bit floating point number
        return Type::Real;
    } else if (serialType <= 10) {
        // 1. 8-bit integer
        // 2. 16-bit integer
        // 3. 24-bit integer
        // 4. 32-bit integer
        // 5. 48-bit integer
        // 6. 64-bit integer
        // 8. false
        // 9. true
        return Type::Integer;
    } else {
        return serialType % 2 ? Type::Text : Type::BLOB;
    }
}

int64_t Cell::integerValue(int index) const
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(index < m_columns.size());
    WCTInnerAssert(getValueType(index) == Type::Integer);
    const auto &cell = m_columns[index];
    int length = getLengthOfSerialType(cell.first);
    WCTInnerAssert(m_deserialization.isEnough(cell.second + length));
    switch (length) {
        case 1:
            return m_deserialization.get1ByteInt(cell.second);
        case 2:
            return m_deserialization.get2BytesInt(cell.second);
        case 3:
            return m_deserialization.get3BytesInt(cell.second);
        case 4:
            return m_deserialization.get4BytesInt(cell.second);
        case 6:
            return m_deserialization.get6BytesInt(cell.second);
        case 8:
            return m_deserialization.get8BytesInt(cell.second);
    }
    WCTInnerFatalError();
    return 0;
}

double Cell::doubleValue(int index) const
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(index < m_columns.size());
    WCTInnerAssert(getValueType(index) == Type::Real);
    const auto &cell = m_columns[index];
    WCTInnerAssert(m_deserialization.isEnough(cell.second + 8));
    return m_deserialization.get8BytesDouble(cell.second);
}

std::pair<int, const char *> Cell::textValue(int index) const
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(index < m_columns.size());
    WCTInnerAssert(getValueType(index) == Type::Text);
    const auto &cell = m_columns[index];
    return {getLengthOfSerialType(cell.first),
            reinterpret_cast<const char *>(m_payload.buffer() + cell.second)};
}

std::string Cell::stringValue(int index) const
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(index < m_columns.size());
    WCTInnerAssert(getValueType(index) == Type::Text);
    const auto &cell = m_columns[index];
    int length = getLengthOfSerialType(cell.first);
    WCTInnerAssert(m_deserialization.isEnough(cell.second + length));
    return m_deserialization.getString(cell.second, length);
}

const Data Cell::blobValue(int index) const
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(index < m_columns.size());
    WCTInnerAssert(getValueType(index) == Type::BLOB);
    const auto &cell = m_columns[index];
    int size = getLengthOfSerialType(cell.first);
    WCTInnerAssert(m_deserialization.isEnough(cell.second + size));
    return m_deserialization.getData(cell.second, size);
}

#pragma mark - Initializeable
bool Cell::doInitialize()
{
    Deserialization deserialization(m_page->getData());
    //parse payload size
    deserialization.seek(m_pointer);
    int lengthOfPayloadSize, payloadSize;
    std::tie(lengthOfPayloadSize, payloadSize) =
        deserialization.advanceVarint();
    if (lengthOfPayloadSize == 0) {
        markPagerAsCorrupted(m_page->number, "PayloadSize");
        return false;
    }
    //parse rowid
    int lengthOfRowid;
    std::tie(lengthOfRowid, m_rowid) = deserialization.advanceVarint();
    if (lengthOfRowid == 0) {
        markPagerAsCorrupted(m_page->number, "Rowid");
        return false;
    }
    //parse local
    int localPayloadSize =
        m_page->getMinLocal() +
        (payloadSize - m_page->getMinLocal()) % (m_pager->getUsableSize() - 4);
    if (localPayloadSize > m_page->getMaxLocal()) {
        localPayloadSize = m_page->getMinLocal();
    }
    if (!deserialization.canAdvance(localPayloadSize)) {
        markPagerAsCorrupted(m_page->number, "LocalPayloadSize");
        return false;
    }
    //parse payload
    int offsetOfPayload = m_pointer + lengthOfPayloadSize + lengthOfRowid;
    if (localPayloadSize < payloadSize) {
        //append overflow pages
        deserialization.seek(offsetOfPayload + localPayloadSize);
        if (!deserialization.canAdvance(4)) {
            markPagerAsCorrupted(m_page->number, "OverflowPageno");
            return false;
        }
        int overflowPageno = deserialization.advance4BytesInt();
        m_payload = Data(payloadSize);
        if (m_payload.empty()) {
            assignWithSharedThreadedError();
            return false;
        }
        //fill payload with local data
        memcpy(m_payload.buffer(), m_page->getData().buffer() + offsetOfPayload,
               localPayloadSize);

        int cursorOfPayload = localPayloadSize;
        std::set<int> overflowPagenos;
        while (overflowPageno > 0 && cursorOfPayload < payloadSize) {
            if (overflowPagenos.find(overflowPageno) !=
                    overflowPagenos.end() // redunant overflow page found
                || overflowPageno > m_pager->getPageCount() // pageno exceeds
                ) {
                m_pager->markAsCorrupted(m_page->number, "OverflowPage");
                return false;
            }
            overflowPagenos.insert(overflowPageno);
            //fill payload with overflow data
            Data overflow = m_pager->acquirePageData(overflowPageno);
            if (overflow.empty()) {
                return false;
            }
            int overflowSize = std::min(payloadSize - cursorOfPayload,
                                        m_pager->getUsableSize() - 4);
            WCTInnerAssert(cursorOfPayload + overflowSize <= m_payload.size());
            memcpy(m_payload.buffer() + cursorOfPayload, overflow.buffer() + 4,
                   overflowSize);
            cursorOfPayload += overflowSize;
            //next overflow page
            Deserialization overflowDeserialization(overflow);
            WCTInnerAssert(overflowDeserialization.canAdvance(4));
            overflowPageno = overflowDeserialization.advance4BytesInt();
        }
        if (overflowPageno != 0 || cursorOfPayload != payloadSize) {
            markPagerAsCorrupted(m_page->number, "OverflowPage");
            return false;
        }
    } else {
        //non-overflow
        m_payload =
            m_page->getData().subdata(offsetOfPayload, localPayloadSize);
    }
    m_deserialization.reset(m_payload);
    //parse value offsets
    int lengthOfOffsetOfValues, offsetOfValues;
    std::tie(lengthOfOffsetOfValues, offsetOfValues) =
        m_deserialization.advanceVarint();
    if (lengthOfOffsetOfValues == 0) {
        markPagerAsCorrupted(m_page->number, "CellValueOffset");
        return false;
    }

    int cursorOfSerialTypes = lengthOfOffsetOfValues;
    int cursorOfValues = offsetOfValues;
    const int endOfValues = payloadSize;
    const int endOfSerialTypes = offsetOfValues;

    while (cursorOfSerialTypes < endOfSerialTypes &&
           cursorOfValues < endOfValues) {
        int lengthOfSerialType, serialType;
        m_deserialization.seek(cursorOfSerialTypes);
        std::tie(lengthOfSerialType, serialType) =
            m_deserialization.advanceVarint();
        if (lengthOfSerialType == 0 || !isSerialTypeSanity(serialType)) {
            markPagerAsCorrupted(m_page->number, "SerialType");
            return false;
        }
        cursorOfSerialTypes += lengthOfSerialType;
        m_columns.push_back({serialType, cursorOfValues});
        cursorOfValues += getLengthOfSerialType(serialType);
    }
    if (cursorOfSerialTypes != endOfSerialTypes ||
        cursorOfValues != endOfValues) {
        markPagerAsCorrupted(m_page->number, "Payload");
        return false;
    }
    return true;
}

} //namespace Repair

} //namespace WCDB
