//
// Created by sanhuazhang on 2018/05/19
//

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

#pragma once

#include "Initializeable.hpp"
#include "MappedData.hpp"
#include "PagerRelated.hpp"
#include "Serialization.hpp"
#include "WCDBOptional.hpp"

namespace WCDB {

namespace Repair {

class Cell;

class Page final : public PagerRelated, public Initializeable {
#pragma mark - Initialize
public:
    Page(int number, Pager *pager);
    Page(int number, Pager *pager, const UnsafeData &data);
    ~Page() override;

    const int number;

    enum Type : char {
        Unknown = 0,
        InteriorIndex = 2,
        InteriorTable = 5,
        LeafIndex = 10,
        LeafTable = 13,
        UnSet = INT8_MAX,
    };

    static Type convertToPageType(int type);
    Optional<Type> acquireType();
    Type getType() const;
    bool isInteriorPage() const;
    bool isLeafPage() const;
    bool isTablePage() const;
    bool isIndexPage() const;

    const Data &getData() const;

protected:
    Type m_type;
    Deserialization m_deserialization;
    Data m_data;

#pragma mark - Interior Table
public:
    int getSubpageno(int index) const;
    int getNumberOfSubpages() const;
    int getRightMostPage() const;

protected:
    std::vector<int> m_subpagenos;

#pragma mark - Leaf Table
public:
    Cell getCell(int index);
    int getNumberOfCells() const;
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
