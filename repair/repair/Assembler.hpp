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

#ifndef Assembler_hpp
#define Assembler_hpp

#include <WCDB/RepairError.hpp>
#include <list>
#include <string>

namespace WCDB {

namespace Repair {

class Cell;

class Assembler {
public:
    virtual void markAsAssembling(const std::string &tableName);
    virtual void markAsAssembled();
    bool isAssembling() const;

    virtual bool assembleTable(const std::string &sql);
    virtual bool assembleTableAssociated(const std::list<std::string> &sqls);
    virtual bool assembleCell(const Cell &cell);

protected:
    std::string m_assembling;

#pragma mark - Error
public:
    const RepairError &getError() const;

protected:
    RepairError m_error;
};

class CanDoAssemble {
public:
    CanDoAssemble();

    void setAssembler(const std::shared_ptr<Assembler> &assembler);

protected:
    std::shared_ptr<Assembler> m_assembler;
};

} //namespace Repair

} //namespace WCDB

#endif /* Assembler_hpp */
