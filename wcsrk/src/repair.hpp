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

#ifndef repair_hpp
#define repair_hpp

#include <stdio.h>
#include "backup.hpp"
#include "parser.hpp"
#include "value.hpp"
#include <queue>

typedef struct sqlite3 sqlite3;

class Repair
{
public:
    struct Assist {
        int pageSize;
        int reserveSize;
        std::shared_ptr<Backup> backup;
    };
    Repair(const std::string& from, const Assist& assist);
    
    bool repair(sqlite3* handle, 
                std::function<void(float)> onProgressUpdated, 
                std::function<void(std::string)> willParseTable);
protected:
    Parser m_parser;
    
    Assist m_assist;
    
    std::map<std::string, int> m_toParse;
    
    int m_count;
};

#endif /* repair_hpp */
