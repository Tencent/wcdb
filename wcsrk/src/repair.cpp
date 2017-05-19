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

#include "repair.hpp"
#include "parse_statement.hpp"
#include "recover.hpp"

Repair::Repair(const std::string& from, const Assist& assist)
: m_parser(from, assist.pageSize, assist.reserveSize)
, m_count(0)
, m_assist(assist)
{
}

bool Repair::repair(sqlite3* handle, 
                    std::function<void(float)> onProgressUpdated,
                    std::function<void(std::string)> willParseTable)
{
    if (m_assist.backup) {
        for (const auto& iter : *m_assist.backup->infos) {
            m_toParse.insert({iter.first, iter.second});
        }
    }
    
    {
        //parse master
        std::shared_ptr<ParseStatement> statement = m_parser.prepare(1);
        ParseStatement::Result result;
        while ((result=statement->step())!=ParseStatement::Result::Done) {
            if (result==ParseStatement::Result::Page) {
                ++m_count;
            }else if (result==ParseStatement::Result::Cell) {
                auto values = statement->getValues();
                const char* name = values->at(1).getTextValue();
                size_t size = values->at(1).getSize();
                int rootpage = (int)values->at(3).getIntegerValue();
                if (name&&rootpage>0) {
                    std::string realName = std::string(name, size);
                    m_toParse.insert({realName, rootpage});
                }
            }else {
                printf("Error %d\n", result);
            }
        }
    }
    
    
    Recover recover(handle);
    {
        for (const auto& iter :m_toParse) {
            std::shared_ptr<RecoverStatement> recoverStatement = nullptr;
            if (willParseTable) {
                willParseTable(iter.first);
            }
            std::shared_ptr<ParseStatement> statement = m_parser.prepare(iter.second);
            ParseStatement::Result result;
            while ((result=statement->step())!=ParseStatement::Result::Done) {
                if (result==ParseStatement::Result::Page) {
                    ++m_count;
                    if (onProgressUpdated) {
                        onProgressUpdated(m_count/(double)m_parser.pageCount);
                    }
                }else if (result==ParseStatement::Result::Cell) {
                    auto values = statement->getValues();
                    if (!recoverStatement) {
                        recoverStatement = recover.prepare(iter.first, values->size());
                    }
                    recoverStatement->execWithValues(values);
                }else {
                    printf("Error %d\n", result);
                }
            }
        }
    }
        
    return false;
}
