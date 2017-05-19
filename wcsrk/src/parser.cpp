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

#include "parser.hpp"
#include "parse_statement.hpp"
#include "file.hpp"

Parser::Parser(const std::string& p, int pageSize, int reservedSize)
: path(p)
, header(GetHeader(pageSize, reservedSize))
, pageCount(GetPageCount(path, pageSize))
{
}

std::shared_ptr<ParseStatement> Parser::prepare(int rootpage)
{
    return std::shared_ptr<ParseStatement>(new ParseStatement(*this, {rootpage}));
}

std::shared_ptr<ParseStatement> Parser::prepare(const std::vector<int>& pages)
{
    return std::shared_ptr<ParseStatement>(new ParseStatement(*this, pages));
}

Parser::Header Parser::GetHeader(int pageSize, int reservedSize)
{
    Header header;
    header.pageSize = pageSize;
    header.reserved = reservedSize;
    header.usable = header.pageSize-header.reserved;
    
    header.maxLocal = (header.usable-12)*64/255-23;
    header.minLocal = (header.usable-12)*32/255-23;
    header.maxLeaf = header.usable-35;
    header.minLeaf = (header.usable-12)*32/255-23;
    return header;
}

size_t Parser::GetPageCount(const std::string& path, int pageSize)
{
    File file(path, File::Mode::ReadOnly);
    if (file.isOpened()) {
        size_t fileSize = file.getSize();
        return fileSize/pageSize;
    }
    return 0;
}
