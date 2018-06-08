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

#ifndef Page_hpp
#define Page_hpp

#include <WCDB/Data.hpp>
#include <WCDB/PagerRelated.hpp>
#include <WCDB/Serialization.hpp>

namespace WCDB {

namespace Repair {

class Cell;

class Page : public PagerRelated {
#pragma mark - Initialize
public:
    Page(int number, Pager &pager);

    const int number;

    enum class Type : int {
        Unknown = 0,
        InteriorIndex = 2,
        InteriorTable = 5,
        LeafIndex = 10,
        LeafTable = 13,
    };

    bool initialize();

    std::pair<bool, Type> acquireType();
    Type getType() const;

    const Data &getData() const;

protected:
    Type m_type;
    Deserialization m_deserialization;
    Data m_data;

#pragma mark - Interior Table
public:
    std::pair<bool, int> getSubPageno(int index) const;
    int getSubPageCount() const;

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
    int getCellPointer(int cellIndex) const;
    bool hasRightMostPageNo() const;

    int m_cellCount;
};

} //namespace Repair

} //namespace WCDB

#endif /* Page_hpp */
