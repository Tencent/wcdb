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

#ifndef _WCDB_PAGE_HPP
#define _WCDB_PAGE_HPP

#include <WCDB/Initializeable.hpp>
#include <WCDB/MappedData.hpp>
#include <WCDB/PagerRelated.hpp>
#include <WCDB/Serialization.hpp>

namespace WCDB {

namespace Repair {

class Cell;

class Page final : public PagerRelated, public Initializeable {
#pragma mark - Initialize
public:
    Page(int number, Pager *pager);
    Page(int number, Pager *pager, const MappedData &data);

    const int number;

    enum class Type : int {
        Unknown = 0,
        InteriorIndex = 2,
        InteriorTable = 5,
        LeafIndex = 10,
        LeafTable = 13,
    };

    std::pair<bool, Type> acquireType();
    Type getType() const;

    const MappedData &getData() const;

protected:
    Type m_type;
    Deserialization m_deserialization;
    MappedData m_data;

#pragma mark - Interior Table
public:
    int getSubPageno(int index) const;
    int getSubPageCount() const;

protected:
    std::vector<int> m_subPagenos;

#pragma mark - Leaf Table
public:
    Cell getCell(int index);
    int getCellCount() const;
    int getMaxLocal() const;
    int getMinLocal() const;

#pragma mark - Common
protected:
    int getOffsetOfHeader() const;
    int getOffsetOfCellPointer() const;
    bool hasRightMostPageNo() const;

    std::vector<int> m_cellPointers;
#pragma mark - Initializeable
protected:
    bool doInitialize() override final;
};

} //namespace Repair

} //namespace WCDB

#endif /* _WCDB_PAGE_HPP */
