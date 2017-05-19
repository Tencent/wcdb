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

#include "parse_statement.hpp"
#include "value.hpp"
#include "file.hpp"
#include "parse_helper.hpp"

ParseStatement::ParseStatement(const Parser& parser, const std::vector<int>& pages)
: m_parser(parser)
, m_op(Op::Page)
{
    for (const auto& page : pages) {
        m_todo.push(page);
    }
}

ParseStatement::Result ParseStatement::step()
{
    m_result = Result::None;
    while (m_result==Result::None) {
        switch (m_op) {
            case Op::Page: {
                if (m_todo.empty()) {
                    m_result = Result::Done;
                    m_op = Op::None;
                    break;
                }
                m_pageno = m_todo.top();
                m_todo.pop();
                m_pageHeaderOffset = m_pageno==1?100:0;
                File file(m_parser.path, File::Mode::ReadOnly);
                if (!file.isOpened()) {
                    m_result = Result::CantOpen;
                    break;
                }
                size_t pageStart = (m_pageno-1)*m_parser.header.pageSize;
                if (!m_pageData) {
                    m_pageData.reset(new unsigned char[m_parser.header.pageSize]);
                    if (!m_pageData) {
                        m_result = Result::NoMem;
                        break;
                    }
                }
                if (!file.read(m_pageData.get(), pageStart, m_parser.header.pageSize)) {
                    m_result = Result::IOError;
                    break;
                }
                m_pageType = (PageType)ParseHelper::ParseInt32(m_pageData.get()+(m_pageHeaderOffset+pageStart), 1);
                switch (m_pageType) {
                    case PageType::IndexInterior:
                    case PageType::TableInterior:
                    case PageType::IndexLeaf:
                    case PageType::TableLeaf:
                        m_op = Op::CellPointerArray;
                        break;
                    default:
                        m_pageType = PageType::Unknown;
                        m_op = Op::Page;
                        m_result = Result::Page;
                        break;
                }
                m_parsed.insert(m_pageno);
                break;   
            }
            case Op::CellPointerArray: {
                int offsetCellPointerArray = (m_pageType==PageType::TableInterior)?12:8;
                int cells = ParseHelper::ParseInt32(m_pageData.get()+m_pageHeaderOffset+3, 2);
                if (cells<=0
                    ||cells*2+offsetCellPointerArray>m_parser.header.pageSize) {
                    m_result = Result::Corrupt;
                    m_op = Op::Page;
                    break;
                }
                for (int i = 0; i < cells; i++) {
                    int cellPointer = ParseHelper::ParseInt32(m_pageData.get()+(m_pageHeaderOffset+offsetCellPointerArray+i*2), 2);
                    m_cellPointerArray.push_back(cellPointer);
                }
                switch (m_pageType) {
                    case PageType::IndexInterior:
                    case PageType::TableInterior:
                        m_op = Op::InteriorCell;
                        break;
                    case PageType::TableLeaf:
                        m_op = Op::Cell;
                        break;
                    default:
                        m_op = Op::Page;
                        break;
                }
                break;
            }
            case Op::InteriorCell: {
                for (const auto cellPointer : m_cellPointerArray) {
                    int pageno = ParseHelper::ParseInt32(m_pageData.get()+cellPointer, 4);
                    if (m_parsed.find(pageno)==m_parsed.end()) {
                        m_todo.push(pageno);
                    }
                }
                if (m_pageType==PageType::TableInterior) {
                    //has right most pageno
                    int pageno = ParseHelper::ParseInt32(m_pageData.get()+8, 4);
                    if (m_parsed.find(pageno)==m_parsed.end()) {
                        m_todo.push(pageno);
                    }
                }
                m_op = Op::Page;
                m_result = Result::Page;
                break;
            }
            case Op::Cell: {
                if (m_cellPointerArray.empty()) {
                    m_op = Op::Page;
                    break;
                }
                int offset = m_cellPointerArray.front();
                m_cellPointerArray.pop_front();

                {
                    int size;
                    bool parsed;
                    
                    std::tie(m_payloadSize, size, parsed) = ParseHelper::ParseVarint(m_pageData.get()+offset);
                    if (!parsed) {
                        m_result = Result::Corrupt;
                        break;
                    }
                    offset += size;
                    
                    int64_t rowid;//not used
                    std::tie(rowid, size, parsed) = ParseHelper::ParseVarint(m_pageData.get()+offset);
                    if (!parsed) {
                        m_result = Result::Corrupt;
                        break;
                    }
                    offset += size;
                }
                
                // check overflow
                size_t local = 0;
                if (m_payloadSize<=m_parser.header.maxLeaf) {
                    local = m_payloadSize;
                }else {
                    // Since it is a leaf-table page, the max local should be equal to max leaf
                    size_t maxPageLocal = m_parser.header.maxLeaf;
                    size_t minPageLocal = m_parser.header.minLocal;
                    size_t surplus = minPageLocal+(m_payloadSize-minPageLocal)%(m_parser.header.usable-4);
                    if (surplus<=maxPageLocal) {
                        local = surplus;
                    }else {
                        local = minPageLocal;
                    }
                }
                
                // read data
                size_t payloadPointer = 0;
                if (offset+local>m_parser.header.pageSize) {
                    m_result = Result::Corrupt;
                    break;
                }
                m_payload.reset(new unsigned char[m_payloadSize]);
                if (!m_payload) {
                    m_result = Result::NoMem;
                    break;
                }
                
                memcpy(m_payload.get(), m_pageData.get()+offset, local);
                payloadPointer += local;
                
                m_overflowPages.clear();
                if (payloadPointer<m_payloadSize) {
                    int overflowPageno = ParseHelper::ParseInt32(m_pageData.get()+offset+local, 4);
                    File file(m_parser.path, File::Mode::ReadOnly);
                    if (!file.isOpened()) {
                        m_result = Result::CantOpen;
                        break;
                    }
                    std::shared_ptr<unsigned char> overflowPageData(new unsigned char[m_parser.header.pageSize]);
                    if (!overflowPageData) {
                        m_result = Result::NoMem;
                        break;
                    }
                    while (overflowPageno>0) {
                        m_parsed.insert(overflowPageno);
                        m_overflowPages.push_back(overflowPageno);
                        if (!file.read(overflowPageData.get(), (overflowPageno-1)*m_parser.header.pageSize, m_parser.header.pageSize)) {
                            m_result = Result::IOError;
                            break;
                        }
                        
                        //read data
                        size_t overflowSize = m_payloadSize-payloadPointer;
                        if (overflowSize>m_parser.header.pageSize-4) {
                            overflowSize = m_parser.header.pageSize-4;
                        }
                        memcpy(m_payload.get()+payloadPointer, overflowPageData.get()+4, overflowSize);
                        payloadPointer += overflowSize;
                        //iter
                        overflowPageno = ParseHelper::ParseInt32(overflowPageData.get(), 4);
                    }
                }
                if (m_result!=Result::None) {
                    m_result = Result::Cell;
                }
                break;
            }
            default:
                m_result = Result::Done;
                break;
        }
    }
    return m_result;
}

std::shared_ptr<Values> ParseStatement::getValues()
{
    assert(m_result==Result::Cell);
    bool result;
    int size;
    
    off_t columnOffset = 0;
    std::tie(columnOffset, size, result) = ParseHelper::ParseVarint(m_payload.get());
    
    off_t offsetSerialType = size;
    off_t offset = columnOffset;
    const off_t endSerialType = offset;
    const off_t end = m_payloadSize;
    
    int serialTypeSize = 0;
    int serialType = 0;
    size_t valueSize = 0;
    
    std::shared_ptr<Values> values(new Values);
    if (!values) {
        return nullptr;
    }
    while (offset<end
           ||offsetSerialType<endSerialType) {
        std::tie(serialType, serialTypeSize, result) = ParseHelper::ParseVarint(m_payload.get()+offsetSerialType);
        valueSize = ParseHelper::ParseSerialTypeSize(serialType);
        if (serialType==0) {
            values->push_back(Value());
        }else if (serialType<7) {
            int64_t value = ParseHelper::ParseInt64(m_payload.get()+offset, valueSize);
            values->push_back(value);
        }else if (serialType==7) {
            double value = ParseHelper::ParseDouble(m_payload.get()+offset);
            values->push_back(value);
        }else if (serialType==8) {
            values->push_back(0);
        }else if (serialType==9) {
            values->push_back(1);
        }else if (serialType>=12) {
            if (serialType%2==0) {
                values->push_back(Value(m_payload.get()+offset, valueSize));
            }else {
                values->push_back(Value(m_payload.get()+offset, valueSize));//no terminator
            }
        }
        offset += valueSize;
        offsetSerialType += serialTypeSize;
    }
    return values;
}

ParseStatement::PageType ParseStatement::getPageType() const
{
    assert(m_result==Result::Page);
    return m_pageType;
}

const std::vector<int>& ParseStatement::getOverflowPages() const
{
    assert(m_result==Result::Cell);
    return m_overflowPages;
}

int ParseStatement::getPageno() const
{
    assert(m_result==Result::Page);
    return m_pageno;
}
