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

#include <WCDB/FactoryAssemblerHandle.hpp>

namespace WCDB {

void FactoryAssemblerHandle::setPath(const std::string &path)
{
    Handle::setPath(path);
}

const std::string &FactoryAssemblerHandle::getPath() const
{
    return Handle::getPath();
}

bool FactoryAssemblerHandle::markAsAssembling()
{
#warning TODO
    return false;
}

bool FactoryAssemblerHandle::markAsAssembled()
{
    return false;
}

bool FactoryAssemblerHandle::markAsMilestone()
{
    return false;
}

bool FactoryAssemblerHandle::assembleTable(const std::string &tableName,
                                           const std::string &sql)
{
    return false;
}

bool FactoryAssemblerHandle::assembleSequence(const std::string &tableName, int64_t sequence)
{
    return false;
}

bool FactoryAssemblerHandle::assembleCell(const Repair::Cell &cell)
{
    return false;
}

void FactoryAssemblerHandle::markAsDuplicated(bool duplicated)
{
}

bool FactoryAssemblerHandle::assembleSQL(const std::string &sql)
{
    return false;
}

const Error &FactoryAssemblerHandle::getError() const
{
    return Handle::getError();
}

} // namespace WCDB
