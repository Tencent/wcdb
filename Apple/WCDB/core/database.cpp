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
#include <WCDB/path.hpp>
#include <WCDB/file.hpp>
#include <WCDB/error.hpp>

namespace WCDB {

DataBase::DataBase(const std::string& thePath)
: CoreBase(HandlePool::GetPool(thePath, DataBase::defaultConfigs), CoreType::DataBase)
{
}

bool DataBase::canOpen()
{
    Error innerError;
    return !m_pool->isDrained()||m_pool->fillOne(innerError);
}

void DataBase::close(std::function<void(void)> onClosed)
{
    m_pool->drain(onClosed);
}

void DataBase::blockade()
{
    m_pool->blockade();
}

bool DataBase::isBlockaded()
{
    return m_pool->isBlockaded();
}

void DataBase::unblockade()
{
    m_pool->unblockade();
}

bool DataBase::isOpened() const
{
    return !m_pool->isDrained();
}

void DataBase::purgeFreeHandles()
{
    m_pool->purgeFreeHandles();
}

void DataBase::PurgeFreeHandlesInAllDataBases()
{
    HandlePool::PurgeFreeHandlesInAllPool();
}

void DataBase::setTag(Tag tag)
{
    m_pool->tag = tag;
}

}//namespace WCDB 
