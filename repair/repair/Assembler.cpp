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

#include <WCDB/Assembler.hpp>
#include <WCDB/Assertion.hpp>
#include <WCDB/Cell.hpp>

namespace WCDB {

namespace Repair {

void Assembler::markAsAssembling(const std::string &tableName)
{
    WCTInnerAssert(!isAssembling());
    m_assembling = tableName;
}

void Assembler::markAsAssembled()
{
    WCTInnerAssert(isAssembling());
    m_assembling.clear();
}

bool Assembler::assembleTable(const std::string &sql)
{
    WCTInnerAssert(isAssembling());
    return true;
}

bool Assembler::assembleTableAssociated(const std::list<std::string> &sqls)
{
    WCTInnerAssert(isAssembling());
    return true;
}

bool Assembler::assembleCell(const Cell &cell)
{
    WCTInnerAssert(isAssembling());
    return true;
}

bool Assembler::isAssembling() const
{
    return !m_assembling.empty();
}

const RepairError &Assembler::getError() const
{
    return m_error;
}

CanDoAssemble::CanDoAssemble() : m_assembler(nullptr)
{
}

void CanDoAssemble::setAssembler(const std::shared_ptr<Assembler> &assembler)
{
    m_assembler = assembler;
}

} //namespace Repair

} //namespace WCDB
