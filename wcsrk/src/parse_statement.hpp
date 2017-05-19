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

#ifndef parse_statement_hpp
#define parse_statement_hpp

#include "parser.hpp"
#include <list>
#include <vector>
#include <memory>
#include <stack>
#include <set>
#include "value.hpp"

class ParseStatement
{
public:
    enum class Result : int{
        //Error
        IOError = -4,
        NoMem = -3,
        CantOpen = -2,
        Corrupt = -1,
        
        None = 0,
        
        //OK
        Done = 1,
        Page = 2,
        Cell = 3,
    };
    Result step();
    
    //Cell
    std::shared_ptr<Values> getValues();
    const std::vector<int>& getOverflowPages() const;
    
    //Page
    enum class PageType {
        Unknown = -1,
        IndexInterior = 2,
        TableInterior = 5,
        IndexLeaf = 10,
        TableLeaf = 13,
    };
    PageType getPageType() const;
    int getPageno() const;
protected:
    friend std::shared_ptr<ParseStatement> Parser::prepare(int rootpage);    
    friend std::shared_ptr<ParseStatement> Parser::prepare(const std::vector<int>& pages);

    ParseStatement(const Parser& parser, const std::vector<int>& pages);
    std::set<int> m_parsed;//avoid circular parsing
    std::stack<int> m_todo;
    enum class Op {
        None,
        Page,
        CellPointerArray,
        InteriorCell,
        Cell,
    };
    Op m_op;
    Result m_result;
    const Parser& m_parser;
    
    //Page
    std::shared_ptr<unsigned char> m_pageData;
    int m_pageno;
    int m_pageHeaderOffset;
    PageType m_pageType;
    
    //Cell Pointer Array
    std::list<int> m_cellPointerArray;
    
    //Cell
    std::shared_ptr<unsigned char> m_payload;
    size_t m_payloadSize;
    std::vector<int> m_overflowPages;
};

#endif /* parse_statement_hpp */
