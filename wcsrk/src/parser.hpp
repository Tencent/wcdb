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

#ifndef parser_hpp
#define parser_hpp

#include <string>

class ParseStatement;

class Parser
{
public:
    struct Header{
        size_t pageSize;
        size_t reserved;
        size_t usable;
        size_t maxLocal;
        size_t minLocal;
        size_t maxLeaf;
        size_t minLeaf;
    };
    typedef struct Header Header;

    Parser(const std::string& path, int pageSize, int reservedSize);
    
    const std::string path;
    const size_t pageCount;
    const Header header;
    
    std::shared_ptr<ParseStatement> prepare(int rootpage);
    std::shared_ptr<ParseStatement> prepare(const std::vector<int>& pages);
protected:
    static Header GetHeader(int pageSize, int reservedSize);
    static size_t GetPageCount(const std::string& path, int pageSize);    
};

#endif /* parser_hpp */
