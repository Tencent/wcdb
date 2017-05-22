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

#include <WCDB/database.hpp>
#include <WCDB/utility.hpp>
#include <WCDB/macro.hpp>
#include <sqlcipher/sqlite3.h>
#include <vector>

namespace WCDB {

const std::string DataBase::defaultConfigName = "default";
const std::string DataBase::defaultCipherConfigName = "cipher";
const std::string DataBase::defaultTraceConfigName = "trace";
std::shared_ptr<Trace> DataBase::s_globalTrace = nullptr;

const Configs DataBase::defaultConfigs({
    {
        DataBase::defaultTraceConfigName,
        [](std::shared_ptr<Handle>& handle, Error& error)->bool {
            std::shared_ptr<Trace> trace = s_globalTrace;
            if (trace) {
                handle->setTrace(*trace.get());
            }
            return true;
        },
        0,
    },
    {
        DataBase::defaultCipherConfigName,
        [](std::shared_ptr<Handle>& handle, Error& error)->bool {
            //place holder
            return true;
        },
        1,
    },
    {
        DataBase::defaultConfigName,
        [](std::shared_ptr<Handle>& handle, Error& error)->bool {
                        
            //Locking Mode
            {
                static const StatementPragma s_getLockingMode = StatementPragma().pragma(Pragma::LockingMode);
                static const StatementPragma s_setLockingModeNormal = StatementPragma().pragma(Pragma::LockingMode, "NORMAL");

                //Get Locking Mode
                std::shared_ptr<StatementHandle> statementHandle = handle->prepare(s_getLockingMode);
                if (!statementHandle) {
                    error = handle->getError();
                    return false;
                }
                statementHandle->step();
                if (!statementHandle->isOK()) {
                    error = statementHandle->getError();
                    return false;
                }
                std::string lockingMode = statementHandle->getValue<WCDB::ColumnType::Text>(0);
                statementHandle->finalize();
                
                //Set Locking Mode
                if (strcasecmp(lockingMode.c_str(), "NORMAL")!=0
                    &&!handle->exec(s_setLockingModeNormal)) {
                    error = handle->getError();
                    return false;
                }
            }
            
            //Journal Mode
            {
                static const StatementPragma s_getJournalMode = StatementPragma().pragma(Pragma::JournalMode);
                static const StatementPragma s_setJournalModeWAL = StatementPragma().pragma(Pragma::JournalMode, "WAL");

                //Get Journal Mode
                std::shared_ptr<StatementHandle> statementHandle = handle->prepare(s_getJournalMode);
                if (!statementHandle) {
                    error = handle->getError();
                    return false;
                }
                statementHandle->step();
                if (!statementHandle->isOK()) {
                    error = statementHandle->getError();
                    return false;
                }
                std::string journalMode = statementHandle->getValue<WCDB::ColumnType::Text>(0);
                statementHandle->finalize();
                
                //Set Journal Mode
                if (strcasecmp(journalMode.c_str(), "WAL")!=0
                    &&!handle->exec(s_setJournalModeWAL)) {
                    error = handle->getError();
                    return false;
                }
            }

            //Synchronous
            {
                static const StatementPragma s_setSynchronousFull = StatementPragma().pragma(Pragma::Synchronous, "FULL");

                if (!handle->exec(s_setSynchronousFull)) {
                    error = handle->getError();
                    return false;
                }
            }
            
            //Fullfsync
            {
                static const StatementPragma s_setFullFsync = StatementPragma().pragma(Pragma::Fullfsync, "ON");
                
                if (!handle->exec(s_setFullFsync)) {
                    error = handle->getError();
                    return false;
                }
            }
            
            error.reset();
            return true;
        },
        2,
    },
});

void DataBase::setConfig(const std::string& name, const Config& config, Configs::Order order)
{
    m_pool->setConfig(name, config, order);
}

void DataBase::setConfig(const std::string& name, const Config& config)
{
    m_pool->setConfig(name, config);
}

void DataBase::setCipherKey(const void* key, int size)
{
    std::shared_ptr<std::vector<unsigned char>> keys(new std::vector<unsigned char>(size));
    memcpy(keys->data(), key, size);
    m_pool->setConfig(DataBase::defaultCipherConfigName,
                      [keys](std::shared_ptr<Handle>& handle, Error& error)->bool {
                          bool result = handle->setCipherKey(keys->data(), (int)keys->size());
                          error = handle->getError();
                          return result;
                      });
}
    
void DataBase::setTrace(const Trace& trace)
{
    m_pool->setConfig(DataBase::defaultTraceConfigName,
                      [trace](std::shared_ptr<Handle>& handle, Error& error)->bool {
                          handle->setTrace(trace);
                          return true;
                      });
}
    
void DataBase::SetGlobalTrace(const Trace& globalTrace)
{
    s_globalTrace.reset(new Trace(globalTrace));
}

}//namespace WCDB
